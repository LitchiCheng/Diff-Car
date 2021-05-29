#ifndef __ABPHASE_H__
#define __ABPHASE_H__

#include <rthw.h>
#include <rtthread.h>

#ifdef __cplusplus
extern "C"
{
#endif

void clearCounter(void);
void getCounter(rt_int16_t *speed);

#ifdef __cplusplus
}
#endif

#endif
