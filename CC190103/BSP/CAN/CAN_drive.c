/*========================================================= 
*    �ļ���:  	        CAN_drive.c 
*    ����: 	        ����CAN�����������
*    �汾��:  	        1.1   
*    ��������:          2015-6-1 8:52:03  
*    IDE����:  	        IAR Embedded Workbench for ARM version 7.30.3
*    �ַ�����:  	GB2312   
*    ��	��:  	        ���  
*    ��Ȩ����:  	���������������������޹�˾
*    �汾�޸���ʷ:    ��XXX:Ϊ�����ߣ�
*       2015-6-1        ���:�����ļ�   
*  	2016-3-20       ���:��������   
* ===========================================================  */     
#include "CAN_drive.h"
CanDriveSt candr;
#define GearScale 1     //������
#define WheelPeri 125   //��ֱ��
#define MotorEncoder 900 //��������
#define Len          500
#define MOTORNUM 2

#include "net.h"


extern u8 QDQERR;

//CAN��ʼ��
//tsjw:����ͬ����Ծʱ�䵥Ԫ. @ref CAN_synchronisation_jump_width   ��Χ: ; CAN_SJW_1tq~ CAN_SJW_4tq
//tbs2:ʱ���2��ʱ�䵥Ԫ.   @ref CAN_time_quantum_in_bit_segment_2 ��Χ:CAN_BS2_1tq~CAN_BS2_8tq;
//tbs1:ʱ���1��ʱ�䵥Ԫ.   @refCAN_time_quantum_in_bit_segment_1  ��Χ: ;	  CAN_BS1_1tq ~CAN_BS1_16tq
//brp :�����ʷ�Ƶ��.��Χ:1~1024;(ʵ��Ҫ��1,Ҳ����1~1024) tq=(brp)*tpclk1
//������=Fpclk1/((tsjw+tbs1+tbs2+3)*brp);
//mode: @ref CAN_operating_mode ��Χ��CAN_Mode_Normal,��ͨģʽ;CAN_Mode_LoopBack,�ػ�ģʽ;
//����ֵ:0,��ʼ��OK;
//    ����,��ʼ��ʧ��;
unsigned char gMag = 0;
u8 CAN1_Mode_Init(u16 BAUD)
{

  	GPIO_InitTypeDef GPIO_InitStructure; 
	  CAN_InitTypeDef        CAN_InitStructure;
  	CAN_FilterInitTypeDef  CAN_FilterInitStructure;

    //ʹ�����ʱ��
	  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE);                   											 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);	
	
    //��ʼ��GPIO
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8| GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;           //���ù���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;         //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;     //100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;           //����
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	  //���Ÿ���ӳ������
	  GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_CAN1); //GPIOB8����ΪCAN1
	  GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_CAN1); //GPIOB9����ΪCAN1
	
	  CAN_DeInit(CAN1);
  	//CAN��Ԫ����
	CAN_InitStructure.CAN_TTCM = DISABLE;	//��ʱ�䴥��ͨ��ģʽ   
  	CAN_InitStructure.CAN_ABOM = DISABLE;	//����Զ����߹���	  
  	CAN_InitStructure.CAN_AWUM = DISABLE;//˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
  	CAN_InitStructure.CAN_NART = DISABLE;//ENABLE;	//��ֹ�����Զ����� 
  	CAN_InitStructure.CAN_RFLM = DISABLE;	//���Ĳ�����,�µĸ��Ǿɵ�  
  	CAN_InitStructure.CAN_TXFP = DISABLE;	//���ȼ��ɱ��ı�ʶ������ 
  	CAN_InitStructure.CAN_Mode= CAN_Mode_Normal;	 //ģʽ���� 
//  	CAN_InitStructure.CAN_Mode= CAN_Mode_LoopBack;	 //ģʽ���� 
	CAN_InitStructure.CAN_SJW=CAN_SJW_2tq;		   //BTR-SJW ����ͬ����Ծ��� 2��ʱ�䵥Ԫ
	 
	/* ss=1 bs1=5 bs2=3 λʱ����Ϊ(1+5+3) �����ʼ�Ϊʱ������tq*(1+3+5)  */
	CAN_InitStructure.CAN_BS1=CAN_BS1_5tq;		   //BTR-TS1 ʱ���1 ռ����5��ʱ�䵥Ԫ
	CAN_InitStructure.CAN_BS2=CAN_BS2_3tq;		   //BTR-TS1 ʱ���2 ռ����3��ʱ�䵥Ԫ	
	
	/* CAN Baudrate = 1 MBps (1MBps��Ϊstm32��CAN�������) (CAN ʱ��Ƶ��Ϊ APB 1 = 45 MHz) */     
	CAN_InitStructure.CAN_Prescaler =5000/BAUD;		  //BTR-BRP �����ʷ�Ƶ��  ������ʱ�䵥Ԫ��ʱ�䳤�� 45/(1+5+3)/5=1 Mbps
	CAN_Init(CAN1, &CAN_InitStructure);
    
		//���ù�����
	  CAN_FilterInitStructure.CAN_FilterNumber = 0;
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32λ 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32λID
  	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32λMASK
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
  	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE; //���������1
  	CAN_FilterInit(&CAN_FilterInitStructure);//�˲�����ʼ��
	  CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);
	  return 0;
}   



