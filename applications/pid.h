/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-06-27     darboy       the first version
 */

template <typename T>
class Pid {
public:
    Pid(T ref, T p, T i, T d, T max, T min):
        _ref(ref),_p(p),_i(i),_d(d),_max(max),_min(min){}
    ~Pid(){}
    T calc(T feed){
      T out;
      T err = _ref - feed;
      rt_kprintf("err %f\r\n",(double)err);

      _sum += err;
      if(_sum > 0)
          _sum = _sum > 100000 ? _max/50 : _sum;
      else
          _sum = _sum < -100000 ? _min/50 : _sum;
      out = _p*err + _i * _sum + _d * (_pre_err - err);
      _pre_err = err;
      rt_kprintf("out %f %f %f \r\n", (double)out,(double) _p*err,(double)_i * _sum);
      out = out > _max ? _max : out;
      out = out < _min ? _min : out;
      rt_kprintf("out1 %f\r\n", (double)out);
      return out;
    }

private:
    T _pre_err;//{0};
    T _ref;//{0};
    T _feed;//{0};
    T _sum;//{0};
    T _max;//{0};
    T _min;//{0};
    T _p;//{0};
    T _i;//{0};
    T _d;//{0};
};

