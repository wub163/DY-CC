/***********************************************************************
�ļ����ƣ�
��    �ܣ�
��дʱ�䣺
�� д �ˣ�
ע    �⣺
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

//ת��������
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
	float FdbWheelVel;       //���Ա��������ٶȣ���λm/s
	

	int16_t CmdMsgVel;
	s16 CmdMsgAngle;
	
//	int16_t LifterInVel;
//	int16_t LifterInAngle;
//	
//	int16_t LifterOutVel;
//	int16_t LifterOutAngle;
	
	int16_t CmdWheelVel;       //���Ա��������ٶȣ���λ -100%~100%
	int16_t CmdWheelAngle;		 //���Ա�������ת���Ƕ� ȡֵ��Χ-100~100
	
	int16_t CmdMotorvel;				//���Ա�����ת�٣���λr/min
	int16_t CmdMotorAngle;			//���Ա���ת������λ��ֵ -419~418
        s32     CmdMotorPwm;                    //������ȡ�
  
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
		int Battery_Temp;//��λ1�ȡ�
	  uint16_t Battery_Voltage;//��λ1mv
	  s16      Battery_Current;//��λ10ma
          u8       chargeswitch;//���̵���0���رգ�1���򿪡�
		uint8_t HandleMove;
		uint8_t Led_State;
		uint8_t Led_Control;
		uint8_t Crashon;
		uint8_t MP3_Control;
		uint8_t MP3_State;
                
                u8      Mp3_num;//�����ļ���
		uint8_t MP3_Vol;//�����������������25����ΪϵͳĬ��������
                
                u8      mp3mute;
                
                u8      manctrl;//2,��բ��1���ٶȡ�0��������
                
                u8    CAR_STATE;//�泵������

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
