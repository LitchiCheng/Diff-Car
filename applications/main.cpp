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

void speedThread(void* parameter)
{
    Pwm test("pwm4");
    Pwm test1("pwm3");
    int pwm = 10000;
    test.set(1, 100000, pwm);
    test.set(2, 100000, 100000-pwm);
    test1.set(1, 100000, pwm);
    test1.set(2, 100000, 100000-pwm);

    rt_int16_t tmp;
    getCounter(&tmp);
    rt_int16_t count = 0;
    count = tmp;
    SlideWindow<rt_int16_t, 150> filter;

    while (1)
    {
        getCounter(&tmp);
        rt_int16_t cnt_per_s = (tmp - count);
        filter.push(cnt_per_s);
        rt_kprintf("%d %d %d\r\n", filter.getFilterValue(), tmp, count);
        count = tmp;
        rt_thread_mdelay(1);
    }
}

int main(void)
{
    rt_thread_t speed_thread_ptr;
    speed_thread_ptr = rt_thread_create("speedThread",
            speedThread, RT_NULL, 1024, 9, 25);
    if (speed_thread_ptr != RT_NULL) rt_thread_startup(speed_thread_ptr);

    while (true)
    {

        rt_thread_mdelay(1000);
    }

    return RT_EOK;
}

