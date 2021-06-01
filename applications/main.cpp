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

void speedThread(void* parameter)
{
    Pwm test("pwm4");
    Pwm test1("pwm3");
    int pwm = 5000;
    test.set(1, 100000, pwm);
    test.set(2, 100000, 100000-pwm);
    test1.set(1, 100000, pwm);
    test1.set(2, 100000, 100000-pwm);

    rt_int16_t tmp;
    getCounter(&tmp);
    rt_int16_t count = 0;
    count = tmp;
    SlideWindow<double, 50> filter;

    rt_tick_t tmp_t = rt_tick_get();
    rt_tick_t last_t = tmp_t;
    while (1)
    {
        getCounter(&tmp);
        rt_int16_t diff = (tmp - count);
        diff = abs(diff);
        tmp_t = rt_tick_get();
        rt_tick_t diff_t = tmp_t - last_t;

        double rps = diff / (50*diff_t);
        filter.push(rps);
        rt_kprintf("%f|%f|%f\r\n", filter.getFilterValue(), (double)diff, (double)diff_t);
        count = tmp;
        last_t = tmp_t;
        rt_thread_mdelay(10);
    }
}

int main(void)
{
    rt_thread_t speed_thread_ptr;
    speed_thread_ptr = rt_thread_create("speedThread",
            speedThread, RT_NULL, 4096, 9, 25);
    if (speed_thread_ptr != RT_NULL) rt_thread_startup(speed_thread_ptr);

    while (true)
    {
        rt_thread_mdelay(1000);
    }

    return RT_EOK;
}

