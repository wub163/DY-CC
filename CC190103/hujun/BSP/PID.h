#ifndef __PID_H
#define __PID_H
#include "main.h"
typedef struct  
{ 
	int Target;     //�趨Ŀ�� Desired Value
	int Uk;			//Uk
	int Udk;		//Udk����
	int Uk_1; 		//Uk-1
	double P; 		//�������� Proportional Const 
	double I; 		//���ֳ��� Integral Const
	int    b; 
	int ek_0;		//ek 
	int ek_1; 		//ek-1 
	int ek_2; 		//ek-2 
}Pid;

extern int ADC_Speed;
extern Pid Front_PID; 
extern Pid Back_PID;
void Speed_PIDInit(Pid *Speed_Point);
extern int Speed_PIDAdjust(int NextPoint);
int16_t Taprevise(Pid *Speed_Point,int8_t Next_Point,double p,double i,int speed);











#endif