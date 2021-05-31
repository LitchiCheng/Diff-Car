/*
 * 程序清单：这是一个脉冲编码器设备使用例程
 * 例程导出了 pulse_encoder_sample 命令到控制终端
 * 命令调用格式：pulse_encoder_sample
 * 程序功能：每隔 500 ms 读取一次脉冲编码器外设的计数值，然后清空计数值，将读取到的计数值打印出来。
*/
#include <board.h>
#include <rtthread.h>
#include <stm32f1xx.h>
#include "drv_common.h"
#define DBG_TAG "board"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

#include <drv_encoder.h>

static int sss(int argc, char *argv[])
{
    rt_err_t ret = RT_EOK;
    rt_int16_t count;
    getCounter(&count);
    LOG_I("count is %d\r\n", count);
    return ret;
}

/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(sss, pulse encoder sample);
