#ifndef __GPIO_H
#define __GPIO_H

#include "sys.h"
#define LEDR  PCout(4)
#define LEDB  PBout(0)
#define LEDY  PCout(5)   //0:����
#define BEEP  PCout(2)   //1:����

//�������   ���߼�
#define BAOZ     PGout(9)  //��բ  
#define CCUP     PIout(11) //�����   
#define CCDN     PCout(1)  //��ݽ�
#define ZDCD     PCout(13) //�Զ����     
#define WRED     PGout(13) //��ɫ��
#define WGEE     PAout(0)  //��ɫ��
#define WYEL     PAout(2)  //��ɫ��

//���벿��
#define STOP     PIin(8)        //��ͣ����  H 
#define LSCB     PAin(3)        //��������  L
#define ZBJC     PBin(11)       //ջ����  L
#define ZXLW     PGin(3)        //ת����λ  L/H
#define CCCX     PAin(4)        //��ݴ���  L
#define AUTO1    PAin(5)        //�Զ���ť  L
#define UPOK     PAin(6)        //��ݸ�λ  L
#define AUTO2    PAin(7)        //�Զ�����//��ҩ���𻵣������豸ʹ�ô����š�

#define manbz    2      //��բ����λ
#define manspeed 1      //�ٶ���Ч����λ
#define manupdn  0      //�����������λ

void GPIOInit(void);

#endif


