#include "sys.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "queue.h"
#include "can.h"
#define broadcast  1
typedef struct _CANID_ST{
      u8 ID[8];         //id��ȡ����
      u8 WID[8];        //idд����
      u8 IDRSSI;          //ID���ź�ǿ�� ����״̬+���ź�ǿ�ȣ�0-8��
      u8 IDWRT;          //idд״̬��   д��ָ��ķ���״̬��0���ɹ���1��д�����������2���޿�����
      u8 IDCOT;          //ID��ָ�   ��ǰ��ָ��        ��0������״̬��1��д��״̬����
      u8 IDERR;         //ID�����������ź�ʱ��.1��Ƶ�ʵĴ����ۼӡ�
      u8 IDCL;          //1,�Ѵ������롣
}xCANID_D;

extern  xCANID_D CANID;
extern QueueHandle_t xCANID;  //���жϽ���

extern void ID_task(void *pv);//��������������

