#include "mr15.h"
#include "main.h"
#include "can.h"

 /**
  * @brief  初始化控制MR15_EN的IO
  * @param  无
  * @retval 无
  */
void MR15_EN_GPIO_Config(void)
{
		/*定义一个GPIO_InitTypeDef类型的结构体*/
		GPIO_InitTypeDef GPIO_InitStructure;
	
		/*开启LED相关的GPIO外设时钟*/
		RCC_AHB1PeriphClockCmd ( MR15_EN_GPIO_CLK, ENABLE); 
	
		/*设置引脚模式为输出模式*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
    
    /*设置引脚的输出类型为推挽输出*/
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    
    /*设置引脚为上拉模式*/
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

		/*设置引脚速率为50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

		/*选择要控制的GPIO引脚*/
		GPIO_InitStructure.GPIO_Pin = MR15_EN_PIN;
    GPIO_Init(MR15_EN_GPIO_PORT, &GPIO_InitStructure);	

		MR15_EN(MR15_DISENABLE);
}
void MR15AceCtrl(int16_t speed)
{
	uint8_t transmit_mailbox = 0;
	CanTxMsg TxMessage;
	static uint8_t stuffing=0;
	uint8_t mode,acc,dec,brake;
	acc = 50;
	dec = 50;
	brake = 0;
	
	stuffing++;
	stuffing%=2;
	if((cha_state.LifterInput&(1<<0))==(1<<0))
	{
			MR15_EN(MR15_ENABLE);
			mode=0x43;
			if(speed>0)
				mode|=(0x01<<3);
			else
			{
				mode|=(0x02<<3);
				speed=-speed;
			}
	}
	else
	{
			MR15_EN(MR15_DISENABLE);
			mode=0x00;
	}
	
	TxMessage.IDE=CAN_ID_STD;					 //标准模式
	TxMessage.RTR=CAN_RTR_DATA;				 //发送的是数据
	TxMessage.DLC=8;							 //数据长度为8字节
	TxMessage.StdId = 0x208;

	if(stuffing)
		mode|=0x80;
	TxMessage.Data[0] = mode;
	TxMessage.Data[1] = (unsigned char)(speed & 0x00ff);
	TxMessage.Data[2] = (unsigned char)(speed >> 8);
	TxMessage.Data[3] = acc;
	TxMessage.Data[4] = dec;
	TxMessage.Data[5] = 0x00;
	TxMessage.Data[6] = brake;
	TxMessage.Data[7] = 0x00;
	
	transmit_mailbox = CAN_Transmit(CAN1,&TxMessage);

}
void MR15EpsCtrl(int16_t angle)
{
	uint8_t transmit_mailbox = 0;
	CanTxMsg TxMessage;
	static uint8_t stuffing=0;
	uint8_t mode;
	uint16_t speed = 50;
	
	stuffing++;
	stuffing%=2;
	if((cha_state.LifterInput&(1<<0))==(1<<0))
	{
		mode=0x43;
		MR15_EN(MR15_ENABLE);
	}
	else
	{
		MR15_EN(MR15_DISENABLE);
		mode=0x00;
	}
	TxMessage.IDE=CAN_ID_STD;					 //标准模式
	TxMessage.RTR=CAN_RTR_DATA;				 //发送的是数据
	TxMessage.DLC=8;							 //数据长度为8字节
	TxMessage.StdId = 0x218;
	if(stuffing)
		mode|=0x80;
	TxMessage.Data[0] = mode;
	TxMessage.Data[1] = (unsigned char)(speed & 0x00ff);
	TxMessage.Data[2] = (unsigned char)(speed >> 8);
	TxMessage.Data[3] = (unsigned char)(angle & 0x00ff);
	TxMessage.Data[4] = (unsigned char)(angle >> 8);
	TxMessage.Data[5] = 0x00;
	TxMessage.Data[6] = 0x00;
	TxMessage.Data[7] = 0x00;

	transmit_mailbox = CAN_Transmit(CAN1,&TxMessage);
}
void MR15AceIoCtrl(uint8_t actiontype,int16_t lift,uint8_t pwm)
{
	uint8_t transmit_mailbox = 0;
	CanTxMsg TxMessage;
	static uint8_t stuffing=0;
	uint8_t mode;
	uint8_t evp1,evp2;
	
	stuffing++;
	stuffing%=2;

	if((cha_state.LifterInput&(1<<0))==(1<<0))
	{
		if(actiontype>0)
		{
			actiontype--;
			mode=1<<actiontype;
		}
		else
			mode=0x00;
		
		if(lift<0)
		{
			evp1=(uint8_t)-lift;
			evp2=0;
		}
		else if(lift==0)
		{
			evp1=0;
			evp2=0;
		}
		else if(lift>0)
		{
			evp1=0;
			evp2=(uint8_t)lift;
		}
		MR15_EN(MR15_ENABLE);
	}
	else
	{
		MR15_EN(MR15_DISENABLE);
		mode=0x00;
		evp1=0;
		evp2=0;
	}
	TxMessage.IDE=CAN_ID_STD;					 //标准模式
	TxMessage.RTR=CAN_RTR_DATA;				 //发送的是数据
	TxMessage.DLC=8;							 //数据长度为8字节
	TxMessage.StdId = 0x508;
	if(stuffing)
		mode|=0x80;
	TxMessage.Data[0] = mode;
	TxMessage.Data[1] = evp1;
	TxMessage.Data[2] = evp2;
	TxMessage.Data[3] = 0x00;
	TxMessage.Data[4] = 0x00;
	TxMessage.Data[5] = 20;  //液压泵电机加速度值
	TxMessage.Data[6] = 20;	 //液压泵电机减速度值
	TxMessage.Data[7] = pwm;

	transmit_mailbox = CAN_Transmit(CAN1,&TxMessage);
}