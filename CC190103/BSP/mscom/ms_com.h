#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "stm32f4xx.h"
extern u8 rxtx;

#define DMARX_SIZE    sizeof(RECV_D)          //ͨѶ����С��
#define DMATX_SIZE    sizeof(SEND_D)          //ͨѶ����С��

void mscomtask (void* pv);
extern   u8* DMATX_BUF,  * DMARX_BUF;
