#ifndef _MOTOR_RMDS_TASK_H_
#define _MOTOR_RMDS_TASK_H_

#include "sys.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "queue.h"
#include "can.h"

#define GearScale 23     //传动比
#define WheelPeri 240   //轮直径
#define MotorEncoder 5000 //周脉冲数
#define MotorLenth  400 //轮距
#define MotoFlagForward0   1
#define MotoFlagForward1   -1    //左右轮电机转向

#define mod 6//模式选择：电流速度。
//1，开环            P1P2                  开环控制pwm
//2，电流            P1P2C3C4              限pwm控制电流
//3，速度            P1P2V3V4              限pwm控制速度
//4，位置            P1P2    PO4PO5PO6PO7  限pwm控制位置
//5，速度位置        P1P2V3V4PO4PO5PO6PO7  限pwm和速度控制位置
//6，电流速度        C1C2V3V4              限制电流控制速度
//7，电流位置        C1C2    PO4PO5PO6PO7  限制电流控制位置
//8，电流速度位置    C1C2V3V4PO4PO5PO6PO7  限制电流速度控制位置
#define set1  20  //参数主动发送周期10ms。
#define set2  0  //开启左右限位发送功能，0，为关闭。
//#define set2  0x01  //开启左右限位发送功能，0，为关闭。
#define drvid0 0x100 //广播地址 第2组驱动器广播地址（组号+地址号+功能号）功能号：0，复位 1，模式选择  2--9 模式号+1  10，配置  11，驱动器数据反馈 12，驱动器限位反馈。 
#define drvid1 0x110 //广播地址 1号驱动器地址
#define drvid2 0x120 //广播地址 2号驱动器地址
#define resetmod 0              //复位模式
#define modmod   0x01           //模式选择
#define setmod   0x0a           //设置模式                  //配置模式不可广播
//#define cvmod 0x07   //电流速度模式
#define pvmod 0x04  //速度模式


typedef struct _CANRMDS_ST{
      s16 cspeed;       //控制速度
      s16 cpwm;     //控制电流（限制电流）
      s16 sspeed;       //反馈速度
      s16 scurrent;     //反馈电流
      s32 position;     //当前位置。（脉冲数）
      s32 posmm;         //当前位置  （mm值 ）
      s8  limit;        //d0:左限位 d1:右限位
      u8  DRVERR;       //驱动器故障 .1秒频率的错误累加。
      s8  MotoFlagForward; //电机转向
}xCANRMDS_D;


extern xCANRMDS_D CANDD; 
extern QueueHandle_t xCANRMDS;  //从中断接收

extern void RMDS_task(void *pv);//启动读卡器命令

#endif