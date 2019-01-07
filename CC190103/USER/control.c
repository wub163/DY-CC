#include "FreeRTOS.h"
#include "control.h"
#include "yaokong.h"
#include "task.h"
#include "queue.h"
#include "string.h"
#include "stdlib.h"
#include "math.h"
#include "upload.h"
#include "bsp_lcd.h"
#include "gpio.h" 
#include "../SPI_DAC/spi_dac.h"
#include "../adc/adc.h"
#include "../jiami/jm.h"
#include "CAN_MAGS.h"
#include "CAN_ID.h"
#include "CCST.h"
//const s16 speed[]={-750,750,700,650,600,550,500,450,0,300,400,500,600,700,800,900,1000};
//ǰ���ͺ��˵��ٶ�ֵ��-8-0-+8 ƫ��8 �ٶȲ�����
xSYS_D SYSST={0,};
  s8 mags[11]={0,};
  s16 mag=0;
  u8 idcount=0;//����ָ�롣




//void  diaodu(void)
//{
////static u8 idcount=0;//����ָ�롣
//static s8  maxspeed=0;//��ǰ��������ٶȡ�
//static u8 zxztj=0;//ת��״̬����
//static s16 zxcount=0;//ת�����
//static s8 dlcount=0;//ת�����
//static s32 zxlen=0;//ת��Ƕȼ���
//      s16 speed=0,turn=0;//���Ʊ�����
//if(  xSentData.funcount!=xComData.funcount) //�������з���
//{idcount=0;xSentData.funcount=xComData.funcount;}     //����ָ������
////����ָ���ȡ��  
//  if(idcount>3){SYSST.dd_com=0x00;SYSST.dd_val=0;}     //�����������ִ����ϣ���ԭ�صȴ���
//  else{ if(0==xComData.id[idcount].id){                        //�ж���������
//            SYSST.dd_com=xComData.id[idcount].com;
//            SYSST.dd_val=xComData.id[idcount].val;
//            xComData.id[idcount].id=0xffffffff;            //����ʼִ�б�ǡ�
////            idcount++;                      //������ִ����ɱ�ǡ�
//           }
//  if(xSentData.rfid){//�ȶ��Ƿ�������
//  for(int i = 0; i < 4; ++i)
//        {
//
//          if(xSentData.rfid==xComData.id[i].id){
//            SYSST.dd_com=xComData.id[i].com;
//            SYSST.dd_val=xComData.id[i].val;
//            xComData.id[i].id=0xffffffff;            //����ʼִ�б�ǡ���֤ÿ�ſ���ִֻ��һ�Ρ�
//            idcount=i; if(i<3)xSentData.rfidnext=xComData.id[i+1].id;xSentData.rfidnext=0xffffffff;
//          }      
//       }
//        }
//  }
//   xSentData.funid=idcount;          //����ָ�뷴��
// //����ָ�����С�
//    static u16 yanshi=0;
//    static u8 yanshizhong=0;
//    static u8 zhixingxunchi=0;
//    static s16 jd=0,zx=0;
//    
//   if(SYSST.dd_com!=0xff)yanshizhong=0;                
//
//   switch (SYSST.dd_com)                          //����ִ���У�Ϊoxff��ִ���꣬gDiaoDuData->ComID_id++
//  {
//  case 0:                               //ͣ����ʱ����    ����ʱ  //�ط�ָ������Ƴ�ͣ����ʱ
//    if (SYSST.dd_val==0)yanshi=0xffff;else yanshi =SYSST.dd_val*50;
//    SYSST.dd_com=0xff;yanshizhong=1;zhixingxunchi=0;maxspeed=0;
//    break;
//    
//  case 1:                               //ǰ��Ѱ��
//    zx=0;zhixingxunchi=1;yanshizhong=0;maxspeed=SYSST.dd_val;
//    idcount++; SYSST.dd_com=0xff;       //����ָ�ָ����һ��ָ��ִ���С�
//    break;
//    
//  case 2:                               //���ȷ�ʽ��ֹ����Ѱ�š�
//     zx=0;zhixingxunchi=1;yanshizhong=0;maxspeed=0;
//    idcount++; SYSST.dd_com=0xff;
//    break;
//
//
//  case 3://zx��ʾת��ʼ��ת�������jd��ʾ��Ҫת���ĽǶȣ�0Ϊת�������ϡ�
//    zx=1;zxztj=0;jd=SYSST.dd_val;
//    if(SYSST.dd_val)  {maxspeed=0;zxztj=1;}   //ֹͣ��ת��
//    else {if (SYSST.con_speed==0)
//              {maxspeed=0;zxztj=20;} //ת�����
//              else{zxztj=40;}  //Ѱ��ת��
//          }SYSST.dd_com=0xff;
//     break;
//   
//
//  case 4://zx��ʾת��ʼ��ת�������jd��ʾ��Ҫת���ĽǶȣ�0Ϊת�������ϡ�
//    zx=-1;zxztj=0;jd=SYSST.dd_val;
//    if(SYSST.dd_val)  {maxspeed=0;zxztj=1;}   //ֹͣ��ת��
//    else {if (SYSST.con_speed==0)
//              {maxspeed=0;zxztj=20;} //ת�����
//              else{zxztj=40;}  //Ѱ��ת��
//          }SYSST.dd_com=0xff;
//    break;
//  case 5:
//     break;
//  case 6:
//    break;
//  case 8:                                       //�������뷶Χ��1-10
//    zhixingxunchi=0;yanshizhong=0;zxztj=0;
//    if(abs(SYSST.st_turn)>5){speed=0;turn=0;zxlen=SYSST.st_meter;break;} 
//    speed=lrspeed;turn=0;
//    if((abs(zxlen-SYSST.st_meter)>(SYSST.dd_val*10))||(CAR_MAGS.OnMag>0)){speed=0;turn=0;idcount++; SYSST.dd_com=0xff;}
//    break;
//  case 9:                                       //�������뷶Χ��1-10
//    zhixingxunchi=0;yanshizhong=0;zxztj=0;
//    if(abs(SYSST.st_turn)>5){speed=0;turn=0;zxlen=SYSST.st_meter;break;} 
//    speed=-lrspeed;turn=0;
//    if((abs(zxlen-SYSST.st_meter)>(SYSST.dd_val*10))||(!ZBOK)){speed=0;turn=0;if(SYSST.st_speed==0){idcount++; SYSST.dd_com=0xff;}}
//    break;
//  case 0x0a:                                       //���������
//    SYSST.con_highset=SYSST.dd_val*10;if((SYSST.con_highwork==0)&&(abs(SYSST.st_high-SYSST.con_highset)<10))
//    {idcount++; SYSST.dd_com=0xff;}
//    break;
//  default:if(SYSST.dd_com!=0xff)if(idcount++>4)idcount=4; SYSST.dd_com=0xff;//0xff��ʾ��ǰָ��δִ���ꡣ��Чָ����������
//  }
//
//    
//  if(zhixingxunchi) {speed =maxspeed; turn = xc();if(zxcount--<0){zxcount=0;CAR_MAGS.zyfx=0;}}
//  
//  //��ʱ����
//  if(yanshizhong){ if(yanshi){
////            if(0 == gInnerData->FinishGive && 1 == lastfinishgive)          ///��ɼ����²��ͷ�ʱ���롣
////              idcount++; lastfinishgive = gInnerData->FinishGive;
//    yanshi--;speed = 0;turn=xc();}
//  else {idcount++;yanshizhong=0;}}else yanshi=0;
//  
////ת�����  
////jd,zx,
//  if(zxztj) {
//    switch (zxztj)
//  {
//  case 1://ת��ǰͣ��                ͣ��ת��
//    if (fabs(SYSST.st_speed) > 50){speed =maxspeed; turn = xc();}  
//    else  {zxztj=2;dlcount=10;}
//    break;
//  case 2://����ת���̶���
//    if(zx>0)turn=lturn; if(zx<0)turn=rturn;if(dlcount--==0){dlcount=0;
//    if(abs(SYSST.st_turn-SYSST.const_turn)<10)
//    {zxlen=SYSST.st_meter;zxztj=3;}}//����Ʒ�����ʵ�ʷ����ӽ�����ʾ������ת��λ��
//    break;
//  case 3://ת���̶���λ
//    if(zx>0){turn=lturn;speed=-lrspeed; 
//    if(abs(zxlen-SYSST.st_meter)>(lcount*jd/180+10)){speed=0;turn=0;zxztj=4;}}
//    if(zx<0){turn=rturn;speed=-lrspeed;
//    if(abs(zxlen-SYSST.st_meter)>(rcount*jd/180+10)){speed=0;turn=0;zxztj=4;}}
//    break;
//  case 4:                              //ת�����
//    idcount++; SYSST.dd_com=0xff;zxztj=0;
//    break;
//  case 20:                              //ת�����
//    if(zx>0)turn=lturn; if(zx<0)turn=rturn;
//    if(abs(SYSST.st_turn-SYSST.const_turn)<2){zxlen=SYSST.st_meter;zxztj=21;}//����Ʒ�����ʵ�ʷ����ӽ�����ʾ������ת��λ��
//    break;
//  case 21:
//    if(zx>0){turn=lturn;speed=lrspeed; }if(zx<0){turn=rturn;speed=lrspeed;}
//    if(CAR_MAGS.OnMag>0){speed=0;turn=0;zxztj=4;}//�ҵ�������
//    break;
//  case 40:                              //Ѱ��ת��
//    zhixingxunchi=1;CAR_MAGS.zyfx=zx;zxcount=200;zxztj=4; //Ѱ��ת���ӳ�4�롣
//    break;
//  }}
// SYSST.con_speed=-speed;
//  SYSST.con_turn=turn;
//
//}  
//  





