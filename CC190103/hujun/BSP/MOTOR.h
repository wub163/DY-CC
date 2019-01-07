#ifndef __MOTOR_H
#define __MOTOR_H
#include "main.h"
//�������ӵ�485ID
//#define WHEEL_LEFT_ADR		  1
#define WHEEL_DRIVE_ADR     1
//#define WHEEL_RIGHT_ADR		  2


//�������ӵķ���
#define DIR_FRONT			  (1)
#define DIR_BACK			  (2)

//485 �쳣
#define ERR_000	0 //no error
#define ERR_010	10 //����ͨѶ��ʱ
#define ERR_011	11 //����ͨѶ��ʱ
#define ERR_012	12 //����ͨѶЭ��ջ�Ƿ�
#define ERR_013	13 //����ͨѶЭ��ջ�Ƿ�
#define ERR_014	14 //����ͨѶCRC����
#define ERR_015	15 //����ͨѶCRC����
#define ERR_016	16 //ͨѶ��ʱ
#define ERR_017	17 //ͨѶЭ��ջ�Ƿ�
#define ERR_018	18 //ͨѶCRC����
#define ERR_060 60 //��������
#define SEEK_ZERO_CLOCK_FAST 0xFC18 //����Ѱ��λ
#define SEEK_ZERO_UCLOCK_FAST 0x03E8 //����Ѱ��λ
#define SEEK_ZERO_CLOCK_SLOW 0xFE0C //0xFF9C //����Ѱ��λ
#define SEEK_ZERO_UCLOCK_SLOW 0x01F4 //0x0064 //����Ѱ��λ
#define ZERO_APPROACH1_ROFFSET 5
#define ZERO_APPROACH1_LOFFSET 10
#define OpenLoop_Mode                       0x01
#define Current_Mode                        0x02
#define Velocity_Mode                       0x03
#define Position_Mode                       0x04
#define Velocity_Position_Mode              0x05
#define Current_Velocity_Mode               0x06
#define Current_Position_Mode               0x07
#define Current_Velocity_Position_Mode      0x08

#define GROUP0       0x00
#define GROUP1       0x01
#define GROUP2       0x02
#define GROUP3       0x03
#define GROUP4       0x04
#define GROUP5       0x05
#define GROUP6       0x06
#define GROUP7       0x07
#define NUMBER1      0x01           
#define NUMBER2      0x02
#define NUMBER3      0x03
#define NUMBER4      0x04
#define NUMBER5      0x05
#define NUMBER6      0x06
#define NUMBER7      0x07
#define NUMBER8      0x08
#define NUMBER9      0x09
#define NUMBER10     0x0A
#define NUMBER11     0x0B
#define NUMBER12     0x0C
#define NUMBER13     0x0D
#define NUMBER14     0x0E
#define NUMBER15     0x0F

#define MAX_RUNVELOCITY  (1000)   //����ٶ���0.65m/s,���ÿתһȦ�ľ�����26.93mm 0.5m/s ��1113r/min 0.15m/s 340r/min 0.1m/s 222.799r/min ȡ��223  0.135m/s ת����300r/min
#define MIN_RUNVELOCITY (-1000)


void InitMotor(void);
uint8_t MovePositionSet(int32_t position);
extern int Seek_zero(void);
extern uint8_t TurningStop(void);
extern void MotorA_Zero_CleanFlag(void);
extern void CAN_RoboModule_DRV_Reset(unsigned char Group,unsigned char Number);
extern void CAN_RoboModule_DRV_Mode_Choice(unsigned char Group,unsigned char Number,unsigned char Mode);
extern void CAN_RoboModule_DRV_Config(unsigned char Group,unsigned char Number,unsigned char Temp_Time,unsigned char Ctl1_Ctl2);
extern void CAN_RoboModule_DRV_Online_Check(unsigned char Group,unsigned char Number);
extern void CAN_RoboModule_DRV_Velocity_Mode(unsigned char Group,unsigned char Number,short Temp_PWM,short Temp_Velocity);
extern void DriveMotorControl(uint16_t PWM,int16_t VelocityA);
extern void CAN_Delay_Us(unsigned int t);

extern uint8_t test_MotorCommunication(uint8_t uADR, uint16_t uTestCount);

extern uint8_t ReadRealTimeFdbSwitchFreq(void);



#endif