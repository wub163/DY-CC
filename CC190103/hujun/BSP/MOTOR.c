/***********************************************************************
�ļ����ƣ�MOTOR.C
��    �ܣ�
��дʱ�䣺2018.3.25
�� д �ˣ�HJ
ע    �⣺
***********************************************************************/
#include "main.h"

static u8 MotorA_zero_Flag  = 0; //��⵽���A����־λ
//static u8 MotorAstopFlag  = 0;  //ת��A���ֹͣ��־λ
static volatile int16_t Diversion1_value = 0;//���1��ǰ�Ƕ�ֵ
static volatile int32_t ZeroDiversionA_value = 0;//���1���Ƕ�ֵ
static int8_t Approach1DiversionFlag = 0;//�ӽ�����1��ʼ״̬

//static u8 ScramFlag = 0;     //��ͣ��־�����¼�ͣ��λ
//static u8 StartFlag = 0;     //������־������������λ

/*-------------------------------------------------------
��������MotorA_Zero_SetFlag
��  �ܣ����õ��A����־
        �������λ
��  ������
����ֵ����
--------------------------------------------------------*/
void MotorA_Zero_SetFlag(void)
{
  MotorA_zero_Flag = 1;
}
/*-------------------------------------------------------
��������MotorA_Zero_SetFlag
��  �ܣ�������A����־
��  ������
����ֵ����
--------------------------------------------------------*/
void MotorA_Zero_CleanFlag(void)
{
  MotorA_zero_Flag = 0;
}
/*-------------------------------------------------------
��������MotorA_Zero_ISFlag
��  �ܣ��жϱ�־λ״̬      
��  ������
����ֵ����
--------------------------------------------------------*/
u8 MotorA_Zero_ISFlag(void)
{
 return MotorA_zero_Flag;
}
/********************************************************************************************************
**������Ϣ :uint8_t TurningStop(void)
**�������� :ֹͣת��
**������� :
**������� :0�ɹ� or ������
*********************************************************************************************************/
uint8_t TurningStop(void)
{
	uint8_t uReturnValue;//����ֵ
	uReturnValue = 1;//��ʼֵΪ��
	vTaskDelay(10/portTICK_PERIOD_MS);

	//Move driving wheel
	uReturnValue = MB_WriteSingleReg(REG_STOP_INSPM, 0);

	if(uReturnValue == MB_ENOERR)
	{
	}
	else if(uReturnValue == MB_ETIMEDOUT)
	{
		return ERR_010;
	}
	else if(uReturnValue == MB_EILLSTATE)
	{
		return ERR_012;
	}
	else if(uReturnValue == MB_CRCERR)
	{
		return ERR_014;
	}
	else
	{
		return ERR_060;
	}
	return ERR_000;
}
/********************************************************************************************************
**������Ϣ :uint8_t SeekZeroTurning(int16_t TurnSpd)
**�������� :Ѱ��λ
**������� :TurnSpd ת���ٶ�
**������� :0�ɹ� or ������
*********************************************************************************************************/
uint8_t SeekZeroTurning(int16_t TurnSpd)
{
	uint8_t uReturnValue;//����ֵ
	uReturnValue = 1;//��ʼֵΪ��
	vTaskDelay(10/portTICK_PERIOD_MS);

	//Move driving wheel
	uReturnValue = MB_WriteSingleReg(REG_TARGET_SPEED_INSPM, TurnSpd);

	if(uReturnValue == MB_ENOERR)
	{
	}
	else if(uReturnValue == MB_ETIMEDOUT)
	{
		return ERR_010;
	}
	else if(uReturnValue == MB_EILLSTATE)
	{
		return ERR_012;
	}
	else if(uReturnValue == MB_CRCERR)
	{
		return ERR_014;
	}
	else
	{
		return ERR_060;
	}
	return ERR_000;
}



int Seek_zero(void)
{
  static u8 errno;
uint16_t i,flag,seekflg;
  /*------------��ȡ��ǰ������λ�ã�����������-------------*/
	scan_data.zero = GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_3);
  if(scan_data.zero == 0)
  {
    Approach1DiversionFlag = 1;
  }
  else
	{
    Approach1DiversionFlag = -1;
  }
/*--------------1����Ѱ���λ----------------------*/  
while(MotorA_Zero_ISFlag()== 0)
 {

 
   scan_data.zero = GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_3);
