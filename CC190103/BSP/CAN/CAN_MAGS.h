#include "sys.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "queue.h"
#include "can.h"

typedef struct _CANMAGS_ST{
      u16 mags;                 //����λͼ���������ҵ�λ���С�1����⵽��
      u16  lasttime;            //�������ݱ仯ʱ�䡣
      u8  OnMag;                //������λ��
      s8  magdata;              //����λ������
      s8  lastmagdata;          //��ʷ����λ������
      s8  zyfx;                 //����ת��
      u8  DRVERR;               //���������� .1��Ƶ�ʵĴ����ۼӡ�
}xCANMAGS_D;

extern QueueHandle_t xCANMAGS;  //���жϽ���
extern xCANMAGS_D CAR_MAGS;
extern void MAGS_task(void *pv);//��������������

