/*========================================================= 
*    文件名:  	        CAN_drive.h 
*    描述: 	        兴颂CAN电机驱动程序
*    版本号:  	        1.1   
*    创建日期:          2015-6-1 8:52:03  
*    IDE环境:  	        IAR Embedded Workbench for ARM version 7.30.3
*    字符编码:  	GB2312   
*    作	者:  	        吴兵  
*    版权所有:  	哈尔滨工大服务机器人有限公司
*    版本修改历史:    （XXX:为操作者）
*       2015-6-1        吴兵:创建文件   
*  	2016-3-20       吴兵:整理完善   
* ===========================================================  */     
#ifndef __CANDRV_H
#define __CANDRV_H


#include "stm32f4xx.h"
#include "stdlib.h"
typedef struct CanDriveStruct
{
        float CmdVel[2];                //控制速度。
        s32 moto[2];                    //控制用脉冲数
        int FbbPos[2];                  //脉冲数
        double  FbPos[2];               //mm数
        double  length;                 //设备里程
        s32      rad;
        double LastFbPos[2];
        s16 dy;
        float dl;
        u8  FatalErr;                   //通讯代码故障
        u8 CANERR;                      //无通讯故障。
        u8 jczt;                //检测状态 1；电流，电压  2：电机位置,0:不反馈数据。
        s8 FlagForward[2];
        
}CanDriveSt;
extern CanDriveSt candr;
//void moto_can (void);
//void CalMotor_can(void);
void Candr_init(void);
void moto_turn(s16 forward,s16 back);
u8 CAN1_Mode_Init(u16 BAUD);
#endif


