#include "CAN_MAGS.h"
#include "can.h"
#include "string.h"
#include "stdlib.h"
#include "control.h"

#define MagDist 10              //磁条点位距离
QueueHandle_t xCANMAGS=NULL;
xCANMAGS_D CAR_MAGS={0,};

u16 ttime[256];
u8 i=0;

/****************************************************************************
* 名    称：void MagIO_Check(void)
* 功    能：
* 入口参数：MAGS ZYFX
* 出口参数： MAGDATA ONMAG
* 说    明：根据16个输入点确定磁条相对于磁条的位置
* 调用方法：无 
****************************************************************************/
void MagIO_Check(void)
{
  int num = 0;
  static u8 lasttime=0;
        s32 magdata;
//        CAR_MAGS.zyfx=CAR.xcxcfx;
        if(CAR_MAGS.zyfx==1)
        CAR_MAGS.mags&=0xff80;
        
        if(CAR_MAGS.zyfx==-1)
        CAR_MAGS.mags&=0x1ff;
	//根据前8个IO点计算前磁感应检测到的磁条位置
        
        
	magdata = 0;
	for(u8 temp = 0; temp < 16; temp++)
	{
	    if (CAR_MAGS.mags&(0X0001<<temp)) //低位左侧排列，正逻辑，正值。
		{magdata+= ((temp -7.5) * MagDist); num++;}
            
	}
	//检测判断前向磁导航IO是否检测到磁条，并计算前方向磁条位置

	if(0 != num) CAR_MAGS.magdata = magdata / num;//如果检测到，则根据磁条判断小车位置。

        else{if(CAR_MAGS.magdata > 0) CAR_MAGS.magdata = 80; else CAR_MAGS.magdata = -80;}//如果没有检测到，则根据之前检测到的位置，估算当前位置。
 	CAR_MAGS.OnMag=num;
        if(lasttime++>200)lasttime--;
        if(CAR_MAGS.lastmagdata!=CAR_MAGS.magdata) 
        {CAR_MAGS.lasttime=(u16)(160/abs(CAR_MAGS.lastmagdata-CAR_MAGS.magdata))*lasttime;//变化率求取
        CAR_MAGS.lastmagdata=CAR_MAGS.magdata;lasttime=0;
        ttime[i++]=CAR_MAGS.lasttime;}
        else {CAR_MAGS.lasttime=(u16)(160/5)*lasttime;}
}



void MAGS_task(void *pv)
{

  CanRxMsg RxMess;
xCANMAGS= xQueueCreate( 3, sizeof( CanRxMsg ) );


//CAN1_Mode_Init(1000); //是否单独配置can端口


while(1){
  if(!JudgeEncrypt())while(1);

if(xQueueReceive(xCANMAGS,&RxMess,50/portTICK_PERIOD_MS)==errQUEUE_EMPTY)
{
  CAR_MAGS.DRVERR=0xf0;//一百毫秒没接收的，就直接判错。
 }//读卡器错误


else
{
   if((CAR_MAGS.DRVERR++)>0xf0)  CAR_MAGS.DRVERR=0xf0;//干扰数据不合法，延时累加判错。
   if(RxMess.DLC==3)
  {if(RxMess.StdId==0x00B){//检测到有效磁传感器。
  CAR_MAGS.DRVERR=0;
  CAR_MAGS.mags=(u16)((*((u32*)RxMess.Data))>>1);
  MagIO_Check();}
  }


  }
}}