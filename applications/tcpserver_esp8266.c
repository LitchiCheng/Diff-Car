/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-11     Litchi       the first version
 */

#include<stdlib.h>
#include<string.h>
#include<rtthread.h>
#include<at.h>

#define LOG_TAG              "tcpserver"
#include <at_log.h>

/* 消息队列控制块 */
struct rt_messagequeue mq;
/* 消息队列中用到的放置消息的内存池 */
static rt_uint8_t msg_pool[2*sizeof(double)];


static void recvCb(const char *data, rt_size_t size)
{
    char* buff = rt_malloc(size);
    memcpy(buff, data, size);
    rt_kprintf("%s %d\r\n", buff, size);
    /* 发送紧急消息到消息队列中 */
    if(size < 2*sizeof(double)){
        rt_free(buff);
        return;
    }

    double vx = *(double*)(buff);
    double vw = *(double*)(buff+sizeof(double));
    rt_kprintf("vx %f vw %f\r\n", vx, vw);
    int result = rt_mq_urgent(&mq, buff, 2*sizeof(double));
    if (result != RT_EOK)
    {
        rt_kprintf("rt_mq_urgent ERR\n");
    }
    rt_free(buff);
}

static void urc_recv_func(struct at_client *client, const char *data, rt_size_t size)
{
    int device_socket = 0;
    rt_int32_t timeout = 0;
    rt_size_t bfsz = 0, temp_size = 0;
    char *recv_buf = RT_NULL, temp[8] = {0};
    RT_ASSERT(data && size);

    /* get the at deveice socket and receive buffer size by receive data */
    sscanf(data, "+IPD,%d,%d:", &device_socket, (int *) &bfsz);

    /* set receive timeout by receive buffer length, not less than 10ms */
    timeout = bfsz > 10 ? bfsz : 10;

    if (device_socket < 0 || bfsz == 0)
        return;

    recv_buf = (char *) rt_calloc(1, bfsz);
    if (recv_buf == RT_NULL)
    {
        LOG_E("no memory receive buffer(%d).", bfsz);
        /* read and clean the coming data */
        while (temp_size < bfsz)
        {
            if (bfsz - temp_size > sizeof(temp))
            {
                at_client_obj_recv(client, temp, sizeof(temp), timeout);
            }
            else
            {
                at_client_obj_recv(client, temp, bfsz - temp_size, timeout);
            }
            temp_size += sizeof(temp);
        }
        return;
    }

    /* sync receive data */
    if (at_client_obj_recv(client, recv_buf, bfsz, timeout) != bfsz)
    {
        LOG_E("urc device receive size(%d) data failed.", bfsz);
        rt_free(recv_buf);
        return;
    }

    /* notice the receive buffer and buffer size */
    recvCb(recv_buf, bfsz);
}

static const struct at_urc urc_table[] =
{
    {"+IPD",             ":",              urc_recv_func},
};

/* AT+CIFSR            Query local IP address and MAC */
int test(int argc,char**argv)
{
    at_response_t resp = RT_NULL;
    int result =0;

    resp = at_create_resp(1024,0, rt_tick_from_millisecond(5000));
    if(resp == RT_NULL)
    {
        LOG_E("No memory for response structure!");
        return-2;
    }

    /* 关闭回显功能 */
    //at_exec_cmd(resp,"ATE0");

    //初始化为server
    result = at_exec_cmd(resp,"AT+CWMODE=1");
    result = at_exec_cmd(resp,"AT+RST");
    char connect_wife[] = "AT+CWJAP=\"Redmi_DD77\",\"alice520\"";
    result = at_exec_cmd(resp, connect_wife);
    result = at_exec_cmd(resp,"AT+CIPMUX=1");
    result = at_exec_cmd(resp,"AT+CIPSERVER=1,8000");

    /* AT  Client 发送查询 IP 地址命令并接收 AT Server 响应 */
    /* 响应数据及信息存放在 resp 结构体中 */
    result = at_exec_cmd(resp,"AT+CIFSR");
    if(result != RT_EOK)
    {
        LOG_E("AT client send commands failed or return response error!");
        goto __exit;
    }

    /* 按行数循环打印接收到的响应数据 */
    {
        const char*line_buffer = RT_NULL;
        LOG_D("Response buffer");
        for(rt_size_t line_num =1; line_num <= resp->line_counts; line_num++)
        {
            if((line_buffer = at_resp_get_line(resp, line_num))!= RT_NULL)
            {
                LOG_I("line %d buffer : %s", line_num, line_buffer);
            }
            else
            {
                LOG_E("Parse line buffer error!");
            }
        }
    }
    /* 按自定义表达式（sscanf 解析方式）解析数据，得到对应数据 */
    {
        char resp_arg[AT_CMD_MAX_LEN]={0};
        /* 自定义数据解析表达式 ，用于解析两双引号之间字符串信息 */
        const char* resp_expr ="%*[^\"]\"%[^\"]\"";

        LOG_D(" Parse arguments");
        /* 解析响应数据中第一行数据，得到对应 IP 地址 */
        if(at_resp_parse_line_args(resp,2, resp_expr, resp_arg)==1)
        {
            LOG_I("Station IP  : %s", resp_arg);
            memset(resp_arg,0x00, AT_CMD_MAX_LEN);
        }
        else
        {
            LOG_E("Parse error, current line buff : %s", at_resp_get_line(resp,4));
        }

        /* 解析响应数据中第二行数据，得到对应 MAC 地址 */
        if(at_resp_parse_line_args(resp,3, resp_expr, resp_arg)==1)
        {
            LOG_I("Station MAC : %s", resp_arg);
        }
        else
        {
            LOG_E("Parse error, current line buff : %s", at_resp_get_line(resp,5));
            goto __exit;
        }
    }

__exit:
    if(resp)
    {
        /* 删除 resp 结构体 */
        at_delete_resp(resp);
    }

    return result;
}

/* 设置当前 AT 客户端最大支持的一次接收数据的长度 */
#define AT_CLIENT_RECV_BUFF_LEN         512
int at_client_test_init()
{
    /* 初始化消息队列 */
        int result = rt_mq_init(&mq,
                        "mqt",
                        &msg_pool[0],             /* 内存池指向 msg_pool */
                        2*sizeof(double),                        /* 每个消息的大小是 1 字节 */
                        sizeof(msg_pool),         /* 内存池的大小是 msg_pool 的大小 */
                        RT_IPC_FLAG_FIFO);        /* 如果有多个线程等待，按照先来先得到的方法分配消息 */

    char argv[] = "uart2";
    at_client_init(argv, AT_CLIENT_RECV_BUFF_LEN);

    /* register URC data execution function  */
    at_obj_set_urc_table(at_client_get_first(), urc_table, sizeof(urc_table) / sizeof(urc_table[0]));

    test(0, RT_NULL);
    return RT_EOK;
}

#ifdef FINSH_USING_MSH
#include<finsh.h>
/* 添加 AT Client 测试命令到 shell  */
MSH_CMD_EXPORT(test, AT client send cmd andget response);
/* 添加 AT Client 初始化命令到 shell  */
INIT_APP_EXPORT(at_client_test_init);
#endif

