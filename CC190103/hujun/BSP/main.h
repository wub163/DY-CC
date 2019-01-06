/***********************************************************************
文件名称：
功    能：
编写时间：
编 写 人：
注    意：
***********************************************************************/
# include "set.h"
#ifndef _MAIN_H_
#define _MAIN_H_


//添加必要的头文件

//#include "M_Type_P.h"
//#include "M_CMSIS_P.h" 

#include "sys.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "queue.h"
#include "gpio.h" 
#include "../jiami/jm.h"
#include "cpuinfo.h"
#include "CAN_PE_XKD.h"
#include "CAN_RMDS.h"




#include "stm32f4xx.h"
#include "Motor_Task.h"
#include "Main_Task.h"
#include "SCI.h"
#include "DMA.h"
#include "NVIC.h"
#include "can.h"
#include "stdio.h"
#include "Modbus.h"
#include "MOTOR.h"
#include "Bsp_Global.h"
#include "stdlib.h"
#include "..\TM1638\TM1638.h"
//#include "AcceInet.h"
//#include "mr15.h"



#endif
