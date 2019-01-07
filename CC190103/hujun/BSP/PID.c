#include "main.h"


/*==================================================================================================== 
						����ʽPID
The PID (���������֡�΢��) function is used in mainly 
control applications. PIDCalc performs one iteration of the PID 
algorithm. 
While the PID function works, main is just a dummy program showing 
a typical usage. 
=====================================================================================================*/
#define ADC_Speed_Max   7	 //
#define ADC_Speed_Min   -7
#define PWM_Max			90
#define PWM_Min     	-90

int ADC_Speed = 0;
   

 Pid Front_PID; 
 Pid Back_PID;

/*==================================================================================================== 
Initialize PID Structure PID������ʼ��
=====================================================================================================*/ 
void Speed_PIDInit(Pid *Speed_Point) 
{ 
	Speed_Point->Target = 0;
	Speed_Point->Uk		= 0;
	Speed_Point->Udk    = 0;
	Speed_Point->Uk_1   = 0;
	Speed_Point->ek_0 	= 0;	 //ek=0
	Speed_Point->ek_1 	= 0;	 //ek-1=0
	Speed_Point->ek_2 	= 0; 	 //ek-2=0
	Speed_Point->P 		= 7.65;  //�������� Proportional Const 
	Speed_Point->I 		= 0.005; 	 //���ֳ���Integral Const
	Speed_Point->b      = 0; 
}
///*==================================================================================================== 
//����ʽPID���㲿�� 
//=====================================================================================================*/ 
//int Speed_PIDAdjust(int Next_Point) 
//{
//	if(ADC_Speed < ADC_Speed_Min)
//	{
//	 	ADC_Speed = ADC_Speed_Min;		
//	}
//	else if(ADC_Speed > ADC_Speed_Max)
//	{
//		ADC_Speed = ADC_Speed_Max;
//	}
//	Speed_Point->Target = ADC_Speed; 
//	Speed_Point->ek_0= Speed_Point->Target - Next_Point; 	//��������
//	if(((Speed_Point->Uk_1>=PWM_Max)&&(Speed_Point->ek_0>=0))||((Speed_Point->Uk_1<=PWM_Min)&&(Speed_Point->ek_0<=0)))
//	{
//	    Speed_Point->b=0;
//	} 
//	else
//	{
//		Speed_Point->b=1;
//	} 
//	Speed_Point->Udk=Speed_Point->P*(Speed_Point->ek_0-Speed_Point->ek_1) + Speed_Point->b*Speed_Point->I*Speed_Point->ek_0 ;
//    /* �洢�������´μ��� */
//	Speed_Point->Uk = Speed_Point->Uk_1 + Speed_Point->Udk;
//	Speed_Point->ek_2 =	Speed_Point->ek_1;
//	Speed_Point->ek_1 = Speed_Point->ek_0;
//	Speed_Point->Uk_1 = Speed_Point->Uk; 
//    if(Speed_Point->Uk >= PWM_Max)
//	{
//		return PWM_Max;
//	}
//	else if(Speed_Point->Uk <= PWM_Min)
//	{
//		return PWM_Min;
//	} 
//	return(Speed_Point->Uk); 
//}
/*
*------------------------------------------------------------------------------
*��������Taprevise
*���ܣ�
*������
*����ֵ��
*------------------------------------------------------------------------------
*/
int16_t Taprevise(Pid *Speed_Point,int8_t Next_Point,double p,double i,int speed)
{
  	Speed_Point->P 		= p;  //�������� Proportional Const 
	Speed_Point->I 		= i; 	 //���ֳ���Integral Const
	if(ADC_Speed < ADC_Speed_Min)
	{
	 	ADC_Speed = ADC_Speed_Min;		
	}
	else if(ADC_Speed > ADC_Speed_Max)
	{
		ADC_Speed = ADC_Speed_Max;
	}
        Speed_Point->Target = speed; 
	Speed_Point->ek_0 = Speed_Point->Target-Next_Point; 	//��������
 	if(((Speed_Point->Uk_1>=PWM_Max)&&(Speed_Point->ek_0>=0))||((Speed_Point->Uk_1<=PWM_Min)&&(Speed_Point->ek_0<=0)))
	{
	    Speed_Point->b=0;
	} 
	else
	{
		Speed_Point->b=1;
	} 
	Speed_Point->Udk = (int)(Speed_Point->P*(Speed_Point->ek_0-Speed_Point->ek_1) + Speed_Point->b*Speed_Point->I*Speed_Point->ek_0) ;
    /* �洢�������´μ��� */
	Speed_Point->Uk = Speed_Point->Uk_1 + Speed_Point->Udk;
	Speed_Point->ek_2 =	Speed_Point->ek_1;
	Speed_Point->ek_1 = Speed_Point->ek_0;
	Speed_Point->Uk_1 = Speed_Point->Uk; 
    if(Speed_Point->Uk >= PWM_Max)
	{
		return PWM_Max;
	}
	else if(Speed_Point->Uk <= PWM_Min)
	{
		return PWM_Min;
	} 
	return(Speed_Point->Uk);                

}
