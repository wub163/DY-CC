#ifndef __CAN_H
#define __CAN_H	 
#include "sys.h"	    
#include "stm32f4xx.h"
#include  "../jiami/jm.h"

	
//CAN1接收RX0中断使能
#define CAN1_RX0_INT_ENABLE	1		//0,不使能;1,使能.								    
										 							 				    
u8 CAN1_Mode_Init(u16 BAUD);//CAN初始化
 
u8 CAN1_Send_Msg(u16 id,u8* msg,u8 len);	//发送数据:id 地址，msg数据包，len数据长度

u8 CAN1_Receive_Msg(u8 *buf);			//接收数据


u8 CAN2_Mode_Init(u16 BAUD);//CAN初始化
 
u8 CAN2_Send_Msg(u16 id,u8* msg,u8 len);	//发送数据:id 地址，msg数据包，len数据长度

u8 CAN2_Receive_Msg(u8 *buf);			//接收数据
#endif






/*can 地址分布：
读卡器             0x041
磁条传感器：       0x042  0x043
驱动器0,1:         0x200
  20X 驱动器广播 21X 1#驱动器  22X 2#驱动器 
          x：0-15 不同功能号
以上波特率为1000k

电池波特率为500k



*/










