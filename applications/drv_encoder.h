#ifndef __ABPHASE_H__
#define __ABPHASE_H__

#include <rthw.h>
#include <rtthread.h>

#ifdef __cplusplus
extern "C"
{
#endif

rt_int16_t getCounter(rt_int16_t cnt);
rt_int32_t getOriginalCounter(rt_int16_t cnt);

#ifdef __cplusplus
}
#endif

#endif
