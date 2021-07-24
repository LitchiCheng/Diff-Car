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

extern struct rt_messagequeue mq;

void speedThread(void* parameter)
{


    uint8_t cnt = 0;

    Button key2(GET_PIN(E, 0), false, PIN_MODE_INPUT_PULLUP, 30);
    Button key3(GET_PIN(D, 6), false, PIN_MODE_INPUT_PULLUP, 30);

    CalcSpeed<double, 50> left("htm2", 30000);
    //left.setInverse(true);
    CalcSpeed<double, 50> right("htm4", 32000);

    double refl = 0;
    double refr = 0;
    Pid<double> pleft(refl, 800, 2, 1, 100000, -100000);
    Pid<double> pright(refr, 800, 2, 1, 100000, -100000);

    Pwm right_pwm("pwm1" ,100000, 1 ,2);
    Pwm left_pwm("pwm3" ,100000, 1 ,2);
    //left_pwm.setInverse(true);

    double wheel_base = 0.21; //m
    double wheel_radius = 0.035; //m



    while (1)
    {
         static double speed[2] = {0.0, 0.0};
        rt_mq_recv(&mq, &speed, 2*sizeof(double), 0);
        double left_speed = (60 * (2 * speed[0] - wheel_base * speed[1]) * 0.5f) / (2*3.14*wheel_radius);
        double right_speed = (60 * (2 * speed[0] + wheel_base * speed[1]) * 0.5f) / (2*3.14*wheel_radius);

        if(fabs(left_speed) > 200){
            left_speed = left_speed > 0 ? 200 : -200;
        }
        if(fabs(right_speed) > 200){
            right_speed = right_speed > 0 ? 200 : -200;
       }

        pleft.setRef(left_speed);
        pright.setRef(right_speed);

        /****************/
//        left_pwm.setCW(50000);
//        right_pwm.setCW(50000);
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
        if(cnt++ >= 100){
            cnt = 0;
            rt_kprintf("%f|%f|%f|%f\r\n", left.calc(), right.calc(), left_speed, right_speed);
        }
        rt_thread_mdelay(1);
    }
}

int main(void)
{
    rt_thread_t speed_thread_ptr;
    speed_thread_ptr = rt_thread_create("speedThread",
            speedThread, RT_NULL, 8148, 15, 25);
    if (speed_thread_ptr != RT_NULL) rt_thread_startup(speed_thread_ptr);

    while (true)
    {
        rt_thread_mdelay(1000);
    }

    return RT_EOK;
}
