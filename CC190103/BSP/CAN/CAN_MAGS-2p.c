#include "CAN_MAGS.h"
#include "can.h"
#include "string.h"
#include "control.h"

#define MagDist 10              //������λ����
QueueHandle_t xCANMAGS=NULL;


xCANMAGS_D CAR_MAGS={0,};


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
  int num[2] = {0,};
        s32 magdata[2];
        CAR_MAGS.zyfx=CAR.xcxcfx;
        if(CAR_MAGS.zyfx==1)
        {CAR_MAGS.mags[0]&=0xf8;CAR_MAGS.mags[1]&=0xf8;}
        
        if(CAR_MAGS.zyfx==-1)
        {CAR_MAGS.mags[0]&=0x18;CAR_MAGS.mags[1]&=0x18;}
	//����ǰ8��IO�����ǰ�Ÿ�Ӧ��⵽�Ĵ���λ��
        
        
	magdata[0] = 0;
	magdata[1] = 0;
	for(u8 temp = 0; temp < 16; temp++)
	{
	    if (CAR_MAGS.mags[0]&(0X0001<<temp)) //��λ������У����߼�����ֵ��
		{magdata[0]+= ((temp -7.5) * MagDist); num[0]++;}
	    if (CAR_MAGS.mags[1]&(0X0001<<temp)) //��λ������У����߼�����ֵ��
		{magdata[1]+= ((temp -7.5) * MagDist); num[1]++;}
            
	}
	//����ж�ǰ��ŵ���IO�Ƿ��⵽������������ǰ�������λ��

	if(0 != num[0]) CAR_MAGS.magdata[0] = magdata[0] / num[0];//�����⵽������ݴ����ж�С��λ�á�

        else{if(CAR_MAGS.magdata[0] > 0) CAR_MAGS.magdata[0] = 80; else CAR_MAGS.magdata[0] = -80;}//���û�м�⵽�������֮ǰ��⵽��λ�ã����㵱ǰλ�á�
 	CAR_MAGS.OnMag[0]=num[0];

	if(0 != num[1]) CAR_MAGS.magdata[1] = magdata[1] / num[1];//�����⵽������ݴ����ж�С��λ�á�

        else{if(CAR_MAGS.magdata[1] > 0) CAR_MAGS.magdata[1] = 80; else CAR_MAGS.magdata[1] = -80;}//���û�м�⵽�������֮ǰ��⵽��λ�ã����㵱ǰλ�á�
 	CAR_MAGS.OnMag[1]=num[1];
         
}



void MAGS_task(void *pv)
{

  CanRxMsg RxMess;
xCANMAGS= xQueueCreate( 3, sizeof( CanRxMsg ) );


//CAN1_Mode_Init(1000); //�Ƿ񵥶�����can�˿�


while(1){
  if(!JudgeEncrypt())while(1);

if(xQueueReceive(xCANMAGS,&RxMess,100/portTICK_PERIOD_MS)==errQUEUE_EMPTY)
{
  CAR_MAGS.DRVERR[0]=0xf0;
  CAR_MAGS.DRVERR[1]=0xf0;
 }//����������


else
{
   if((CAR_MAGS.DRVERR[0]++)>0xf0)  CAR_MAGS.DRVERR[0]=0xf0;
   if((CAR_MAGS.DRVERR[1]++)>0xf0)  CAR_MAGS.DRVERR[1]=0xf0;
  if(RxMess.DLC==3)
  {if((RxMess.StdId==0x42)||(RxMess.StdId==0x43)){//��⵽��Ч�Ŵ�������
  CAR_MAGS.DRVERR[RxMess.StdId-0x42]=0;
  CAR_MAGS.mags[RxMess.StdId-0x42]=(u16)((*((u32*)RxMess.Data))>>1);
  MagIO_Check();}
  }


  }
}}