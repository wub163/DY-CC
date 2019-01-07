/*******************************************88888

�οƵµ��
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
      u8 txok;         //��ǰ��ͨѶ������
      u8 state;        //��ǰ״̬
      u16 sjlc;             //��������1��*10   0��*1
      u16 dqdy;             //��ǰ��ѹmv
      s16 dqdl;             //��ǰ����ma
      u16 csrl;             //��ʼ����
      u16 dqrl;             //��ǰ����
      u16 syrl;             //ʣ������
      u8  sybfb;           //ʣ������ٷֱ�
      u16 cdxh;         //���ѭ��
      u16 day;          //ʹ������
      u16 minutes;      //ʹ�÷���
      u8  sccd;         //�ϴγ�����
      s8  pjwd;         //ƽ���¶ȡ�
      u8  POWER_err;      //ͨѶ�����ۼơ�
}xCANPE_D;
#pragma   pack()
extern QueueHandle_t xCANPE;  //���жϽ���

extern void CANPE_task(void *pv);//��������������

extern   xCANPE_D CANPE;
   
#endif