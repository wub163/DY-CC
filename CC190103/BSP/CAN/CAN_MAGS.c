#include "CAN_MAGS.h"
#include "can.h"
#include "string.h"
#include "stdlib.h"
#include "control.h"

#define MagDist 10              //������λ����
QueueHandle_t xCANMAGS=NULL;
xCANMAGS_D CAR_MAGS={0,};

u16 ttime[256];
u8 i=0;

/****************************************************************************
* ��    �ƣ�void MagIO_Check(void)
* ��    �ܣ�
* ��ڲ�����MAGS ZYFX
* ���ڲ����� MAGDATA ONMAG
* ˵    ��������16�������ȷ����������ڴ�����λ��
* ���÷������� 
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
	//����ǰ8��IO�����ǰ�Ÿ�Ӧ��⵽�Ĵ���λ��
        
        
	magdata = 0;
	for(u8 temp = 0; temp < 16; temp++)
	{
	    if (CAR_MAGS.mags&(0X0001<<temp)) //��λ������У����߼�����ֵ��
		{magdata+= ((temp -7.5) * MagDist); num++;}
            
	}
	//����ж�ǰ��ŵ���IO�Ƿ��⵽������������ǰ�������λ��

	if(0 != num) CAR_MAGS.magdata = magdata / num;//�����⵽������ݴ����ж�С��λ�á�

        else{if(CAR_MAGS.magdata > 0) CAR_MAGS.magdata = 80; else CAR_MAGS.magdata = -80;}//���û�м�⵽�������֮ǰ��⵽��λ�ã����㵱ǰλ�á�
 	CAR_MAGS.OnMag=num;
        if(lasttime++>200)lasttime--;
        if(CAR_MAGS.lastmagdata!=CAR_MAGS.magdata) 
        {CAR_MAGS.lasttime=(u16)(160/abs(CAR_MAGS.lastmagdata-CAR_MAGS.magdata))*lasttime;//�仯����ȡ
        CAR_MAGS.lastmagdata=CAR_MAGS.magdata;lasttime=0;
        ttime[i++]=CAR_MAGS.lasttime;}
        else {CAR_MAGS.lasttime=(u16)(160/5)*lasttime;}
}



void MAGS_task(void *pv)
{

  CanRxMsg RxMess;
xCANMAGS= xQueueCreate( 3, sizeof( CanRxMsg ) );


//CAN1_Mode_Init(1000); //�Ƿ񵥶�����can�˿�


while(1){
  if(!JudgeEncrypt())while(1);

if(xQueueReceive(xCANMAGS,&RxMess,50/portTICK_PERIOD_MS)==errQUEUE_EMPTY)
{
  CAR_MAGS.DRVERR=0xf0;//һ�ٺ���û���յģ���ֱ���д�
 }//����������


else
{
   if((CAR_MAGS.DRVERR++)>0xf0)  CAR_MAGS.DRVERR=0xf0;//�������ݲ��Ϸ�����ʱ�ۼ��д�
   if(RxMess.DLC==3)
  {if(RxMess.StdId==0x00B){//��⵽��Ч�Ŵ�������
  CAR_MAGS.DRVERR=0;
  CAR_MAGS.mags=(u16)((*((u32*)RxMess.Data))>>1);
  MagIO_Check();}
  }


  }
}}