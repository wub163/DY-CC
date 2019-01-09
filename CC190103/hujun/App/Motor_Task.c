/*
******************************************************************************************************
*
*  Copyright (C),   2014-2016, ���������������������޹�˾
*  FileName:        Motor_Task.c
*  Version:         ver1.0
*  Author:          HJ
*  Date:            2018-3-15
*  Description:     
*  Maintainers:     none
*
******************************************************************************************************
*/
#include "main.h" 
#include "stdlib.h"
#include "math.h"

SemaphoreHandle_t sem_RS485_rec_flag = NULL;//RS485������һ�������ź�������
SemaphoreHandle_t sem_RS232_rec_flag = NULL;


u8* RS232DATA=NULL;
u8* RS485DATA=NULL;

//BaseHalSt gHalData;
int TIM5_times_100ms_flag;

u16 	uiMsgLengthTrans,uiMsgSpdTrans,MsgSpd,MsgLifterState;
float uiMsgAngTrans,MsgAng;
char MotorVel[10];
char Position[10];
char Sensor[10];
int16_t FdbMotorVel;
s32 RealPosition;
s32 DisAngle;

u16 LockTime = 0;

void TIM3_Init(int freq)
{ 
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  	//TIM3ʱ��ʹ��   

  /* TIM3 configuration */
  TIM_TimeBaseStructure.TIM_Period = 1000000/freq -1; 
  TIM_TimeBaseStructure.TIM_Prescaler = 84-1;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
  /* Clear TIM3 update pending flag*/
  TIM_ClearFlag(TIM3, TIM_FLAG_Update);
  /* Enable TIM3 Update interrupt*/
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);  

  /* TIM3 enable counter */
  TIM_Cmd(TIM3, ENABLE);
} 

void TIM3_IRQHandler(void)
{
	static u16 TIM3_counts = 0;
	if (TIM_GetITStatus(TIM3,TIM_IT_Update) != RESET)
	{
		if(cha_state.TurnctrlRespCnt ++>200)cha_state.TurnctrlRespCnt=200;
		if(cha_state.TurnfreqRespCnt ++>200)cha_state.TurnfreqRespCnt=200;		
		if(   cha_state.FlashRespCnt ++>200)cha_state.FlashRespCnt=200;
		
		
		TIM3_counts ++;
		if(TIM3_counts >= 10)
		{
			TIM3_counts = 0;
			cha_state.timeshare = (~cha_state.timeshare)&0x01;
		}
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
}

void TIM5_Init(int freq)
{ 
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);  	//TIM5ʱ��ʹ��   
  //TIM_DeInit( TIM2);

  /* TIM5 configuration */
  TIM_TimeBaseStructure.TIM_Period = 1000000/freq -1; 
  TIM_TimeBaseStructure.TIM_Prescaler = 84-1;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
  TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

	TIM_ClearITPendingBit(TIM5,TIM_IT_Update);
  /* Clear TIM5 update pending flag*/
  TIM_ClearFlag(TIM5, TIM_FLAG_Update);
  /* Enable TIM5 Update interrupt*/
  TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);  

  /* TIM5 enable counter */
  TIM_Cmd(TIM5, ENABLE);
} 
/***************************************************************************************************/
//��������:TIM5�жϽ���ͳ��
//
/***************************************************************************************************/

void TIM5_IRQHandler(void)
{
//	OSIntEnter(); 
	if (TIM_GetITStatus(TIM5,TIM_IT_Update) != RESET)
	{
		TIM5_times_100ms_flag = 1;		
	}
	TIM_ClearITPendingBit(TIM5,TIM_IT_Update);
//	OSIntExit(); 
}


/***************************************************************************************************/
//��������:�����ʼ��
//
/***************************************************************************************************/
void Motor_Init(void)
{
	TIM3_Init(1000);        //��ɫ��ָʾ
	TIM5_Init(10);     //100ms ��ʱδ��
}




/******************************************************************
	* Function:          int Report11Info(void)
	* Description:       �ϱ�С���ڵ�ID��λ����Ϣ,��ת�Ƕ���Ϣ
	* Input parameters:  void
	*                    
	* Return parameters: 0�����³ɹ���1���������ݲ�����Ҫ��
********************************************************************/
void Report11Info(void)
{
  sentst sentdata={0,};
 
sentdata.head=0x55;
sentdata.lenth=sizeof(sentst);
sentdata.end=0xaa;

sentdata.CCSTAT=cha_state.CAR_STATE;
sentdata.dcdl=CANPE.dqdl;
sentdata.dcdy=CANPE.dqdy;
sentdata.dcwd=CANPE.pjwd;
sentdata.LEDST=cha_state.Led_State;
sentdata.LifterState=cha_state.LifterState;
sentdata.FdbMotorVel=cha_state.wheelst.FdbMotorVel;  
sentdata.RealPosition=(s16)cha_state.turn_moto_getpro.RealPosition;
sentdata.speedPosition=CANDD.position;
  
u8 crc=0;
for(int i = 0; i < (sentdata.lenth-4); ++i)
{
  crc+=*(u8*)(&(sentdata.head)+i+2);
}
sentdata.crc=crc;
	RS232_Send_Data(&sentdata.head,sentdata.lenth);
//        LEDY=!LEDY;  //2019-01-03 �����á�

}



