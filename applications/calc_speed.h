/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-03     DELL       the first version
 */
#ifndef APPLICATIONS_CALC_SPEED_H_
#define APPLICATIONS_CALCSPEED_H_

#include "slide_window.h"
#include "drv_encoder.h"

template <typename T, uint16_t N = 1>
class CalcSpeed {
public:
    CalcSpeed(const char* name, rt_uint16_t cnt_per_r):_cpr(cnt_per_r){
        memset(_encode_name, 0x00, 6);
        memcpy(_encode_name, name, strlen(name));
    }
    void setInverse(bool inverse){_inverse = inverse ? -1 : 1;}
    T calc(){
        if(_first){
            _first = false;
            _last_time = rt_tick_get();
            if(strcmp(_encode_name, "htm1") == 0){
                _last_cnt = getCounter(1);
            }else if(strcmp(_encode_name, "htm2") == 0){
                _last_cnt = getCounter(2);
            }
        }
        rt_uint32_t now_cnt;
        if(strcmp(_encode_name, "htm1") == 0){
            now_cnt = getCounter(1);
        }else if(strcmp(_encode_name, "htm2") == 0){
            now_cnt = getCounter(2);
        }
        rt_int32_t diff_cnt = (now_cnt - _last_cnt);
        rt_tick_t now_time = rt_tick_get();
        rt_tick_t diff_t = now_time - _last_time;
        diff_t = diff_t == 0 ? 1 : diff_t;
        double tt = diff_t / RT_TICK_PER_SECOND;
//        printf("..............  %d \r\n", diff_cnt);
        tt = tt == 0 ? 1 : tt;
        T rpm = (diff_cnt * 60.0 * 1000.0 / _cpr) / diff_t;
        _filter.push(rpm);
        _last_cnt = now_cnt;
        _last_time = now_time;
        /****************/
        return _inverse * _filter.getFilterValue();
    }
    bool isValid(){return _filter.isValid();}
private:
    SlideWindow<T, N> _filter;
    char _encode_name[6];
    rt_uint32_t _last_cnt{0};
    rt_tick_t _last_time{0};
    TIM_HandleTypeDef _tim;
    rt_uint16_t _cpr{50000};
    bool _first{true};
    rt_int8_t _inverse{1};
};



#endif /* APPLICATIONS_CALC_SPEED_H_ */
