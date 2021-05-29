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

void speedThread(void* parameter)
{
    rt_int16_t count2;
    bool first_in = true;
    while (1)
    {
        rt_int16_t tmp;
        getCounter(&tmp);
        if(first_in){
            first_in = false;
            count2 = tmp;
        }
        rt_int16_t cnt_per_s = (tmp - count2) * 1000 / 10;
        rt_kprintf("%d\r\n", cnt_per_s*1000);
        count2 = tmp;
        rt_thread_mdelay(10);
    }
}

int main(void)
{
    rt_thread_t speed_thread_ptr;
    rt_err_t result;
    /* 创建线程 2 */
    /* 线程的入口是 thread2_entry, 参数是 RT_NULL
     * 栈空间是 512，优先级是 250，时间片是 25 个 OS Tick
     */
    speed_thread_ptr = rt_thread_create("speedThread",
            speedThread, RT_NULL,
                                1024, 10, 25);

    /* 启动线程 */
    if (speed_thread_ptr != RT_NULL) rt_thread_startup(speed_thread_ptr);

    Pwm test("pwm4");
    Pwm test1("pwm3");
    int count = 1;
    while (true)
    {
        count = 30000;
        test.set(1, 100000, count);
        test.set(2, 100000, 100000-count);
        test1.set(1, 100000, count);
        test1.set(2, 100000, 100000-count);
        rt_thread_mdelay(1000);
    }

    return RT_EOK;
}

