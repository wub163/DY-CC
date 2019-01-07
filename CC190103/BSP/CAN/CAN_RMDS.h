#ifndef _MOTOR_RMDS_TASK_H_
#define _MOTOR_RMDS_TASK_H_

#include "sys.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "queue.h"
#include "can.h"

#define GearScale 23     //������
#define WheelPeri 240   //��ֱ��
#define MotorEncoder 5000 //��������
#define MotorLenth  400 //�־�
#define MotoFlagForward0   1
#define MotoFlagForward1   -1    //�����ֵ��ת��

#define mod 6//ģʽѡ�񣺵����ٶȡ�
//1������            P1P2                  ��������pwm
//2������            P1P2C3C4              ��pwm���Ƶ���
//3���ٶ�            P1P2V3V4              ��pwm�����ٶ�
//4��λ��            P1P2    PO4PO5PO6PO7  ��pwm����λ��
//5���ٶ�λ��        P1P2V3V4PO4PO5PO6PO7  ��pwm���ٶȿ���λ��
//6�������ٶ�        C1C2V3V4              ���Ƶ��������ٶ�
//7������λ��        C1C2    PO4PO5PO6PO7  ���Ƶ�������λ��
//8�������ٶ�λ��    C1C2V3V4PO4PO5PO6PO7  ���Ƶ����ٶȿ���λ��
#define set1  20  //����������������10ms��
#define set2  0  //����������λ���͹��ܣ�0��Ϊ�رա�
//#define set2  0x01  //����������λ���͹��ܣ�0��Ϊ�رա�
#define drvid0 0x100 //�㲥��ַ ��2���������㲥��ַ�����+��ַ��+���ܺţ����ܺţ�0����λ 1��ģʽѡ��  2--9 ģʽ��+1  10������  11�����������ݷ��� 12����������λ������ 
#define drvid1 0x110 //�㲥��ַ 1����������ַ
#define drvid2 0x120 //�㲥��ַ 2����������ַ
#define resetmod 0              //��λģʽ
#define modmod   0x01           //ģʽѡ��
#define setmod   0x0a           //����ģʽ                  //����ģʽ���ɹ㲥
//#define cvmod 0x07   //�����ٶ�ģʽ
#define pvmod 0x04  //�ٶ�ģʽ


typedef struct _CANRMDS_ST{
      s16 cspeed;       //�����ٶ�
      s16 cpwm;     //���Ƶ��������Ƶ�����
      s16 sspeed;       //�����ٶ�
      s16 scurrent;     //��������
      s32 position;     //��ǰλ�á�����������
      s32 posmm;         //��ǰλ��  ��mmֵ ��
      s8  limit;        //d0:����λ d1:����λ
      u8  DRVERR;       //���������� .1��Ƶ�ʵĴ����ۼӡ�
      s8  MotoFlagForward; //���ת��
}xCANRMDS_D;


extern xCANRMDS_D CANDD; 
extern QueueHandle_t xCANRMDS;  //���жϽ���

extern void RMDS_task(void *pv);//��������������

#endif