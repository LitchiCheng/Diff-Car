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

void speedThread(void* parameter)
{
    Pwm test("pwm4" ,100000, 1 ,2);
    Pwm test1("pwm3" ,100000, 1 ,2);

    rt_int16_t tmp;
    getCounter(&tmp);
    rt_int16_t count = 0;
    count = tmp;
    SlideWindow<double, 200> filter;

    rt_tick_t tmp_t = rt_tick_get();
    rt_tick_t last_t = tmp_t;

    uint8_t cnt = 0;

    Button key2(GET_PIN(E, 0), false, PIN_MODE_INPUT_PULLUP, 30);
    Button key3(GET_PIN(D, 6), false, PIN_MODE_INPUT_PULLUP, 30);

    double ref = 0;
    Pid<double> pp(ref,500,2,0,100000,-100000);
    bool trigger = true;
    bool trigger1 = true;
    while (1)
    {
        /*add speed*/
        if(key2.isTrigger()){
            ref += 50;
            pp.setRef(ref);
        }
        /*minus speed*/
        if(key3.isTrigger()){
            ref -= 50;
            pp.setRef(ref);
        }
        /*calc speed*/
        getCounter(&tmp);
        rt_int16_t diff = (tmp - count);
        tmp_t = rt_tick_get();
        rt_tick_t diff_t = tmp_t - last_t;
        diff_t = diff_t == 0 ? 1 : diff_t;
        double rps = ((double)diff / ((double)diff_t * 50));
        double rpm = rps * 60.0;
        filter.push(rpm);
        count = tmp;
        last_t = tmp_t;
        /****************/
        double now = filter.getFilterValue();
        double err = now - pp.getRef();
        if(filter.isValid()){
            double p = pp.calc(now);
            if(p > 0){
                test.setCW(p);
                test1.setCW(p);
            }else if(p < 0){
                test.setCCW(-p);
                test1.setCCW(-p);
            }else{
                test.setCW(0);
                test1.setCW(0);
            }
        }else{
           //if()
        }
        /********************/
        if(cnt++ >= 10){
            cnt = 0;
            rt_kprintf("%f|%f|%d\r\n", filter.getFilterValue(), err, diff_t);
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

