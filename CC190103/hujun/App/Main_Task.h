/***********************************************************************
文件名称：
功    能：
编写时间：
编 写 人：
注    意：
***********************************************************************/
#ifndef _MAIN_TASK_H_
#define _MAIN_TASK_H_

#define TASK_TEST_LED_STK_SIZE	400
#define TASK_TEST_WIFI_STK_SIZE	400
#define TASK_TEST_MOTOR_STK_SIZE	1024
#define TASK_TEST_MAIN_STK_SIZE	1024
#define IN_ON	1
#define IN_OFF	0
#define LifterUP 1
#define LifterDOWN 0
#define LifterCHANGING 2

#include "main.h"
//extern QueueHandle_t xsem_WIFI_rec_flag;


void Task_LED(void *pdata);
void Task_WIFI(void *pdata);
void Task_MAIN(void *pdata);
void Task_MOTOR(void *pdata);

typedef unsigned char State;
typedef void (*Procedure)(void *);

enum states{ s_seekzero,s_auto,s_stop,s_crash};//????

struct Scan_data
{
                int8_t zero;
		int8_t fork;
		int8_t lifter;
		int8_t Auto;
		int8_t risen;
		int8_t stop;
		int8_t crash;
		int8_t nouse;
};

//转向电机参数
typedef struct Turn_Motor03_GET
{
	uint8_t  ADR;
	uint8_t  CMD;
	uint16_t StartRegister; 
	uint16_t RegisterCount;
}TurnMotorGet;

typedef struct Turn_Motor03_GETPROCESS
{
	uint8_t  COUNT;
	int16_t  Revolutions;
	uint8_t  TurnctrlFlag;
	int32_t  RealPosition;
	uint32_t RemainingTime;
}TurnMotorGetProcess;

typedef struct WheelStruct
{
  short FdbSwitchFreq;
	short CmdSwitchFreq;

	int16_t FdbMotorVel;
	float FdbWheelVel;       //用以保存轮子速度，单位m/s
	

	int16_t CmdMsgVel;
	s16 CmdMsgAngle;
	
//	int16_t LifterInVel;
//	int16_t LifterInAngle;
//	
//	int16_t LifterOutVel;
//	int16_t LifterOutAngle;
	
	int16_t CmdWheelVel;       //用以保存轮子速度，单位 -100%~100%
	int16_t CmdWheelAngle;		 //用以保存轮子转动角度 取值范围-100~100
	
	int16_t CmdMotorvel;				//用以保存电机转速，单位r/min
	int16_t CmdMotorAngle;			//用以保存转向电机的位置值 -419~418
        s32     CmdMotorPwm;                    //电机力度。
  
	int SpdctrlRespCnt;
	int SwitchfreqRespCnt;
}WheelSt;

typedef struct _AC_STA  //????????
{
    int stateid;
		int first;
		int next;
		int zero_Flag;
		uint8_t motoinit;
		uint8_t MotoRestart;
	  int32_t zero_Value;
	
		int TurnctrlRespCnt;
		int TurnfreqRespCnt;
		int FlashRespCnt;
	
		int LifterCtrl;
		int LifterState;
		int LifterChange;
		int Battery_Temp;//单位1度。
	  uint16_t Battery_Voltage;//单位1mv
	  s16      Battery_Current;//单位10ma
          u8       chargeswitch;//充电继电器0，关闭，1，打开。
		uint8_t HandleMove;
		uint8_t Led_State;
		uint8_t Led_Control;
		uint8_t Crashon;
		uint8_t MP3_Control;
		uint8_t MP3_State;
                
                u8      Mp3_num;//播放文件号
		uint8_t MP3_Vol;//控制音量，如果大于25，则为系统默认音量。
                
                u8      mp3mute;
                
                u8      manctrl;//2,抱闸。1，速度。0，顶升。
                
                u8    CAR_STATE;//叉车传感器

		uint8_t Cmd_Stop;
		uint8_t Auto_State;
	
		uint16_t LifterInput;

		unsigned int timecount;
		unsigned int cancount;
		unsigned char timeshare;
	
		WheelSt wheelst;
	
		TurnMotorGet turn_moto_get;
		TurnMotorGetProcess turn_moto_getpro;

			
}AC_STA;

extern AC_STA cha_state;
extern struct Scan_data scan_data;
extern CanRxMsg CanMsg;

#endif
