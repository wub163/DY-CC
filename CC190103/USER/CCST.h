#include "sys.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "queue.h"
#include "can.h"

//#define highbase  34

extern QueueHandle_t xCCST;  //���жϽ���

extern void CCST_task(void *pv);//��������������

void highset(u16 high);
void CCST_task(void *pv);
void moto_task(void *pv);