/****************************************************************************
* ��    �ƣ�void USB_LP_CAN1_RX0_IRQHandler(void)
* ��    �ܣ��������жϷ������
* ��ڲ�����null
* ���ڲ�����null
* ˵    ����
* ���÷�����null 
****************************************************************************/
void CAN1_RX0_IRQHandler(void)
{
  static u8 dl[2];
    CanRxMsg RxMessage;
	RxMessage.StdId = 0;
	//CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
	switch(RxMessage.StdId&0x0f)
	{
        
        
        case 0:             //CAN������
                ;
        case 1:   ;          //CAN������
//             QDQERR=0;
               if(candr.jczt==2)      //�жϵ�ǰΪʲô������
               switch (RxMessage.Data[0])
                {
                case 1:
                   candr.FbbPos[0]=(s32)(RxMessage.Data[4]<<24|RxMessage.Data[5]<<16|RxMessage.Data[6]<<8|RxMessage.Data[7]);break;
               case 2:
                   candr.FbbPos[0]=(s32)(RxMessage.Data[4]<<24|RxMessage.Data[5]<<16|RxMessage.Data[6]<<8|RxMessage.Data[7]);candr.jczt=1;break;
                
                }   
               else     {
               candr.dy=(RxMessage.Data[2]<<8|RxMessage.Data[3]);//����ѹģ�鲻����ʱ�����������ж�ȡ��ѹֵ��
               switch (RxMessage.Data[0])
                {
                case 1:
                   dl[0]=RxMessage.Data[7];break;
                case 2:
                   dl[1]=RxMessage.Data[7];candr.dl=(float)(dl[0]+dl[1])/10;candr.jczt=2;break;
                
                }        }
               candr.CANERR=(RxMessage.Data[1]&0x0f);break;
               
                
              
	}
}




/****************************************************************************
* ��    �ƣ�void CalMotor_can(void)
* ��    �ܣ���������������
* ��ڲ�����null
* ���ڲ�����null
* ˵    ����null
* ���÷�����null 
****************************************************************************/
void CalMotor_can(void)              /// 2015-04-22��Ϊmm/s
{
    int num = 0;
	int errcount = 0;
	int errsum = 0;
	static char firstflag[MOTORNUM] = {0,};
	static int errnum = 0;
	static char errflag[50] = {0,};
	double actpos[MOTORNUM] = {0,};
//	double basex = 0.0;
//	double basey = 0.0;
	double baseth = 0.0;
	double length = 0.0;
	static double radcount=0.0;


		 errnum = (errnum+1)%50;                                                         ///���50���ŷ�״̬��¼
          if((candr.CANERR+candr.CANERR)>0)            
		 {
                      errflag[errnum] = 1;
		 }
		 else
		 {
                      errflag[errnum] = 0;
		 }
		 for(errcount=0;errcount<50;errcount++)
		 {
		     errsum += errflag[errcount];
		 }
		 if(errsum > 45)        ///���1s����45�ε�������򱣻�
		 
		     candr.FatalErr |= 0x01;
		 else candr.FatalErr = 0;
                 candr.FatalErr = candr.FatalErr;
                 
                 
	for(num = 0;num < MOTORNUM; num++)
	{
                 candr.FbPos[num%2]=(double)( candr.FbbPos[num%2]* WheelPeri*candr.FlagForward[num%2] /(GearScale * MotorEncoder));
                 

                 
                 if(0 == firstflag[num])
		 {
                         candr.LastFbPos[num%2]	= candr.FbPos[num%2];
			 firstflag[num] = 1;
		 }
		 actpos[num] = (candr.FbPos[num%2] - candr.LastFbPos[num%2]);///��λʱ�䵥���ƶ����루mm��
		  candr.LastFbPos[num%2]	= candr.FbPos[num%2];
	}

	//gComData->FbVx = (gHalData->WheelHal[0].FbVel + gHalData->WheelHal[1].FbVel + gHalData->WheelHal[2].FbVel + gHalData->WheelHal[3].FbVel)/4.0;
	//gComData->FbVy = (-gHalData->WheelHal[0].FbVel + gHalData->WheelHal[1].FbVel + gHalData->WheelHal[2].FbVel - gHalData->WheelHal[3].FbVel)/4.0;
	//gComData->FbVthe = (-gHalData->WheelHal[0].FbVel + gHalData->WheelHal[1].FbVel - gHalData->WheelHal[2].FbVel + gHalData->WheelHal[3].FbVel)/(2*(gInnerData->Length + gInnerData->Width));
    
	baseth = (actpos[1] - actpos[0])/Len;                  ///��λʱ������ֲ������־��û���ֵ(rad)


        
  length = (actpos[0] + actpos[1]) / 2.0 ;      
/*	length = (actpos[0] + actpos[1]) / (2.0 * baseth);                      ///��λʱ��ת��뾶
	if(fabs(baseth) <= 0.001)
	{
	    basex = (actpos[0] + actpos[1])/2.0;
	}
	else
	{
	    basex = sin(baseth)*length;
	}
	basey = length*(1 - cos(baseth));

	gComData->FbPosx += basex*cos(gComData->FbPosthe) - basey*sin(gComData->FbPosthe);      ///Xƫ���� �ۼ�
	gComData->FbPosy += basex*sin(gComData->FbPosthe) + basey*cos(gComData->FbPosthe);      ///Yƫ����  �ۼ�
*/
       candr.length +=length;
	radcount+=baseth;
        candr.rad =(s16)(radcount*100);   //��������                                                        ///����ƫ����  �ۼ�
	
}


