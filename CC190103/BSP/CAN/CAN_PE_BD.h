/*******************************************88888

博达电池
************************************************/
#include "sys.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "queue.h"
#include "can.h"

#define CANPEID 0x16
typedef struct _CANPE_ST{
      u16 DY[17];         //电压0.1v  0，总电压 1-16 单体电压。
      s16 DL;             //电流0.1a
      u8 cap;             //电池电量0-100
      u8 temp;             //电池温度0对应-40度
      u8 stat;             //电池状态1，充电中 0，未充电
      u8 num;             //电池串数
      u8 DRVERR;
}xCANPE_D;

extern QueueHandle_t xCANPE;  //从中断接收

extern void CANPE_task(void *pv);//启动读卡器命令

extern   xCANPE_D CANPE;
   