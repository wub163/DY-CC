
//#ifndef __bianmaqi_H
//#define __bianmaqi_H	 
#include "sys.h"

typedef struct bianmast
{
	int front;
	int behind;
        int high;
}bianmaqi;

extern QueueHandle_t xBIANMA;
void SensorTask(void * pv);
//#endif