void Candr_init(void)
{
candr.FlagForward[0]=-1;
candr.FlagForward[1]=1;
candr.jczt=1;
}


/****************************************************************************
* ��    �ƣ�void moto_can (void)
* ��    �ܣ���������·�����
* ��ڲ�����ch��������
* ���ڲ�����null
* ˵    ��������gHalData->WheelHal[ch].Cmdvel���ٶȿ��Ƶ��
* ���÷�����null 
****************************************************************************/

void moto_can (void)
{
  static u8 moto_err_delay;
   s16 speed;
 CanTxMsg motot;
CalMotor_can();
Candr_init();
 //         motot.StdId=1;                //���������
          motot.IDE=CAN_Id_Standard;
          motot.RTR=CAN_RTR_Data;
          motot.DLC=8;
          motot.Data[2]=0;//����ģʽ��
          motot.Data[3]=0;//���������
          motot.Data[4]=1;//������ơ�
char i=0;
while(i<MOTORNUM){
//while(i<2){

if(0 == candr.FatalErr)       ///����܇�٣��ٱȣ�݆���L���ͷ���Ӌ�� 늙C�D�١�
		{
	        moto_err_delay=0;
                  candr.moto[i%2]= (int)(candr.CmdVel[i%2] * GearScale * 60  * candr.FlagForward[i%2] / WheelPeri);
			 
		}
		else
		{
                  moto_err_delay++;
                  candr.moto[i%2] = 0;
                  if (moto_err_delay>250){moto_err_delay=0;motot.Data[3]=1;}             //����������5���Ӻ��Զ�������ϡ�

		}
          speed=(u16)(candr.moto[i%2]*500/15);//ÿ���ӽ��ٶ�ת��Ϊ���塣0.1hz(100*5��������*6.5r/min)
 //speed=2000;
          motot.StdId=(i/2+1);
          motot.Data[0]=i%2+1;                      //���ѡ��
          motot.Data[1]=1;//���ݷ��ء�
          motot.Data[5]=speed<0?1:0;                      //�������
           speed=abs(speed);
         motot.Data[6]=(speed>>8)&0xff;        //����ٶ�H��
          motot.Data[7]=speed&0xff;             //����ٶ�L��
          CAN_Transmit(CAN1,&motot);
        i++;
//         if (i==1){for(u32 a=0;a<2000;a++);}// can���������䣬����ȴ���
}

}

void moto_turn(s16 forward,s16 back)
{candr.CmdVel[0]=(float)forward;
  candr.CmdVel[1]=(float)back;
  *(u16*)&writebuf[1]=candr.dy;
  moto_can();
}



