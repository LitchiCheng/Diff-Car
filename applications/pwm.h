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
    Pwm(const char* name, rt_uint32_t period, rt_uint8_t channel_cw, rt_uint8_t channel_ccw):
        _period(period), _channel_cw(channel_cw),_channel_ccw(channel_ccw)
    {
        memset(_pwm_dev_name, 0x00, 6);
        setPwmName(name);
        init();
    }
    ~Pwm(){}
    void setPwmName(const char* name){
        memcpy(_pwm_dev_name, name, strlen(name));
        rt_kprintf("pwm_dev_name is %s\n", _pwm_dev_name);
    }
    void setInverse(bool inverse){_inverse = inverse;}
    void init(){
        /* 查找设备 */
        _pwm_dev = (struct rt_device_pwm *)rt_device_find(_pwm_dev_name);
        if (_pwm_dev == RT_NULL)
        {
            rt_kprintf("pwm run failed! can't find %s  device!\n", _pwm_dev_name);
            return;
        }
        rt_pwm_enable(_pwm_dev, 0);
        rt_pwm_enable(_pwm_dev, 1);
        rt_pwm_enable(_pwm_dev, 2);
        rt_pwm_enable(_pwm_dev, 3);
    }
    void setCW(rt_uint32_t pulse){
        /* 设置PWM周期和脉冲宽度默认值 */
        if(!_inverse){
            rt_pwm_set(_pwm_dev, _channel_cw, _period, pulse);
            rt_pwm_set(_pwm_dev, _channel_ccw, _period, 0);
        }else{
            rt_pwm_set(_pwm_dev, _channel_ccw, _period, pulse);
            rt_pwm_set(_pwm_dev, _channel_cw, _period, 0);
        }

    }
    void setCCW(rt_uint32_t pulse){
        /* 设置PWM周期和脉冲宽度默认值 *//* PWM脉冲宽度值，单位为纳秒ns */
        if(!_inverse){
            rt_pwm_set(_pwm_dev, _channel_ccw, _period, pulse);
            rt_pwm_set(_pwm_dev, _channel_cw, _period, 0);
        }else{
            rt_pwm_set(_pwm_dev, _channel_cw, _period, pulse);
            rt_pwm_set(_pwm_dev, _channel_ccw, _period, 0);
        }

    }
private:
    int8_t _last_channel{1};
    char _pwm_dev_name[6];
    struct rt_device_pwm *_pwm_dev{RT_NULL};      /* PWM设备句柄 */
    rt_uint32_t _period{0};                /* 周期，单位为纳秒ns */
    rt_uint8_t _channel_cw{0};
    rt_uint8_t _channel_ccw{0};
    bool _inverse{false};
};

#endif /* APPLICATIONS_PWM_H_ */
