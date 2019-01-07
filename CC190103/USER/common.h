#ifndef __COMMON_H
#define __COMMON_H

#include "stdio.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "can.h"
#include "24cxx.h" 
#include "gpio.h"
#include "main.h"
#include "NVIC.h"
#include "spi_24l.h"
#include "spi_flash.h"
#include "LCD1963.h"
#include "DS2780.h"
#include "sram.h"
#include "malloc.h"
#include "COM1_LCM.h"
#include "COM2_Zigbee.h"
#include "COM5_Mp3.h"
#include "COM6_Motor.h"
#include "math.h"

#include "CAN_Magnetic.h"
#define LED0  PFout(9)
#define LED1  PFout(10)
#define KEY2  PEin(2)
#define KEY1  PEin(3)
#define KEY0  PEin(4)

//#define LED3  PEout(4)
//#define LED4  PEout(5)


#define LED_BLUE_OFF GPIO_SetBits(GPIOE,GPIO_Pin_2)
#define LED_BLUE_ON GPIO_ResetBits(GPIOE,GPIO_Pin_2)

#define LED_RED_OFF GPIO_SetBits(GPIOE,GPIO_Pin_3)
#define LED_RED_ON GPIO_ResetBits(GPIOE,GPIO_Pin_3)

#define LED_GREEN_OFF GPIO_SetBits(GPIOE,GPIO_Pin_4)
#define LED_GREEN_ON GPIO_ResetBits(GPIOE,GPIO_Pin_4)

#define LED_YELLOW_OFF GPIO_SetBits(GPIOE,GPIO_Pin_5)
#define LED_YELLOW_ON GPIO_ResetBits(GPIOE,GPIO_Pin_5)

#define OBSTAC_DIS  200     //�ϰ������
extern void SendHeadDis(uint8_t ch) ; //�����沿����

 
extern OS_EVENT *g_COM3Sem;   //֪ͨ�����˽��뵽Σ������
extern OS_EVENT *g_COM4Mbox ; //֪ͨ�Ѿ���ȡ������
extern OS_EVENT *g_COM5Mbox;  //֪ͨ�޸Ĳ��ŵ����ֻ��ǵ���������
extern OS_EVENT *g_headDisMbox;

#define MINDOUBLE (10e-5)
#define MAXDOUBLE (10e8)

#define zhuohao  80     //ռ��8�ֽ�
#define fangxiang 40    //ռ��12�ֽ�
#define xianshu   20    //ռ��8�ֽ�
//#define duzhan    20    //ռ��12�ֽ�
#define  shebei   20
#define   jinqu     30    //��������
extern u16 jl[],jm[],jr[];
extern u8 cmd[], stat[][33];
extern u8 pccmd[];

//���ڽ��յ����ϲ�����  ///�������
typedef struct BaseDownStruct
{
	int Angle;		             	//
	int distx;		            	//
	int disty;	        	     	//
	char CmdTable;					//Ŀ�������
	char State;             //���ڷ���С������״̬
	//	char Cmd;						//���Ա��淢�͸�С�����������ͣ������ֶ����ƺ��Զ����Ƶȣ�
	char CmdBeating;				//���Ա������²����������Ŀǰû����
	u8 bjmp3;
	int   mp3;  					//���Ա���ͨ�ſ�������ͨ�����ݵ�CRCУ����
	s8 fx;
}BaseDownSt;
//���ڷ���С������ϵͳ״̬��200ms�̶��ϴ�
typedef struct BaseUpStruct
{
	s32     Fbposx;       //x����
	s32     Fbposy;       //y����
	s32     Fbposth;      //С����Ի���
	s32     ID;           //��ǰ�����Ŀ���
	s32     BUSY;         //CPU���м���
	s8      FbVx;						//���Ա��淴����С��x�ٶ�
	s8      FbVthe;					//���Ա��淴����С��theta�ٶȣ�
	u8      FbErr;          //���Ա��浼��С��ֹͣ�Ĵ���
	u8      FbStatus;       //���Ա���С��״̬+״̬��
	s8      DL;             //С������
	u8      MagIo;          //����λ�á�
	u8      Vcc;            //��ص�ѹ��
	u8      DZ;             //��ռ����
	u8      FbCRC;          //У��
	u8      needup;         //�ֶ�253 MP3  ���ϴ���254 MP3 ���ϴ���
}BaseUpSt;


//�����ڲ������õ������״̬����
typedef struct BaseComStruct
{
	s16 CmdVx;					        //���Ա�������ϲ������ȡ��x�����ٶȣ���λm/s
	s16 CmdVxgo;					      //���Ա�������ϲ������ȡ��x�����ٶȣ���λm/s
	s16 CmdVxgoxs;
	s16 CmdVthe;					      //���Ա�������ϲ������ȡ��theta�����ٶȣ���λrad/s
	s16 LastCmdVx;
	s16 LastCmdVthe;
	unsigned char State;        //������ʽѡ���ֶ�/�Զ�
	unsigned char CmdStation;		//���Ա���С����һ��Ŀ��վ��
	u8 qcd;
	double FbPosx;
	double FbPosy;
	s16 FbPosthe; 
	s8 back;                    //�Ͳ�����Ƿ񷵻�1�����أ�-1 ����ǰ����
	u8 magon;                   //�Ƿ���ҪѰ�� 1��Ҫ�� 0����Ҫ
}BaseComSt;