/***********************************************************************
�������ƣ�void Task_USART(void *pdata)
��    �ܣ�
���������
���������
��дʱ�䣺
�� д �ˣ�
ע    �⣺
***********************************************************************/
void MotorCtrl_init(void)
{

		cha_state.turn_moto_get.ADR = 0x01;
		cha_state.turn_moto_get.CMD = 0x03;
		cha_state.turn_moto_get.StartRegister = 0x0022;
		cha_state.turn_moto_get.RegisterCount = 0x0006;
	
		cha_state.wheelst.CmdMotorvel = 0;
		cha_state.wheelst.CmdMotorAngle = 0;
		cha_state.wheelst.CmdWheelAngle = 0;
		cha_state.wheelst.CmdWheelVel = 0;
		cha_state.wheelst.CmdMsgAngle = 0;
		cha_state.wheelst.CmdMsgVel = 0;
}
/***********************************************************************
�������ƣ�void Task_USART(void *pdata)
��    �ܣ�
���������
���������
��дʱ�䣺
�� д �ˣ�
ע    �⣺
***********************************************************************/
void Task_USART(void *pdata)
{		 
RS485DATA=pvPortMalloc(RS485_REC_BUFF_SIZE);

  RS_485_RX_EN;//485����ģʽ
	sem_RS485_rec_flag=xSemaphoreCreateBinary(); //����һ���ź���,
	while(1)
	{  
		xSemaphoreTake(sem_RS485_rec_flag, portMAX_DELAY);			//�����ȴ�sem_RS485_rec_flag�ź�����Ч
		{
			GetResp(RS485DATA,RS485_rec_counter);//��������������
			RS485_REC_Flag = 0;						//�����꽫���ռ���������
			
		}

	}
	//OSTimeDlyHMSM(0, 0, 0, 10);
}
/***********************************************************************
�������ƣ�void Task_RS485(void *pdata)
��    �ܣ�
���������
���������
��дʱ�䣺
�� д �ˣ�
ע    �⣺
***********************************************************************/
void Task_RS232(void *pdata)
{
  recvst* recvdata;
                recvdata=(recvst*)&(RS232DATA[0]);        

//  unsigned  char  os_err;
  u8 commancount=0;
  u8 comautocount=0;//ͨѶ������
	u8 i;
        
RS232DATA=pvPortMalloc(RS232_REC_BUFF_SIZE);

	sem_RS232_rec_flag=xSemaphoreCreateBinary(); //����һ���ź���,
	while(1)
	{  
          if(xSemaphoreTake(sem_RS232_rec_flag, 500/portTICK_PERIOD_MS)==errQUEUE_EMPTY){commancount=200;comautocount=200;}; 			//�����ȴ�sem_RS232_rec_flag�ź�����Ч
		{
			i =(u8)RS232DATA[0];
/***********************************wifi-app****************************************************/			
			if((i == 0x55)&&((u8)RS232DATA[9] == 0xAA))
			{
					if((u8)RS232DATA[3] == 11)
					{
							cha_state.timecount = 0;LEDB=1;LEDR=!LEDR;//���ص�ָʾ
							if(((u8)RS232DATA[6] != 0x64)&&((u8)RS232DATA[6] != 0x9C))
							{commancount=0;
							cha_state.wheelst.CmdWheelAngle = (int16_t)((int8_t)RS232DATA[5]&0xFFFF);
							cha_state.wheelst.CmdWheelVel = (int16_t)((int8_t)RS232DATA[6]&0xFFFF);
                                                        cha_state.LifterCtrl=((u8)RS232DATA[4]>>manupdn)&0x01;
                                                        cha_state.manctrl=(u8)RS232DATA[4];
                                                        
							}
					}
			}
/*************************************PC�·��ٶȿ���*******************************************************/		
	if((i == 0x55)&&((u8)RS232DATA[1] == sizeof(recvst)))//���ף������жϡ�
          {u8 crc=0;
        for(int i = 0; i < RS232DATA[1]-4; ++i)   crc+=RS232DATA[i+2];
                        recvdata=(recvst*)&(RS232DATA[0]);        
        if((recvdata->end == 0xaa)&&(recvdata->crc == crc))
		{comautocount=0;LEDB=!LEDB;LEDR=1;//���ص�ָʾ
				cha_state.timecount = 0;

  
                                cha_state.wheelst.CmdMsgVel =  recvdata->CmdMsgVel  ;
				cha_state.wheelst.CmdMsgAngle = recvdata->CmdMsgAngle;
                                cha_state.LifterCtrl = recvdata->LifterCtrl;
                                cha_state.Led_Control = recvdata->Led_Control;
                                cha_state.chargeswitch=recvdata->chargeswitch;
                                cha_state.mp3mute=recvdata->mp3mute;
						 }


			}
			memset(RS232DATA,0,30);RS232_REC_Flag = 0;
		}
		if( cha_state.Battery_Current>-10)//�ǳ���У������״ָ̬ʾ�����ʱ������ָʾ��
                {if(!(AUTO1&&AUTO2)){ if(cha_state.Led_Control==0){
                    if(comautocount++>10){WGEE=0;WRED=1;comautocount=200;LEDB=1;}else {WGEE=1;WRED=0;}WYEL=0;LEDR=1;}}
                else {if(commancount++>10){WYEL=0;WRED=1;commancount=200;LEDR=1;}else {WYEL=1;WRED=0;}WGEE=0;LEDB=1;}}
	}
}


