#ifndef __MOTO_H
#define __MOTO_H

#include "sys.h"

#define MOTO01  PAout(2) //1，关机         0，开机
#define MOTO02  PAout(3) //1,升            0，降

#define MOTO11  PCout(4) //1，关机         0，开机
#define MOTO12  PCout(5) //1,升            0，降

#define MOTO21  PDout(14) //1，关机         0，开机
#define MOTO22  PDout(15) //1,升            0，降

#define MOTO31  PBout(1) //1，关机         0，开机
#define MOTO32  PEout(7) //1,升            0，降

#define MOTO0D          {MOTO01=0;MOTO02=1;}//正转
#define MOTO0U          {MOTO01=1;MOTO02=0;}//反转
#define MOTO0B          {MOTO01=0;MOTO02=0;}//刹车
#define MOTO0OFF        {MOTO01=1;MOTO02=1;}//关机

#define MOTO1D          {MOTO11=0;MOTO12=1;}//正转
#define MOTO1U          {MOTO11=1;MOTO12=0;}//反转
#define MOTO1B          {MOTO11=0;MOTO12=0;}//刹车
#define MOTO1OFF        {MOTO11=1;MOTO12=1;}//关机

#define MOTO2D          {MOTO21=0;MOTO22=1;}//正转
#define MOTO2U          {MOTO21=1;MOTO22=0;}//反转
#define MOTO2B          {MOTO21=0;MOTO22=0;}//刹车
#define MOTO2OFF        {MOTO21=1;MOTO22=1;}//关机

#define MOTO3D          {MOTO31=0;MOTO32=1;}//正转
#define MOTO3U          {MOTO31=1;MOTO32=0;}//反转
#define MOTO3B          {MOTO31=0;MOTO32=0;}//刹车
#define MOTO3OFF        {MOTO31=1;MOTO32=1;}//关机

//步进电机控制线
//#define MOTOTO      PDout(14)
//#define MOTOOFF     PDout(15)
#define HTHIGH        PDin(13)
#define HTLOW         PDin(12)
#define YTLOW         PAin(4)

#define GearScale               27       //减速比
#define MotorEncoder            1000     //圈脉冲
#define WheelPeri               95      //轮直径径
#define MotoLength              300     //驱动轮距

s32 setspeed(s16 speed);
typedef struct _MOTO_ST{
        s16 comspeed;                        //电机给定速度 +-100
        s16 comhz;                        //实际给定频率   +-200    
        s16 lasthz;                       //上次给定频率
        s16 cuspe;                       //电机当前速度,(也为两次调用的位置差。)
        s16 lastcuspe;
        u8  err;                        //<251失调时间，254电机堵转
        u8  dzstop;
        s32 pos;                         //当前电机位置( 脉冲数)
        s32 lastpos;                    //历史电机位置（脉冲数） 计算速度用。
        s32 highpos;                    //电机100%高位值。
        s32 conpos;
    }xMOTO_D;


extern xMOTO_D moto[];//四台电机的速度给定值。+-100；0,1左右轮 2，升降台，3，云台。
void mototask(void* pv);


#endif 