//�����ڲ�������ڲ�����
typedef struct BaseInnerStruct
{
	s8      ID ;                                    //�豸ID.
	s8      mkjs;
	s16     Length;				        //��ʾ�������ľ൥λm
	s16     WheelPeri;					//��ʾ�����ܳ���λm
	u8      GearScale;					//��ʾ��������ӵĳ��ֱ�32
	u16     MotorEncoder;
	s16     MagFIOX;						//���Ա�ʾǰ��������������Ӧ���Ĵ���λ��
	s16     MagDist;						//���Ա�ʾ����������IO�ڼ��
	s16     MagFBefore;                  //���Ա�ʾ��һ��ǰ��������������Ӧ���Ĵ���λ��
	s8      ScaleEStop;
	s16     MaxVx;
	unsigned char NeedUp;				//��ʾ�Ƿ���Ҫ�ϴ�����
  unsigned char FatalErr;             //���ڱ�ʶ�������󣬵��Ӧ����ֹͣ����ʹ�ܣ�
	unsigned char State;				//���ڱ�ʾ���ֶ�״̬�����Զ�״̬ ///С���ĵ�ǰ״̬��š�
	unsigned char OnMag;				//���ڱ�ʾ�Ƿ��⵽����
	unsigned char InPosition;			//���ڱ�ʾ�Ƿ񵽴�Ŀ���
	unsigned char FlagStation;			//���ڱ�ʾ�Ƿ���ҪѰ��վ��
	unsigned char FinishGive;
	unsigned char MpNum;
	u8 sbjq[jinqu];                            //30������֮������Щ�豸���ڡ����С�����ߣ�ÿ��Ҫ�ߵ��豸�Լ��Ƚ�
	u8 sbwz[shebei+1];                            //�豸λ�ã�ÿ̨�豸����λ�á�
	s8 fx;                                  //ǰ��ת�䡣
	int TurnFlag;
	s16 xianshuka;
}BaseInnerSt;

//�ĸ����ӵ���ز��� ==================�ײ�Ӳ������moto-go CalMotor MagIO_Check�����������м�����ֵ��
typedef struct WheelStruct
{
	double CmdVel;					                  //���Ա��������ٶȣ���λm/s
	int CmdMotorvel;				                  //���Ա�����ת�٣���λr/min
	int FlagForward;			                    //���Ա�ʾ���������
	s32  FbbPos;
	double FbPos;                              //ÿ������ת������Ծ��롣
	double LastFbpos;                          //����һֵ����С�������λ�ã���
	unsigned char PowerFlag;                   //CAN��ʼ��ʱ��״̬��¼��

       
}WheelSt;
//���Ա���ײ�����     ��ȡ����ID�� �ʹ���������ֵ
typedef struct BaseHalStruct
{
	vu32 Station; 			 //���Ա�ʾID����������ȡ��ID������  
	u8 forward;		       //���Ա�ʾǰ��8��IO״̬
  u8  num;             //�ŵ�����
	WheelSt WheelHal[2]; //���Ա���2�����ӵ��������
}BaseHalSt;

typedef struct ZuoHaoStruct
{
        u32 ID;         //
        s8  ZJD;        //�����Ƕȣ�����3.14Ϊ180�� ��λ0.01��
        u8  ZJL;        //������ţ�255 0��ȡ�Ϳڣ�
        s8  ZFH;        //����ת���ǰ������
}ZhuoHaoSt;
typedef struct FangXiangStruct
{
        u32 ID;
        u8  LL;
        u8  LH;
        u8  RL;
        u8  RH;
        u8  FL;
        u8  FH;
        u8  DZ;
//        u8  BH;
}FangXiangSt;
typedef struct XianShuStruct
{
	u32 ID;
	u8  UP;
	u8  DN;
}XianShuSt;

typedef struct BaseSysStruct
{
	BaseComSt BaseComData;			//���Ա�����Ҫ���²�ͨ�ŵĲ���
	BaseInnerSt BaseInnerData;		//���Ա����ڲ������õ����ò�������������
	BaseHalSt BaseHalData;			//���Ա���ӵײ㴫������ȡ�Ĳ���
	BaseDownSt BaseDownData;		//���Ա�������ͨ�Ż�ȡ������
	BaseUpSt BaseUpData;			//���Ա�������ͨ���ϴ�������

	ZhuoHaoSt   BaseZhuoHaoData[zhuohao];
	FangXiangSt BaseFangXiangData[fangxiang];
	XianShuSt   BaseXianShuData[xianshu];
	//        DuZhanSt    BaseDuZhanData[duzhan];

}BaseSysSt;

extern BaseSysSt* gSysData;
extern BaseComSt* gComData;
extern BaseInnerSt* gInnerData;
extern BaseHalSt* gHalData;
extern BaseDownSt* gDownData;
extern BaseUpSt* gUpData;

extern ZhuoHaoSt*   ZhuoHaoData;
extern FangXiangSt* FangXiangData;
extern XianShuSt*   XianShuData;
//extern DuZhanSt*    DuZhanData;
char COM3_SendHeadDis(char ch); 


extern void FCXS(void);
extern void MP3CRBF (u8 da);
extern void System_Init(void);
extern void Status_Handle(void);
extern void Command_Handle(void);


extern void jiema(void);
void RNG_init (void);
u32 SRNG (void);
extern u8 jv_js(void);

extern void peibao(void);
extern void dzbwh(void);

extern char COM3_SendCsb(char ch);
#endif


