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

int main(void)
{
    Pwm test("pwm4");
    Pwm test1("pwm3");
    int count = 1;
    while (true)
    {
        count += 5000;
        test.set(1, 100000, count);
        test.set(2, 100000, 100000-count);
        test1.set(1, 100000, count);
        test1.set(2, 100000, 100000-count);
        rt_thread_mdelay(1000);
    }

    return RT_EOK;
}