/***********************************************************************
�������ƣ�void Task_MOTOR(void *pdata)
��    �ܣ����ߵ����������
���������
���������
��дʱ�䣺
�� д �ˣ�
ע    �⣺
***********************************************************************/
void Task_MOTOR(void *pdata)
{		 
 TickType_t xLastWakeTime;
  s16 errcount=0;//����ͳ��
  u8 errlock=0;
  vTaskDelay(2000/portTICK_PERIOD_MS);
	MotorCtrl_init();
	 xLastWakeTime = xTaskGetTickCount ();
	
  while(1)
  {

    
			

                        if(!(AUTO1&AUTO2)){
                                        if(errcount>200)errlock=1;
					if(errlock) cha_state.wheelst.CmdMotorvel =0;//3��ת��û��λ��ֱ���ٶ����α�����
                                        else  cha_state.wheelst.CmdMotorvel = (int16_t)cha_state.wheelst.CmdMsgVel;
                                          
                                        if(abs((s32)cha_state.wheelst.CmdMotorAngle - cha_state.turn_moto_getpro.RealPosition)>150)
                                          cha_state.wheelst.CmdMotorvel>>=1;//�Ƕ�ƫ��󣬽�������
					cha_state.wheelst.CmdMotorAngle = (int32_t)(cha_state.wheelst.CmdMsgAngle * CMD_MSG_ANGLE);

			}
			else{
					cha_state.wheelst.CmdMotorvel = (int16_t)(cha_state.wheelst.CmdWheelVel * 5);
					cha_state.wheelst.CmdMotorAngle = (int32_t)(cha_state.wheelst.CmdWheelAngle * 4);
			}
  		 if((cha_state.timecount >= 50)||(cha_state.Battery_Current<-10)||(cha_state.zero_Flag == 0)||STOP)//ͨѶ��ʱ����,����б�����
							{
					//cha_state.MP3_Control = 0;
					cha_state.wheelst.CmdMotorvel = 0;
					cha_state.wheelst.CmdMotorAngle = 0;
					cha_state.wheelst.FdbMotorVel = 0;
					cha_state.wheelst.CmdWheelAngle = 0;
					cha_state.wheelst.CmdWheelVel = 0;
                                        errlock=0;//����ͣ���������
			}
  
					Report11Info();
  
  
  
			if(cha_state.stateid==1)//0:��ʼ����λ 2����ͣ  1��������
			{
					if(cha_state.timecount++>200)cha_state.timecount=200;
//					cha_state.cancount++;
					cha_state.cancount=0;
					
	////				MB_SetReg(&cha_state);//���Ͷ��Ĵ�������
                            MB_Get03();vTaskDelay(5/portTICK_PERIOD_MS);
					DisAngle = abs((s32)cha_state.wheelst.CmdMotorAngle - cha_state.turn_moto_getpro.RealPosition);
					if(DisAngle > miniturn)
					{
							MovePositionSet(cha_state.wheelst.CmdMotorAngle);//����ת���Ƕȡ�
                                                        if(DisAngle>50) {if(errcount++>60)errcount=250;}//3����û�ӽ�Ŀ���5�����ڣ����ж��쳣��
					}
					else
					{
							MovePositionSet(cha_state.turn_moto_getpro.RealPosition);//�Ƕȹ�С������ת��λ��
                                                        errcount=0;
					}

			}
			vTaskDelayUntil(&xLastWakeTime,50/portTICK_PERIOD_MS);

  }
}