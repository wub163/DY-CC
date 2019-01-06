#ifndef _MOTOR_TASK_H_
#define _MOTOR_TASK_H_

#define TASK_TEST_USART_STK_SIZE	400
#define TASK_TEST_RS232_STK_SIZE	400
#define TASK_TEST_CAN_STK_SIZE	400
#define CMD_WHEEL_VEL  15
#define CMD_WHEEL_ANGLE  4.189
#define CMD_MSG_ANGLE  1  //4.656


#pragma   pack(1)//单字节对齐
typedef struct sent_st
{
u8 head;                //0X55
u8 lenth;               //包长 20

  u8  LEDST;              //三色灯状态。
  u8  LifterState;        //叉车状态，0，地位 1，高位。2，升降中。。
  s8  CCSTAT;           // 叉车状态位。STOP<<7|LSCB<<6|ZBJC<<5|ZXLW<<4|CCCX<<3|AUTO<<2|UPOK<<1|0;
  s8  dcwd;             //电池温度
  u16 dcdy;             //电池电压
  s16 dcdl;             //当前电流
  s16 FdbMotorVel;        //cha_state.wheelst.FdbMotorVel
  s16 RealPosition;        //cha_state.turn_moto_getpro.RealPosition / CMD_MSG_ANGLE
  s32 speedPosition;       //行走电机实时位置。
  
  

u8 crc;
u8 end;                 //0XAA

}sentst;



typedef struct recv_st
{
u8 head;                //0X55
u8 lenth;               //包长

u8 LifterCtrl;          //插齿命令，0降，1升。
u8 Led_Control;         //灯光控制。0关闭 ，1，红闪  2，黄闪。3，绿闪。
u8 chargeswitch;        //打开充电继电器。
u8 mp3mute;             //0:静音，0xff。系统默认。其它值为音量。
s16 CmdMsgVel;          //速度
s16 CmdMsgAngle;      //舵轮角度



u8 crc;
u8 end;
}recvst;                //包尾0XAA

#pragma   pack()




extern SemaphoreHandle_t sem_RS485_rec_flag;			//RS485接收完一桢数据信号量定义
extern SemaphoreHandle_t sem_RS232_rec_flag;
extern SemaphoreHandle_t sem_CAN_rec_flag;			//CAN1接收完一桢数据信号量定义

extern u8* RS232DATA;
extern u8* RS485DATA;



void Task_USART(void *pdata);
void Task_RS232(void *pdata);
void Task_CAN(void *pdata);
void MotorCtrl_init(void);
void MP3_Control(void);
void Motor_Init(void);



extern int TIM5_times_100ms_flag;
extern int Report21Info(void);





#endif