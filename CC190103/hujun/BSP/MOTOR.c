/***********************************************************************
文件名称：MOTOR.C
功    能：
编写时间：2018.3.25
编 写 人：HJ
注    意：
***********************************************************************/
#include "main.h"

static u8 MotorA_zero_Flag  = 0; //监测到电机A零点标志位
//static u8 MotorAstopFlag  = 0;  //转向A电机停止标志位
static volatile int16_t Diversion1_value = 0;//电机1当前角度值
static volatile int32_t ZeroDiversionA_value = 0;//电机1零点角度值
static int8_t Approach1DiversionFlag = 0;//接近开关1初始状态

//static u8 ScramFlag = 0;     //急停标志，按下急停置位
//static u8 StartFlag = 0;     //启动标志，按下启动置位

/*-------------------------------------------------------
函数名：MotorA_Zero_SetFlag
功  能：设置电机A零点标志
        到零点置位
参  数：无
返回值：无
--------------------------------------------------------*/
void MotorA_Zero_SetFlag(void)
{
  MotorA_zero_Flag = 1;
}
/*-------------------------------------------------------
函数名：MotorA_Zero_SetFlag
功  能：清除电机A零点标志
参  数：无
返回值：无
--------------------------------------------------------*/
void MotorA_Zero_CleanFlag(void)
{
  MotorA_zero_Flag = 0;
}
/*-------------------------------------------------------
函数名：MotorA_Zero_ISFlag
功  能：判断标志位状态      
参  数：无
返回值：无
--------------------------------------------------------*/
u8 MotorA_Zero_ISFlag(void)
{
 return MotorA_zero_Flag;
}
/********************************************************************************************************
**函数信息 :uint8_t TurningStop(void)
**功能描述 :停止转向
**输入参数 :
**输出参数 :0成功 or 错误编号
*********************************************************************************************************/
uint8_t TurningStop(void)
{
	uint8_t uReturnValue;//返回值
	uReturnValue = 1;//初始值为真
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
**函数信息 :uint8_t SeekZeroTurning(int16_t TurnSpd)
**功能描述 :寻零位
**输入参数 :TurnSpd 转动速度
**输出参数 :0成功 or 错误编号
*********************************************************************************************************/
uint8_t SeekZeroTurning(int16_t TurnSpd)
{
	uint8_t uReturnValue;//返回值
	uReturnValue = 1;//初始值为真
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
  /*------------获取当前相对零点位置，是在左还是右-------------*/
	scan_data.zero = GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_3);
  if(scan_data.zero == 0)
  {
    Approach1DiversionFlag = 1;
  }
  else
	{
    Approach1DiversionFlag = -1;
  }
/*--------------1号轮寻零点位----------------------*/  
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
						 TurningStop();//停止
						 for(i=0;i<ZERO_APPROACH1_LOFFSET;i++)
						 {
							errno= SeekZeroTurning(SEEK_ZERO_UCLOCK_SLOW);//反转
							 scan_data.zero = GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_3);
							 if(flag == 1 && scan_data.zero == 0)
							{
                                                          if(errno==ERR_000){
									TurningStop();//停止
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
**函数信息 :uint8_t ReadRealTimeFdbSpd(int16_t LeftWheelFdbSpd,int16_t RightWheelFdbSpd)
**功能描述 :驱动电机
**输入参数 :
**输出参数 :
*********************************************************************************************************/
//uint8_t ReadRealTimeFdbSwitchFreq(void)
//{
//	uint8_t uReturnValue;//返回值
//	
//	uReturnValue =1;//初始值为真
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
**函数信息 :uint8_t MovePositionSet(int32_t position)
**功能描述 :设置驱动轮转动位置
**输入参数 :position  转动绝对位置
**输出参数 :0成功 or 错误编号
*********************************************************************************************************/
uint8_t MovePositionSet(int32_t position)//电机转动到指定位置
{
	uint8_t uReturnValue;//返回值
	int16_t ubuffH,ubuffL;
	int32_t realposition;
	uReturnValue = 1;//初始值为真
	//Move driving wheel
	realposition = position + cha_state.zero_Value;  //实际偏转值=偏转位置+虚拟零位误差值
	ubuffH = (int16_t)(realposition >> 16);    //位置的高字节
	ubuffL = (int16_t)realposition;    //位置的低字节
	uReturnValue = MB_WritePluralReg(WHEEL_DRIVE_ADR, REG_TARGET_SPEED_INPOM, 0x0004,0x08,0x1388,0x0000,ubuffH,ubuffL);//已0x1388的速度转到指定位置。

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
//	uint8_t uFuncReturnValue;//调用的函数的返回值
//	uint16_t i;//循环变量
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
