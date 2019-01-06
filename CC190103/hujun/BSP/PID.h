#ifndef __PID_H
#define __PID_H
#include "main.h"
typedef struct  
{ 
	int Target;     //设定目标 Desired Value
	int Uk;			//Uk
	int Udk;		//Udk增量
	int Uk_1; 		//Uk-1
	double P; 		//比例常数 Proportional Const 
	double I; 		//积分常数 Integral Const
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