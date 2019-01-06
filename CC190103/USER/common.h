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

#define OBSTAC_DIS  200     //障碍物距离
extern void SendHeadDis(uint8_t ch) ; //发送面部表情

 
extern OS_EVENT *g_COM3Sem;   //通知机器人进入到危险区域
extern OS_EVENT *g_COM4Mbox ; //通知已经获取到卡号
extern OS_EVENT *g_COM5Mbox;  //通知修改播放的音乐或是调整音量等
extern OS_EVENT *g_headDisMbox;

#define MINDOUBLE (10e-5)
#define MAXDOUBLE (10e8)

#define zhuohao  80     //占据8字节
#define fangxiang 40    //占据12字节
#define xianshu   20    //占据8字节
//#define duzhan    20    //占据12字节
#define  shebei   20
#define   jinqu     30    //禁区数量
extern u16 jl[],jm[],jr[];
extern u8 cmd[], stat[][33];
extern u8 pccmd[];

//用于接收到的上层命令  ///解包数据
typedef struct BaseDownStruct
{
	int Angle;		             	//
	int distx;		            	//
	int disty;	        	     	//
	char CmdTable;					//目标餐桌号
	char State;             //用于发送小车运行状态
	//	char Cmd;						//用以保存发送给小车的命令类型，比如手动控制和自动控制等；
	char CmdBeating;				//用以保存上下层控制心跳，目前没用上
	u8 bjmp3;
	int   mp3;  					//用以保存通信控制命令通信内容的CRC校验码
	s8 fx;
}BaseDownSt;
//用于反馈小车控制系统状态，200ms固定上传
typedef struct BaseUpStruct
{
	s32     Fbposx;       //x坐标
	s32     Fbposy;       //y坐标
	s32     Fbposth;      //小车相对弧度
	s32     ID;           //当前读到的卡号
	s32     BUSY;         //CPU空闲计数
	s8      FbVx;						//用以保存反馈的小车x速度
	s8      FbVthe;					//用以保存反馈的小车theta速度；
	u8      FbErr;          //用以保存导致小车停止的错误
	u8      FbStatus;       //用以保存小车状态+状态机
	s8      DL;             //小车电流
	u8      MagIo;          //磁条位置。
	u8      Vcc;            //电池电压。
	u8      DZ;             //独占区。
	u8      FbCRC;          //校验
	u8      needup;         //手动253 MP3  开上传，254 MP3 关上传。
}BaseUpSt;


//用于内部计算用的命令和状态反馈
typedef struct BaseComStruct
{
	s16 CmdVx;					        //用以保存根据上层命令获取的x方向速度，单位m/s
	s16 CmdVxgo;					      //用以保存根据上层命令获取的x方向速度，单位m/s
	s16 CmdVxgoxs;
	s16 CmdVthe;					      //用以保存根据上层命令获取的theta方向速度，单位rad/s
	s16 LastCmdVx;
	s16 LastCmdVthe;
	unsigned char State;        //操作方式选择，手动/自动
	unsigned char CmdStation;		//用以保存小车下一个目标站点
	u8 qcd;
	double FbPosx;
	double FbPosy;
	s16 FbPosthe; 
	s8 back;                    //送餐完毕是否返回1：返回，-1 继续前进。
	u8 magon;                   //是否需要寻磁 1：要。 0：不要
}BaseComSt;

//用于内部计算的内部参数
typedef struct BaseInnerStruct
{
	s8      ID ;                                    //设备ID.
	s8      mkjs;
	s16     Length;				        //表示轮组中心距单位m
	s16     WheelPeri;					//表示轮子周长单位m
	u8      GearScale;					//表示电机到轮子的齿轮比32
	u16     MotorEncoder;
	s16     MagFIOX;						//用以表示前方磁条传感器感应到的磁条位置
	s16     MagDist;						//用以表示磁条传感器IO口间距
	s16     MagFBefore;                  //用以表示上一次前方磁条传感器感应到的磁条位置
	s8      ScaleEStop;
	s16     MaxVx;
	unsigned char NeedUp;				//表示是否需要上传数据
  unsigned char FatalErr;             //用于标识致命错误，电机应立即停止并下使能；
	unsigned char State;				//用于表示是手动状态或者自动状态 ///小车的当前状态编号。
	unsigned char OnMag;				//用于表示是否检测到磁条
	unsigned char InPosition;			//用于表示是否到达目标点
	unsigned char FlagStation;			//用于表示是否需要寻找站点
	unsigned char FinishGive;
	unsigned char MpNum;
	u8 sbjq[jinqu];                            //30个禁区之中有哪些设备在内。编号小的先走，每个要走的设备自己比较
	u8 sbwz[shebei+1];                            //设备位置，每台设备所处位置。
	s8 fx;                                  //前方转弯。
	int TurnFlag;
	s16 xianshuka;
}BaseInnerSt;

//四个轮子的相关参数 ==================底层硬件操作moto-go CalMotor MagIO_Check三个函数的中间运算值。
typedef struct WheelStruct
{
	double CmdVel;					                  //用以保存轮子速度，单位m/s
	int CmdMotorvel;				                  //用以保存电机转速，单位r/min
	int FlagForward;			                    //用以表示电机正方向；
	s32  FbbPos;
	double FbPos;                              //每个轮子转过的相对距离。
	double LastFbpos;                          //与上一值计算小车的相对位置，。
	unsigned char PowerFlag;                   //CAN初始化时的状态记录。

       
}WheelSt;
//用以保存底层数据     读取到的ID号 和磁条传感器值
typedef struct BaseHalStruct
{
	vu32 Station; 			 //用以表示ID卡读卡器读取的ID卡编码  
	u8 forward;		       //用以表示前方8个IO状态
  u8  num;             //磁点数量
	WheelSt WheelHal[2]; //用以保存2个轮子的相关数据
}BaseHalSt;

typedef struct ZuoHaoStruct
{
        u32 ID;         //
        s8  ZJD;        //餐桌角度（弧度3.14为180度 单位0.01）
        u8  ZJL;        //餐桌编号（255 0：取餐口）
        s8  ZFH;        //餐桌转向后前进距离
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
	BaseComSt BaseComData;			//用以保存需要上下层通信的参数
	BaseInnerSt BaseInnerData;		//用以保存内部计算用的配置参数和其他参数
	BaseHalSt BaseHalData;			//用以保存从底层传感器获取的参数
	BaseDownSt BaseDownData;		//用以保存蓝牙通信获取的数据
	BaseUpSt BaseUpData;			//用以保存蓝牙通信上传的数据

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


