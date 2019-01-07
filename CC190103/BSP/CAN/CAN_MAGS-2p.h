#include "sys.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "queue.h"
#include "can.h"

typedef struct _CANMAGS_ST{
      u16 mags[2];              //����λͼ���������ҵ�λ���С�1����⵽��
      u8  OnMag[2];             //������λ��
      s8  magdata[2];        //����λ������
      s8  zyfx;              //����ת��
      u8  DRVERR[2];       //���������� .1��Ƶ�ʵĴ����ۼӡ�
}xCANMAGS_D;

extern QueueHandle_t xCANMAGS;  //���жϽ���
extern xCANMAGS_D CAR_MAGS;
extern void MAGS_task(void *pv);//��������������

