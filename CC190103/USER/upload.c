#include "FreeRTOS.h"
#include "upload.h"
#include "task.h"
#include "queue.h"
#include "string.h"
#include "LED2811.h"
#include "control.h"
#include "CAN_MAGS.h"
#include "CAN_ID.h"
#include "cpuinfo.h"
#include "gpio.h" 

SEND_D xSentData={0};
RECV_D xComData={0};
//  QueueHandle_t xDRV,xSYS,xCSB,xZHT,xYAK,xREV,xCON;


//  void upload_init(void)
//{
//;
//}




void upload(void)
{
      
            xSentData.start=SENTHEAD;
            xSentData.len=(sizeof(SEND_D)-1);
            xSentData.drvlen=SYSST.st_meter;
            xSentData.sysdl=SYSST.st_dl;
            xSentData.sysdy=SYSST.st_dy;
            xSentData.magsbit=CAR_MAGS.mags;
            xSentData.contemp=SYSST.st_contp;
            xSentData.drvtemp=SYSST.st_mototp;
            xSentData.sysfree=cpu_avg;
            //xSentData.ccerr=0x00|(SDOK<<7)|(SYSST.st_tbopen<<6)|(SYSST.st_man<<5)|(SYSST.st_lyk<<4)|(SYSST.st_ryk<<3);//SYSST.st_man:此信号工作中会误报
            xSentData.ccerr=0x00|(SDOK<<7)|(SYSST.st_tbopen<<6)|(SYSST.st_lyk<<4)|(SYSST.st_ryk<<3);
            if((!CCBZ)||(STOP)||(BIZ1)||(BIZ2))xSentData.ccerr|=(1<<5);
            if(SYSST.err>100)xSentData.ccerr|=0x04;else xSentData.ccerr&=(~0x04);//20MS 无信号扫描 
            if(CAR_MAGS.DRVERR>20)xSentData.ccerr|=0x02;else xSentData.ccerr&=(~0x02);//50MS 无信号扫描
            if(CANID.IDERR>20)xSentData.ccerr|=0x01;else xSentData.ccerr&=(~0x01);//50MS 无信号扫描
            xSentData.ccspeed=SYSST.con_speed;
            xSentData.ccturn=SYSST.st_turn/10;
            if(SYSST.st_high<2540)xSentData.high=(u8)SYSST.st_high/10;else xSentData.high=255;
            xSentData.ccauto=SYSST.st_mod;

            xSentData.sum=0;
            xSentData.end=SENTEND;
            for(int i = 0; i < 98; ++i)
            {
            xSentData.sum+=*(u8*)(&(xSentData.start)+i+4);
             }
}

 u8 jiema(s8 n,RECV_D* com_data)
 {
 if((com_data->datalen==(n-1))&&(com_data->start==RECVHEAD))
 {u8 sum=0;
            for(int i = 0; i < (com_data->datalen-3); ++i)
            {
            sum+=*(u8*)((u32)com_data+i+2);
             }
if((com_data->function==0x73)&&(sum==com_data->sum))
{
//if(xREV)xQueueSend(xREV,com_data,0); 
  memcpy(&xComData,com_data, n);

return 1;
}
 }  
 return 0;
 }