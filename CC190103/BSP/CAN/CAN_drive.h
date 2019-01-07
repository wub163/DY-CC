/*========================================================= 
*    �ļ���:  	        CAN_drive.h 
*    ����: 	        ����CAN�����������
*    �汾��:  	        1.1   
*    ��������:          2015-6-1 8:52:03  
*    IDE����:  	        IAR Embedded Workbench for ARM version 7.30.3
*    �ַ�����:  	GB2312   
*    ��	��:  	        ���  
*    ��Ȩ����:  	���������������������޹�˾
*    �汾�޸���ʷ:    ��XXX:Ϊ�����ߣ�
*       2015-6-1        ���:�����ļ�   
*  	2016-3-20       ���:��������   
* ===========================================================  */     
#ifndef __CANDRV_H
#define __CANDRV_H


#include "stm32f4xx.h"
#include "stdlib.h"
typedef struct CanDriveStruct
{
        float CmdVel[2];                //�����ٶȡ�
        s32 moto[2];                    //������������
        int FbbPos[2];                  //������
        double  FbPos[2];               //mm��
        double  length;                 //�豸���
        s32      rad;
        double LastFbPos[2];
        s16 dy;
        float dl;
        u8  FatalErr;                   //ͨѶ�������
        u8 CANERR;                      //��ͨѶ���ϡ�
        u8 jczt;                //���״̬ 1����������ѹ  2�����λ��,0:���������ݡ�
        s8 FlagForward[2];
        
}CanDriveSt;
extern CanDriveSt candr;
//void moto_can (void);
//void CalMotor_can(void);
void Candr_init(void);
void moto_turn(s16 forward,s16 back);
u8 CAN1_Mode_Init(u16 BAUD);
#endif


