/****************************************************************************
* Copyright (C), 2013
***************************************************************************/
//#ifndef __COMDRV_H
//#define __COMDRV_H

#include "FreeRTOS.h"
#include "queue.h"
#include "stm32f4xx.h"
#include "stdlib.h"

#define se485 PDout(11)=1
#define re485 PDout(11)=0
#define GearScale 1     //������
#define WheelPeri 125   //��ֱ��
#define MotorEncoder 900 //��������
#define MotorTurn    3000*(5)   // ���ת���ٶȡ���1-10��       
#define MOTORNUM485 2           //���������
#define MotoFlagForward0  -1
#define MotoFlagForward1   1    //�����ֵ��ת��


void MOTOtask(void* pv);

extern  QueueHandle_t  xMOVE;



//#endif






