/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-20     DELL       the first version
 */
#ifndef APPLICATIONS_PWM_H_
#define APPLICATIONS_PWM_H_

#include <rtdevice.h>
#include <stdio.h>
#include <string.h>
#include <rtthread.h>
#include <board.h>

class Pwm
{
public:
    Pwm(){}
    Pwm(const char* name){
        setPwmName(name);
        init();
    }
    ~Pwm(){}
    void setPwmName(const char* name){
        memcpy(_pwm_dev_name, name, strlen(name));
        rt_kprintf("pwm_dev_name is %s\n", _pwm_dev_name);
    }
    void init(){
        /* 查找设备 */
        _pwm_dev = (struct rt_device_pwm *)rt_device_find(_pwm_dev_name);
        if (_pwm_dev == RT_NULL)
        {
            rt_kprintf("pwm sample run failed! can't find %s device!\n", _pwm_dev_name);
            return;
        }
        rt_pwm_enable(_pwm_dev, 0);
        rt_pwm_enable(_pwm_dev, 1);
        rt_pwm_enable(_pwm_dev, 2);
        rt_pwm_enable(_pwm_dev, 3);
    }
    void set(rt_uint8_t channel, rt_uint32_t period, rt_uint32_t pulse){
        /* 设置PWM周期和脉冲宽度默认值 */
        rt_pwm_set(_pwm_dev, channel, period, pulse);
        _last_channel = channel;
        _last_period = period;
        _last_pulse = pulse;
    }
private:
    int8_t _last_channel{1};
    char _pwm_dev_name[6];
    struct rt_device_pwm *_pwm_dev{RT_NULL};      /* PWM设备句柄 */
    rt_uint32_t _last_period{0};                /* 周期，单位为纳秒ns */
    rt_uint32_t _last_pulse{0};             /* PWM脉冲宽度值，单位为纳秒ns */
};

#endif /* APPLICATIONS_PWM_H_ */
