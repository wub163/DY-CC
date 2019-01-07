#ifndef __MOTO_H
#define __MOTO_H

#include "sys.h"

#define MOTO01  PAout(2) //1���ػ�         0������
#define MOTO02  PAout(3) //1,��            0����

#define MOTO11  PCout(4) //1���ػ�         0������
#define MOTO12  PCout(5) //1,��            0����

#define MOTO21  PDout(14) //1���ػ�         0������
#define MOTO22  PDout(15) //1,��            0����

#define MOTO31  PBout(1) //1���ػ�         0������
#define MOTO32  PEout(7) //1,��            0����

#define MOTO0D          {MOTO01=0;MOTO02=1;}//��ת
#define MOTO0U          {MOTO01=1;MOTO02=0;}//��ת
#define MOTO0B          {MOTO01=0;MOTO02=0;}//ɲ��
#define MOTO0OFF        {MOTO01=1;MOTO02=1;}//�ػ�

#define MOTO1D          {MOTO11=0;MOTO12=1;}//��ת
#define MOTO1U          {MOTO11=1;MOTO12=0;}//��ת
#define MOTO1B          {MOTO11=0;MOTO12=0;}//ɲ��
#define MOTO1OFF        {MOTO11=1;MOTO12=1;}//�ػ�

#define MOTO2D          {MOTO21=0;MOTO22=1;}//��ת
#define MOTO2U          {MOTO21=1;MOTO22=0;}//��ת
#define MOTO2B          {MOTO21=0;MOTO22=0;}//ɲ��
#define MOTO2OFF        {MOTO21=1;MOTO22=1;}//�ػ�

#define MOTO3D          {MOTO31=0;MOTO32=1;}//��ת
#define MOTO3U          {MOTO31=1;MOTO32=0;}//��ת
#define MOTO3B          {MOTO31=0;MOTO32=0;}//ɲ��
#define MOTO3OFF        {MOTO31=1;MOTO32=1;}//�ػ�

//�������������
//#define MOTOTO      PDout(14)
//#define MOTOOFF     PDout(15)
#define HTHIGH        PDin(13)
#define HTLOW         PDin(12)
#define YTLOW         PAin(4)

#define GearScale               27       //���ٱ�
#define MotorEncoder            1000     //Ȧ����
#define WheelPeri               95      //��ֱ����
#define MotoLength              300     //�����־�

s32 setspeed(s16 speed);
typedef struct _MOTO_ST{
        s16 comspeed;                        //��������ٶ� +-100
        s16 comhz;                        //ʵ�ʸ���Ƶ��   +-200    
        s16 lasthz;                       //�ϴθ���Ƶ��
        s16 cuspe;                       //�����ǰ�ٶ�,(ҲΪ���ε��õ�λ�ò)
        s16 lastcuspe;
        u8  err;                        //<251ʧ��ʱ�䣬254�����ת
        u8  dzstop;
        s32 pos;                         //��ǰ���λ��( ������)
        s32 lastpos;                    //��ʷ���λ�ã��������� �����ٶ��á�
        s32 highpos;                    //���100%��λֵ��
        s32 conpos;
    }xMOTO_D;


extern xMOTO_D moto[];//��̨������ٶȸ���ֵ��+-100��0,1������ 2������̨��3����̨��
void mototask(void* pv);


#endif 

