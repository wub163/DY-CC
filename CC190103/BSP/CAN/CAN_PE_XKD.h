/*******************************************88888

鑫科德电池
************************************************/
#ifndef __CAN_PE_XKD_H
#define __CAN_PE_XKD_H


#include "sys.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "queue.h"
#include "can.h"
#include "main.h"
#define CANPEID 0x16
#pragma   pack(1)
typedef struct _CANPE_ST{
      u8 txok;         //当前包通讯正常。
      u8 state;        //当前状态
      u16 sjlc;             //数据量程1，*10   0，*1
      u16 dqdy;             //当前电压mv
      s16 dqdl;             //当前电流ma
      u16 csrl;             //初始容量
      u16 dqrl;             //当前容量
      u16 syrl;             //剩余容量
      u8  sybfb;           //剩余电量百分比
      u16 cdxh;         //充电循环
      u16 day;          //使用天数
      u16 minutes;      //使用分钟
      u8  sccd;         //上次充电情况
      s8  pjwd;         //平均温度。
      u8  POWER_err;      //通讯错误累计。
}xCANPE_D;
#pragma   pack()
extern QueueHandle_t xCANPE;  //从中断接收

extern void CANPE_task(void *pv);//启动读卡器命令

extern   xCANPE_D CANPE;
   
#endif