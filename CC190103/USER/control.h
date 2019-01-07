#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define STOP IN1        //��е���� ������
#define CCBZ IN2        //��ݱ��� ������
#define SDOK IN3        //�ֶ��ź� ������
#define ZBOK IN4        //ջ�嵽λ ������
#define ZBAQ IN5        //ջ�尲ȫ ������
#define BIZ1 IN6        //�������� ������
#define BIZ2 IN7        //�������� ������

#define GO   OUT0               //�͵�ƽ���ߵ�ƽ��ֹ
#define CCON OUT7               //�͵�ƽ���ߵ�ƽ��ͣ

#define speedmid 1400   //�ٶȿ�����λ  ǰ���� 300��170�綯  400��400  500��1000 600��2000---900��5000 1000��6000��<5800��
                        //      ����ֵ�����ˣ� 450��200      500��500  550��800 600��1100 650:1400 700:1700 750:2000
#define speedaq  200    //�ٶȵ��ٽ�ֹ
#define lrmid    1700   //���ҿ�����λ  ÿ100����50
#define udmid    1400   //���¿�����λ  400�綯  900����
#define highbase 248    //�߶Ȼ�׼ֵ�����λ�������߶�
#define highmax  2900   //�߶����ֵ
#define highmin  31     //���λ,�ܽ��������λ��
#define highslow 5      //������λ��
#define highfast 9     //��������
#define lturn    -83     //��ת��̶����
#define lcount   4160   //��ת��180����
#define rturn    77   //��ת��̶����
#define rcount   4280   //����ת��180����
#define lrspeed  3      //����ת���ٶȡ�
//const s16 speed[]={750,750,700,650,600,550,500,450,0,300,400,500,600,700,800,900,1000};
//AOUT(0)  SPEED   650    1260 1384  1540   2555
//AOUT(1,2)TURN
//LEFT 3021  1670     340
//RIGHT 350  1670    2978
//AOUT(3,4) UP/DOWN
//UP    384     1277    2340
//DOWN  2375    1508    490



typedef struct _SYS_ST{
  s32 st_meterhz;       //С����ʻ������
  s32 st_meter;      //С����̼�
  s16 st_speed;      //С���ٶ�
  s16 con_speed;     //С�������ٶȡ�

  s16 st_turn;       //С��ʵ��ת���
  s16 const_turn;       //����ת�����ֵ
  s16 con_turn;         //С������ת��

  s16 st_high;       //��ݸ߶ȡ�
  s16 con_highsp;         //�����ٶ�ֵ��
  s16 con_highset;      //�����߶�ֵ��   ���߶�Ϊ����Чֵ��0--3000��ʱ���������ٶȿ��ơ����߶Ȳ��ɳ�����Ч��Χ��ǰ��߶��ѱ��㣨highinit=0xaa����
  s16 con_highwork;      //�Զ����������С�
    
  s16 st_dy;         //ϵͳ��ѹ
  s16 st_dl;         //������� 
  s16 st_contp;      //�������¶�
  s16 st_mototp;     //����¶�
  u8  highinit;      //highinit:0xA0  �����������̡�0xA1���½�������㣬0xA2������������㡣0xAA����ʼ����ɡ�0xAF ��ʼ��ʧ�ܡ���
  u8  st_mod;        //���Ʒ�ʽ  1���Զ� 0���ֶ���
  u8  st_stat;       //С��״̬  1������ 0��������         ����������
  u8  st_man;        //С��̤�����ˡ�̤�����ʱ����������ź�  ���źŲ���
  u8  st_conok;        //�ֱ����£�С���ɿ� ��            ����������
  u8  st_tbopen;        //̤����¡�
  u8  st_ryk;          //�Ҳ��������¡�
  u8  st_lyk;          //����������¡�
  u8  dd_com;          //��ǰ��������
  u8  dd_val;          //��ǰ�������������
  u8  dd_ok;            //�յ��Ϸ����Ȱ���
  u8  err;             //���߼���
  
  
}xSYS_D;
extern xSYS_D SYSST;
void control(void* pa);
void high_init(void);