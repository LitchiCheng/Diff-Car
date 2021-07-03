/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-03     DELL       the first version
 */
#ifndef APPLICATIONS_BUTTON_H_
#define APPLICATIONS_BUTTON_H_

class Button {
public:
    Button(rt_base_t gpio, bool state, uint8_t mode, uint16_t jitter = 0):
        _gpio(gpio), _state(state), _mode(mode), _jitter(jitter){
        rt_pin_mode(gpio, mode);
    }
    ~Button(){}
    bool isTrigger(){
        if(rt_pin_read(_gpio) == _state && !_start){
            _start = true;
            _last_t = rt_tick_get();
            _triggered = false;
        }else if(rt_pin_read(_gpio) != _state){
            _start = false;
            return false;
        }
        rt_uint32_t now = rt_tick_get();
        if(((now - _last_t) * 1000 / RT_TICK_PER_SECOND  >= _jitter) && !_triggered){
            _triggered = true;
            return true;
        }else{
            return false;
        }
    }
    bool isPress(){
        if(rt_pin_read(_gpio) == _state && !_start){
            _start = true;
            _last_t = rt_tick_get();
            _triggered = false;
        }else if(rt_pin_read(_gpio) != _state){
            _start = false;
            return false;
        }
        rt_uint32_t now = rt_tick_get();
        if(((now - _last_t) * 1000 / RT_TICK_PER_SECOND  >= _jitter)){
            return true;
        }else{
            return false;
        }
    }
private:
    rt_base_t _gpio;
    bool _state;
    uint8_t _mode;
    uint16_t _jitter;
    rt_tick_t _last_t;
    bool _start{false};
    bool _triggered{false};
};




#endif /* APPLICATIONS_BUTTON_H_ */