void control(void* pa)
{  
  TickType_t xLastWakeTime;
 xLastWakeTime = xTaskGetTickCount ();
 while(1)
{
 vTaskDelayUntil( &xLastWakeTime, 20/portTICK_PERIOD_MS);


//uxTaskGetStackHighWaterMark( NULL );

}

}

//highinit:0xA0  �����������̡�0xA1���½�������㣬0xA2������������㡣0xAA����ʼ����ɡ�0xAF ��ʼ��ʧ�ܡ�
void high_init(void)
{
  static u8 count=0;
  static s32 temp=0;//�߶�ֵ�ݴ档
  if((SYSST.highinit&0xf8)==0xa0){//�߶��Զ�����λ��
    SYSST.con_speed=0;SYSST.con_turn=0;
    if((count++)==10)
      temp=SYSST.st_high;
    if(count==60){if(abs(temp-SYSST.st_high)<10)
      SYSST.highinit=0XAF;count=0;}//һ����λ�Ƶ���10mm���򱨴�
   switch(SYSST.highinit)
  {
  case 0xA0:
    if(ZBAQ){SYSST.con_highsp=-5;SYSST.highinit=0XA1;}
    else  {SYSST.con_highsp=5;SYSST.highinit=0XA2;}
    break;
  case 0xa1:
    if(!ZBAQ){
      SYSST.highinit=0XA0;count=0;}
    break;
  case 0xa2:
    if(ZBAQ)
    {highset(highbase);SYSST.highinit=0XAA;SYSST.con_highsp=0;SYSST.st_mod=1;}
    break;
  case 0xaa:
    
    break;
  default:
    break;
     
  
  
  }
  }else count=0;

}

