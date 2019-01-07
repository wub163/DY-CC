#ifndef _MOTOR_TASK_H_
#define _MOTOR_TASK_H_

#define TASK_TEST_USART_STK_SIZE	400
#define TASK_TEST_RS232_STK_SIZE	400
#define TASK_TEST_CAN_STK_SIZE	400
#define CMD_WHEEL_VEL  15
#define CMD_WHEEL_ANGLE  4.189
#define CMD_MSG_ANGLE  1  //4.656


#pragma   pack(1)//���ֽڶ���
typedef struct sent_st
{
u8 head;                //0X55
u8 lenth;               //���� 20

  u8  LEDST;              //��ɫ��״̬��
  u8  LifterState;        //�泵״̬��0����λ 1����λ��2�������С���
  s8  CCSTAT;           // �泵״̬λ��STOP<<7|LSCB<<6|ZBJC<<5|ZXLW<<4|CCCX<<3|AUTO<<2|UPOK<<1|0;
  s8  dcwd;             //����¶�
  u16 dcdy;             //��ص�ѹ
  s16 dcdl;             //��ǰ����
  s16 FdbMotorVel;        //cha_state.wheelst.FdbMotorVel
  s16 RealPosition;        //cha_state.turn_moto_getpro.RealPosition / CMD_MSG_ANGLE
  s32 speedPosition;       //���ߵ��ʵʱλ�á�
  
  

u8 crc;
u8 end;                 //0XAA

}sentst;



typedef struct recv_st
{
u8 head;                //0X55
u8 lenth;               //����

u8 LifterCtrl;          //������0����1����
u8 Led_Control;         //�ƹ���ơ�0�ر� ��1������  2��������3��������
u8 chargeswitch;        //�򿪳��̵�����
u8 mp3mute;             //0:������0xff��ϵͳĬ�ϡ�����ֵΪ������
s16 CmdMsgVel;          //�ٶ�
s16 CmdMsgAngle;      //���ֽǶ�



u8 crc;
u8 end;
}recvst;                //��β0XAA

#pragma   pack()




extern SemaphoreHandle_t sem_RS485_rec_flag;			//RS485������һ�������ź�������
extern SemaphoreHandle_t sem_RS232_rec_flag;
extern SemaphoreHandle_t sem_CAN_rec_flag;			//CAN1������һ�������ź�������

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