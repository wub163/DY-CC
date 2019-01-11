/*
******************************************************************************************************
*
*  Copyright (C),   2014-2016, 哈尔滨工大服务机器人有限公司
*  FileName:        Main_Task.c
*  Version:         ver1.0
*  Author:          HJ
*  Date:            2018-3-15
*  Description:     
*  Maintainers:     none
*
******************************************************************************************************
*/
#include "main.h"  

AC_STA cha_state;
struct Scan_data scan_data;
int seekvel = 0;
int seekcount = 0;
int batterycount = 1;
uint8_t LifterctrlRespCnt = 0;

/******************************************************************
	* Function:          void State_Check(void)
  * Description:       子任务函数
	* Input parameters:  
	* Return parameters: void
********************************************************************/
void State_Check(void)
{						
	
				  scan_data.fork = ZBJC;
				  scan_data.lifter = CCCX;
				  scan_data.Auto = AUTO1;
				  scan_data.risen = UPOK;
				  scan_data.zero = ZXLW;
				  scan_data.nouse = AUTO2;
				  scan_data.stop = STOP;
				  scan_data.crash = LSCB;
        cha_state.CAR_STATE=STOP<<7|LSCB<<6|ZBJC<<5|ZXLW<<4|CCCX<<3|(AUTO1&AUTO2)<<2|UPOK<<1|0;      
}

/***********************************************************************
函数名称：void Task_LED(void *pdata)
功    能：
输入参数：
输出参数：
编写时间：
编 写 人：
注    意：
***********************************************************************/
void Task_LED(void *pdata)
{		 
u16 count=0;

    while(1)
	{

          if((count++%25)==0){  cha_state.Led_State=cha_state.Led_Control;
          if(!(AUTO1&&AUTO2))
          {switch((cha_state.Led_Control)){
            case 1:
                            WRED=!WRED;WYEL=0;WGEE=0;
              break;
            case 2:
                            WRED=0;WYEL=!WYEL;WGEE=0;

              break;
            case 3:
                            WRED=0;WYEL=0;WGEE=!WGEE;

              break;
          
          }
            
            }    
}
        
if( cha_state.Battery_Current<-10)//充电情况指示
{if((count++%15)==0){
  if(cha_state.Battery_Temp>powerchtemp){ WRED=0;WYEL=!WYEL;WGEE=0;}
                                  else {WRED=!WRED;WYEL=0;WGEE=0;}
}}

        
        
        
          State_Check();
	vTaskDelay(20/portTICK_PERIOD_MS);
	}
}
/******************************************************************
	* Function:          void step_seekzero(void * sta)
  * Description:       主任务函数
	* Input parameters:  void * sta
	* Return parameters: void
********************************************************************/
void step_seekzero(void * cha_state)
{
    AC_STA *p = (AC_STA *)cha_state;
    p->stateid = 0;	

    if(p->first == 1){
				p->first = 0;
				vTaskDelay(1000/portTICK_PERIOD_MS);
				p->zero_Flag = Seek_zero();
		}
		if(p->first == 2){
				p->first = 0;
				vTaskDelay(1000/portTICK_PERIOD_MS);
				p->zero_Flag = Seek_zero();
		}
		if(p->zero_Flag == 1){
				MB_Get03();
				p->zero_Value = position1; 
				seekcount ++;
		}
		if(seekcount > 4)
		{
				p->first = 1;
				p->next = s_auto;
				seekcount = 0;
		}
}


