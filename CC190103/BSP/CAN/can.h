#ifndef __CAN_H
#define __CAN_H	 
#include "sys.h"	    
#include "stm32f4xx.h"
#include  "../jiami/jm.h"

	
//CAN1����RX0�ж�ʹ��
#define CAN1_RX0_INT_ENABLE	1		//0,��ʹ��;1,ʹ��.								    
										 							 				    
u8 CAN1_Mode_Init(u16 BAUD);//CAN��ʼ��
 
u8 CAN1_Send_Msg(u16 id,u8* msg,u8 len);	//��������:id ��ַ��msg���ݰ���len���ݳ���

u8 CAN1_Receive_Msg(u8 *buf);			//��������


u8 CAN2_Mode_Init(u16 BAUD);//CAN��ʼ��
 
u8 CAN2_Send_Msg(u16 id,u8* msg,u8 len);	//��������:id ��ַ��msg���ݰ���len���ݳ���

u8 CAN2_Receive_Msg(u8 *buf);			//��������
#endif






/*can ��ַ�ֲ���
������             0x041
������������       0x042  0x043
������0,1:         0x200
  20X �������㲥 21X 1#������  22X 2#������ 
          x��0-15 ��ͬ���ܺ�
���ϲ�����Ϊ1000k

��ز�����Ϊ500k



*/










