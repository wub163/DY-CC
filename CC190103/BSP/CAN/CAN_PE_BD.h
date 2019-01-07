/*******************************************88888

������
************************************************/
#include "sys.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "queue.h"
#include "can.h"

#define CANPEID 0x16
typedef struct _CANPE_ST{
      u16 DY[17];         //��ѹ0.1v  0���ܵ�ѹ 1-16 �����ѹ��
      s16 DL;             //����0.1a
      u8 cap;             //��ص���0-100
      u8 temp;             //����¶�0��Ӧ-40��
      u8 stat;             //���״̬1������� 0��δ���
      u8 num;             //��ش���
      u8 DRVERR;
}xCANPE_D;

extern QueueHandle_t xCANPE;  //���жϽ���

extern void CANPE_task(void *pv);//��������������

extern   xCANPE_D CANPE;
   