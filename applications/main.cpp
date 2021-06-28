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

void speedThread(void* parameter)
{
    Pwm test("pwm4");
    Pwm test1("pwm3");
    int pwm = 20000;
//    test.set(1, 100000, 100000 - pwm);
//    test.set(2, 100000,  pwm);
//    test1.set(1, 100000, pwm);
//    test1.set(2, 100000, 100000-pwm);

    rt_int16_t tmp;
    getCounter(&tmp);
    rt_int16_t count = 0;
    count = tmp;
    SlideWindow<double, 10> filter;

    rt_tick_t tmp_t = rt_tick_get();
    rt_tick_t last_t = tmp_t;

    Pid<double> pp(25,10,1,1,100000,-100000);
    while (1)
    {
        /*calc speed*/
        getCounter(&tmp);
        rt_int16_t diff = (tmp - count);
        tmp_t = rt_tick_get();
        rt_tick_t diff_t = tmp_t - last_t;
        diff_t = diff_t == 0 ? 1 : diff_t;
        double rps = ((double)diff / ((double)diff_t * 50));
        double rpm = rps * 60.0;
        filter.push(rpm);
        rt_kprintf("%f|%d|%d\r\n", filter.getFilterValue(), diff, diff_t);

        count = tmp;
        last_t = tmp_t;
        /****************/
        double now = filter.getFilterValue();
        if(filter.isValid()){
            double p = pp.calc(now);
            if(p > 0){
                test.set(1, 100000, 100000 - p);
                test.set(2, 100000, p);
            }else if(p < 0){
                test.set(2, 100000, -p);
                test.set(1, 100000, 100000+p);
            }else{
                test.set(2,100000,0);
                test.set(1,100000,0);
            }
        }else{
           //if()
        }

        /********************/
        //double p = 0;
        //rt_kprintf("%f|%f|%f|%f\r\n", (double)filter.getFilterValue(), (double)diff, (double)diff_t, (double)p);
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