if(Approach1DiversionFlag== 1)
     {
        if(scan_data.zero == 0)
        {  
						 SeekZeroTurning(SEEK_ZERO_CLOCK_FAST);
						 flag = 1;
        }
        else
        { 
						 TurningStop();//ֹͣ
						 for(i=0;i<ZERO_APPROACH1_LOFFSET;i++)
						 {
							errno= SeekZeroTurning(SEEK_ZERO_UCLOCK_SLOW);//��ת
							 scan_data.zero = GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_3);
							 if(flag == 1 && scan_data.zero == 0)
							{
                                                          if(errno==ERR_000){
									TurningStop();//ֹͣ
////									MB_SetReg(&cha_state);
                                                                        MB_Get03();
									MotorA_Zero_SetFlag();
									seekflg = 1;
									break;}
                                                          else {Approach1DiversionFlag=-1;}
							 }
						 }
        }
     }
     if(Approach1DiversionFlag==-1)
     {       
        if(scan_data.zero == 1)
        {
						SeekZeroTurning(SEEK_ZERO_UCLOCK_FAST);
        }
        else
        {          
      vTaskDelay(1000/portTICK_PERIOD_MS);
      Approach1DiversionFlag=1;

        }
     }

 }
 return seekflg;
}
/********************************************************************************************************
**������Ϣ :uint8_t ReadRealTimeFdbSpd(int16_t LeftWheelFdbSpd,int16_t RightWheelFdbSpd)
**�������� :�������
**������� :
**������� :
*********************************************************************************************************/
//uint8_t ReadRealTimeFdbSwitchFreq(void)
//{
//	uint8_t uReturnValue;//����ֵ
//	
//	uReturnValue =1;//��ʼֵΪ��
//	
////	M_Delay_us(60);
//	//Move left wheel
//  
////	uReturnValue = MB_ReadReg(WHEEL_DRIVE_ADR, REG_REALTIME_STATUS_SPD, 6);//
//
//	if(uReturnValue == MB_ENOERR)
//	{
//	}
//	else if(uReturnValue == MB_ETIMEDOUT)
//	{
//		return ERR_010;
//	}
//	else if(uReturnValue == MB_EILLSTATE)
//	{
//		return ERR_012;
//	}
//	else if(uReturnValue == MB_CRCERR)
//	{
//		return ERR_014;
//	}
//	else
//	{
//		return ERR_060;
//	}
//	
//	return ERR_000;
//
//}

/********************************************************************************************************
**������Ϣ :uint8_t MovePositionSet(int32_t position)
**�������� :����������ת��λ��
**������� :position  ת������λ��
**������� :0�ɹ� or ������
*********************************************************************************************************/
uint8_t MovePositionSet(int32_t position)//���ת����ָ��λ��
{
	uint8_t uReturnValue;//����ֵ
	int16_t ubuffH,ubuffL;
	int32_t realposition;
	uReturnValue = 1;//��ʼֵΪ��
	//Move driving wheel
	realposition = position + cha_state.zero_Value;  //ʵ��ƫתֵ=ƫתλ��+������λ���ֵ
	ubuffH = (int16_t)(realposition >> 16);    //λ�õĸ��ֽ�
	ubuffL = (int16_t)realposition;    //λ�õĵ��ֽ�
	uReturnValue = MB_WritePluralReg(WHEEL_DRIVE_ADR, REG_TARGET_SPEED_INPOM, 0x0004,0x08,0x1388,0x0000,ubuffH,ubuffL);//��0x1388���ٶ�ת��ָ��λ�á�

	if(uReturnValue == MB_ENOERR)
	{
	}
	else if(uReturnValue == MB_ETIMEDOUT)
	{
		return ERR_010;
	}
	else if(uReturnValue == MB_EILLSTATE)
	{
		return ERR_012;
	}
	else if(uReturnValue == MB_CRCERR)
	{
		return ERR_014;
	}
	else
	{
		return ERR_060;
	}
	return ERR_000;
}



//uint8_t test_MotorCommunication(uint8_t uADR, uint16_t uTestCount)
//{
//	uint8_t uFuncReturnValue;//���õĺ����ķ���ֵ
//	uint16_t i;//ѭ������
//
//	for(i = 0; i < uTestCount; i++)
//	{
////		M_Delay_us(500);
//		uFuncReturnValue= MB_WriteSingleReg(REG_STOP_INSPM, MOTOR_FREE_STOP);
//		if(uFuncReturnValue != MB_ENOERR)
//		{
//			break;
//		}
//	}
//	return	uFuncReturnValue;
//}
