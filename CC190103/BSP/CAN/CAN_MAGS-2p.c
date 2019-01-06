#include "CAN_MAGS.h"
#include "can.h"
#include "string.h"
#include "control.h"

#define MagDist 10              //磁条点位距离
QueueHandle_t xCANMAGS=NULL;


xCANMAGS_D CAR_MAGS={0,};


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
  int num[2] = {0,};
        s32 magdata[2];
        CAR_MAGS.zyfx=CAR.xcxcfx;
        if(CAR_MAGS.zyfx==1)
        {CAR_MAGS.mags[0]&=0xf8;CAR_MAGS.mags[1]&=0xf8;}
        
        if(CAR_MAGS.zyfx==-1)
        {CAR_MAGS.mags[0]&=0x18;CAR_MAGS.mags[1]&=0x18;}
	//根据前8个IO点计算前磁感应检测到的磁条位置
        
        
	magdata[0] = 0;
	magdata[1] = 0;
	for(u8 temp = 0; temp < 16; temp++)
	{
	    if (CAR_MAGS.mags[0]&(0X0001<<temp)) //低位左侧排列，正逻辑，正值。
		{magdata[0]+= ((temp -7.5) * MagDist); num[0]++;}
	    if (CAR_MAGS.mags[1]&(0X0001<<temp)) //低位左侧排列，正逻辑，正值。
		{magdata[1]+= ((temp -7.5) * MagDist); num[1]++;}
            
	}
	//检测判断前向磁导航IO是否检测到磁条，并计算前方向磁条位置

	if(0 != num[0]) CAR_MAGS.magdata[0] = magdata[0] / num[0];//如果检测到，则根据磁条判断小车位置。

        else{if(CAR_MAGS.magdata[0] > 0) CAR_MAGS.magdata[0] = 80; else CAR_MAGS.magdata[0] = -80;}//如果没有检测到，则根据之前检测到的位置，估算当前位置。
 	CAR_MAGS.OnMag[0]=num[0];

	if(0 != num[1]) CAR_MAGS.magdata[1] = magdata[1] / num[1];//如果检测到，则根据磁条判断小车位置。

        else{if(CAR_MAGS.magdata[1] > 0) CAR_MAGS.magdata[1] = 80; else CAR_MAGS.magdata[1] = -80;}//如果没有检测到，则根据之前检测到的位置，估算当前位置。
 	CAR_MAGS.OnMag[1]=num[1];
         
}



void MAGS_task(void *pv)
{

  CanRxMsg RxMess;
xCANMAGS= xQueueCreate( 3, sizeof( CanRxMsg ) );


//CAN1_Mode_Init(1000); //是否单独配置can端口


while(1){
  if(!JudgeEncrypt())while(1);

if(xQueueReceive(xCANMAGS,&RxMess,100/portTICK_PERIOD_MS)==errQUEUE_EMPTY)
{
  CAR_MAGS.DRVERR[0]=0xf0;
  CAR_MAGS.DRVERR[1]=0xf0;
 }//读卡器错误


else
{
   if((CAR_MAGS.DRVERR[0]++)>0xf0)  CAR_MAGS.DRVERR[0]=0xf0;
   if((CAR_MAGS.DRVERR[1]++)>0xf0)  CAR_MAGS.DRVERR[1]=0xf0;
  if(RxMess.DLC==3)
  {if((RxMess.StdId==0x42)||(RxMess.StdId==0x43)){//检测到有效磁传感器。
  CAR_MAGS.DRVERR[RxMess.StdId-0x42]=0;
  CAR_MAGS.mags[RxMess.StdId-0x42]=(u16)((*((u32*)RxMess.Data))>>1);
  MagIO_Check();}
  }


  }
}}