/******************************************************************
	* Function:          void step_auto(void * sta)
  * Description:       主任务函数
	* Input parameters:  void * sta
	* Return parameters: void
********************************************************************/
void step_auto(void * cha_state)
{
static u8 stopcount=0;
  AC_STA *p = (AC_STA *)cha_state;
    p->stateid = 1;

		if(p->first == 1)//
		{

				if(p->LifterChange == 1)
				{
						p->LifterChange = 0;
						p->LifterState = LifterDOWN;
				}
				p->first = 0;
				p->Auto_State = scan_data.Auto;
		}
		else
		{

		
				if(scan_data.stop == IN_ON )stopcount++;else stopcount=0;
                                   if(stopcount>10)
				{stopcount=250;
			p->first = 1;
      			p->next = s_stop;
				}
/*****************************************/
				if(p->LifterCtrl != p->LifterState)
				{
						p->LifterState = LifterCHANGING;
				}
				else
				{
						CCUP=0;
						CCDN=0;
				}
				
				if(p->LifterState == LifterCHANGING)
				{
						if(p->LifterCtrl)
						{
								CCDN=0;vTaskDelay(100/portTICK_PERIOD_MS);CCUP=1;
								LifterctrlRespCnt =0;

								if(scan_data.risen == 0)
								{
										CCUP=0;
										p->LifterState = LifterUP;
										//Report21Info();
								}
						}
						else
						{
								CCUP=0;vTaskDelay(100/portTICK_PERIOD_MS);CCDN=1;
                                                                if(UPOK==0)LifterctrlRespCnt =0;//防止插齿下降不成功。
								LifterctrlRespCnt ++;

								if(LifterctrlRespCnt > 30)
								{
										CCDN=0;
										LifterctrlRespCnt = 0;
										p->LifterChange = 1;
//										p->zero_Flag = 0;
										MotorA_Zero_CleanFlag();
										p->first = 1;
//										p->next = s_seekzero;
										p->next = s_auto;

										//Report21Info();
								}
						}
				}
		}
	
}
/******************************************************************
	* Function:          step_stop(void * cha_state)
  * Description:       主任务函数
	* Input parameters:  void * sta
	* Return parameters: void
********************************************************************/
void step_stop(void * cha_state)
{
		AC_STA *p = (AC_STA *)cha_state;
    p->stateid = 2;

		if(p->first == 1)
		{
				p->first = 0;	
		}
		else
		{
				if(scan_data.stop != IN_ON )
				
				{
						p->first = 1;
						p->zero_Flag = 0;
						MotorA_Zero_CleanFlag();
						p->next = s_seekzero;
				}
		}
}
///******************************************************************
//	* Function:          step_crash(void * cha_state)
//  * Description:       主任务函数
//	* Input parameters:  void * sta
//	* Return parameters: void
//********************************************************************/
//void step_crash(void * cha_state)
//{
//		AC_STA *p = (AC_STA *)cha_state;
//    p->stateid = 3;
//
//		if(p->first == 1)
//		{
//				p->MP3_Control = 2;
//				p->first = 0;	
//		}
//		else
//		{
//				if(scan_data.crash == 0 )
//				{
//						if((p->wheelst.CmdMsgVel == 0)&(p->wheelst.CmdWheelVel == 0))
//						{
//								p->HandleMove = 1;
//						}
//				}
//				else
//				{
//						if((p->wheelst.CmdMsgVel == 0)&(p->wheelst.CmdWheelVel == 0))
//						{
//								p->HandleMove = 0;
//								p->first = 1;
//								p->next = s_auto;
//						}
//				}
//		}
//}

/******************************************************************
	* Function:          void StateMachineRun(void * sta)
  * Description:       主任务函数
	* Input parameters:  void * sta
	* Return parameters: void
********************************************************************/
Procedure Steps[] = {step_seekzero,step_auto,step_stop};

void StateMachineRun(void * cha_state)
{
    AC_STA *p = (AC_STA *)cha_state;
		State NS = p->next; //??????
    Steps[NS](p);
}
/***********************************************************************
函数名称：void Task_MAIN(void *pdata)
功    能：
输入参数：
输出参数：
编写时间：
编 写 人：
注    意：
***********************************************************************/
void Cha_state_Init(void)
{		
	cha_state.first = 1;
	cha_state.next = s_seekzero;
	MotorA_Zero_CleanFlag();
	
	cha_state.zero_Value = 0;
	cha_state.zero_Flag = 0;
	cha_state.LifterState = LifterUP;
	cha_state.LifterCtrl = LifterDOWN;
	cha_state.LifterInput = 0;
//	cha_state.Led_State = 0;
	cha_state.MP3_Control = 0;
	cha_state.MP3_State = 0;
	cha_state.MP3_Vol = initmp3vol;
//        cha_state.MP3_initnum=initmp3num;
	cha_state.MotoRestart = 0;
        cha_state.wheelst.CmdMotorPwm=5000;
	cha_state.Cmd_Stop = 0;
	cha_state.Auto_State = 0;
	cha_state.Crashon = 0;
	cha_state.HandleMove = 0;
	cha_state.Battery_Temp = -10;
	cha_state.Battery_Voltage = 28000;
	//MR15_EN_GPIO_Config();
	//cha_state.motoinit = 1;
	
}
/***********************************************************************
函数名称：void Task_MAIN(void *pdata)
功    能：
输入参数：
输出参数：
编写时间：
编 写 人：
注    意：
***********************************************************************/
void Task_MAIN(void *pdata)
{		
	pdata = pdata;
	Cha_state_Init();
	
    while(1)
	{
		StateMachineRun(&cha_state);
		vTaskDelay(100/portTICK_PERIOD_MS);

	}
}