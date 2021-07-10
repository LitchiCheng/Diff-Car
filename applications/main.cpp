/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-18     RT-Thread    first version
 */

#include <rtthread.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>
#include <stdint.h>

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include <rtthread.h>
#include <rtdevice.h>

#include "pwm.h"
#include "drv_encoder.h"
#include "slide_window.h"
#include "SEGGER_RTT.h"
#include <stdlib.h>

#include "pid.h"
#include <math.h>

#include "button.h"

#include "calc_speed.h"

void udpclient(int argc, char **argv);
double vx = 0;
double vw = 0;

void speedThread(void* parameter)
{


    uint8_t cnt = 0;

    Button key2(GET_PIN(E, 0), false, PIN_MODE_INPUT_PULLUP, 30);
    Button key3(GET_PIN(D, 6), false, PIN_MODE_INPUT_PULLUP, 30);

    CalcSpeed<double, 10> left("htm2", 50000);
    left.setInverse(true);
    CalcSpeed<double, 10> right("htm1", 50000);

    double refl = 0;
    double refr = 0;
    Pid<double> pleft(refl, 500, 2, 1, 100000, -100000);
    Pid<double> pright(refr, 500, 2, 1, 100000, -100000);

    Pwm right_pwm("pwm4" ,100000, 1 ,2);
    Pwm left_pwm("pwm3" ,100000, 1 ,2);
    //left_pwm.setInverse(true);

    while (1)
    {
        /*add speed*/
        if(key2.isTrigger())
        {
            refl += 50;
            pleft.setRef(refl);
            refr += 50;
            pright.setRef(refr);
        }
//        /*minus speed*/
//        if(key3.isTrigger())
//        {
//            refl -= 50;
//            pleft.setRef(refl);
//            refr -= 50;
//            pright.setRef(refr);
//        }
//        vw *= 50;
//        vx *= 50;
//        pleft.setRef(vx);
//        pright.setRef(vx);
        /****************/
        //if(left.isValid())
        {
            double p = pleft.calc(left.calc());
            if(p > 0){
                left_pwm.setCW(p);
            }else if(p < 0){
                left_pwm.setCCW(-p);
            }else{
                left_pwm.setCW(0);
            }
        }
        //if(right.isValid())
        {
            double p = pright.calc(right.calc());
            if(p > 0){
                right_pwm.setCW(p);
            }else if(p < 0){
                right_pwm.setCCW(-p);
            }else{
                right_pwm.setCW(0);
            }
        }
        /********************/
        if(cnt++ >= 10){
            cnt = 0;
            rt_kprintf("%f|%f|%d\r\n", left.calc(), right.calc(), 0);
        }
        rt_thread_mdelay(1);
    }
}

int main(void)
{
    rt_thread_t speed_thread_ptr;
    speed_thread_ptr = rt_thread_create("speedThread",
            speedThread, RT_NULL, 40960, 15, 25);
    if (speed_thread_ptr != RT_NULL) rt_thread_startup(speed_thread_ptr);

    while (true)
    {
        rt_thread_mdelay(1000);
    }

    return RT_EOK;
}

#include <sys/socket.h> /* 使用BSD socket，需要包含sockets.h头文件 */
#include <netdb.h>
#define BUFSZ   1024
void udpclient(int argc, char **argv)
{
    int sock, port, count;
    struct hostent *host;
    struct sockaddr_in server_addr;
    const char *url;
    int bytes_read;
    char *recv_data;
    socklen_t addr_len;
    /* 分配接收用的数据缓冲 */
    recv_data = (char*)rt_malloc(BUFSZ);
    if (recv_data == RT_NULL)
    {
        /* 分配内存失败，返回 */
        rt_kprintf("No memory\n");
        return;
    }


    url = argv[1];
    port = 777;//strtoul(argv[2], 0, 10);

    /* 通过函数入口参数url获得host地址（如果是域名，会做域名解析） */
    host = (struct hostent *) gethostbyname("192.168.31.157");

    /* 创建一个socket，类型是SOCK_DGRAM，UDP类型 */
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        rt_kprintf("Socket error\n");
        return;
    }

    /* 初始化预连接的服务端地址 */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *((struct in_addr *)host->h_addr);
    rt_memset(&(server_addr.sin_zero), 0, sizeof(server_addr.sin_zero));

    /* 设置阻塞超时 */
    struct timeval timeOut;
    timeOut.tv_sec = 1;                 //设置5s超时
    timeOut.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeOut, sizeof(timeOut)) < 0)
    {
        printf("time out setting failed\n");
    }

    while(1){
        char send_data[] = "i am udp client";
        sendto(sock, send_data, strlen(send_data), 0, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
        bytes_read = recv(sock, recv_data, BUFSZ-1, 0);
        recv_data[bytes_read] = '\0'; /* 把末端清零 */
        rt_kprintf("check recv %s\r\n", recv_data);
        /* 如果接收数据是exit，退出 */
        if (strcmp(recv_data, "go") == 0)
        {
            break;
        }
    }

    /* 总计发送count次数据 */
    while (1)
    {
        bytes_read = recv(sock, recv_data, BUFSZ-1, 0);
        /* 如果接收数据是exit，退出 */
          vx = *(double*)recv_data;
          vw = *(double*)(recv_data+sizeof(double));
        rt_kprintf("vx %f vw %f\r\n", vx, vw);
        if (strcmp(recv_data, "exit") == 0)
        {
            closesocket(sock);
            /* 释放接收用的数据缓冲 */
            rt_free(recv_data);
            break;
        }
    }

    /* 关闭这个socket */
    closesocket(sock);
}

MSH_CMD_EXPORT(udpclient, ssss);
