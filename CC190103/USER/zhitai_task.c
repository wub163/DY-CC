#include "FreeRTOS.h"
#include "task.h"
#include "upload.h"
#include "zhitai_task.h"
#include "gpio.h" 
#include "../mpu6050/mpu6050.h"
#include "../i2c/i2c.h"
#include "../dmp/eMPL/inv_mpu_dmp_motion_driver.h"
#include "../dmp/eMPL/inv_mpu.h"
#include "../csb/i2c_CSB.h"
#include "math.h"

#define q30  1073741824.0f

typedef struct
{
  float dm[4];
  int16_t ACC_X_OUT;                                                          //原始数据：加速度计x轴输出数据
   int16_t ACC_Y_OUT;                                                          //原始数据：加速度计y轴输出数据
   int16_t ACC_Z_OUT;                                                          //原始数据：加速度计z轴输出数据        
   int16_t GYR_X_OUT;                                                           //原始数据：陀螺仪y轴输出数据
   int16_t GYR_Y_OUT;                                                           //原始数据：陀螺仪y轴输出数据
   int16_t GYR_Z_OUT;                                                           //原始数据：陀螺仪z轴输出数据
   int16_t MAG_X_OUT;                                                           //原始数据：磁力计x轴输出数据
   int16_t MAG_Y_OUT;                                                           //原始数据：磁力计y轴输出数据
   int16_t MAG_Z_OUT;                                                           //原始数据：磁力计z轴输出数据
   float Yaw;                                                                   //姿态数据：欧拉角之一偏航角
   float Roll;                                                                  //姿态数据：欧拉角之一转动角
   float Pitch;                                                                 //姿态数据：欧拉角之一倾斜角
}attitudeData;
attitudeData myAttitudeData;


QueueHandle_t  xZHTt=NULL,xCSBt=NULL;

void READ_EulerAngle_DATA(void)
{
  float q0=1.0f,q1=0.0f,q2=0.0f,q3=0.0f;
  short gyro[3], accel[3];
  unsigned long sensor_timestamp;
  short  sensors;
  unsigned char more;
  long quat[4];
  float Yaw,Roll,Pitch;
  dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors,&more);
  if (sensors & INV_WXYZ_QUAT )
  {
    q0=quat[0] / q30;
    q1=quat[1] / q30;
    q2=quat[2] / q30;
    q3=quat[3] / q30;
    myAttitudeData.dm[0]=q0;
    myAttitudeData.dm[1]=q1;
    myAttitudeData.dm[2]=q2;
    myAttitudeData.dm[3]=q3;
    myAttitudeData.GYR_X_OUT=gyro[0];   //陀螺
    myAttitudeData.GYR_Y_OUT=gyro[1];
    myAttitudeData.GYR_Z_OUT=gyro[2];
    myAttitudeData.ACC_X_OUT=accel[0];  //重力
    myAttitudeData.ACC_Y_OUT=accel[1];
    myAttitudeData.ACC_Z_OUT=accel[2];
    Pitch  = asin(-2 * q1 * q3 + 2 * q0* q2)* 57.3; // pitch
    Roll = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.3; // roll
    Yaw = atan2(2*(q1*q2 + q0*q3),q0*q0+q1*q1-q2*q2-q3*q3) * 57.3;
    myAttitudeData.Yaw=Yaw;
    myAttitudeData.Roll=Roll;
    myAttitudeData.Pitch=Pitch;    
   }              
}


void zt_task(void* pv)
{
  u16 count=0;
u16 csb;
float tempp;
  xZHT_D zt={0,};  
  u8 CSB[8]={0,};
  xZHTt=xQueueCreate( 2, sizeof(xZHT_D ) );     //姿态
  xCSBt=xQueueCreate( 2, sizeof(xCSB_D ) );     //超声波
//I2cCSB_Init();
//I2C_Config();

  
  dmp_init(200);
   
  while(1){

// if((count++)%2==0){CSB_go(0xe8);}
//// if((count++)%3==0){CSB_go(0xe8);}

#if lease_board     //激光小车主控板
    if( mpu_get_id()==0X68){
#else
    if( mpu_get_id()==0X68){
#endif 
 
    READ_EulerAngle_DATA();
zt.dmp[0]=myAttitudeData.dm[0];
zt.dmp[1]=myAttitudeData.dm[1];
zt.dmp[2]=myAttitudeData.dm[2];
zt.dmp[3]=myAttitudeData.dm[3];
zt.alj[0]=myAttitudeData.Yaw;           //偏航角
zt.alj[1]=myAttitudeData.Roll;          //横滚角
zt.alj[2]=myAttitudeData.Pitch;         //俯仰角
zt.jaosd[0]=myAttitudeData.GYR_X_OUT;
zt.jaosd[1]=myAttitudeData.GYR_Y_OUT;
zt.jaosd[2]=myAttitudeData.GYR_Z_OUT;
zt.jasd[0]=myAttitudeData.ACC_X_OUT;
zt.jasd[1]=myAttitudeData.ACC_Y_OUT;
zt.jasd[2]=myAttitudeData.ACC_Z_OUT;
mpu_get_temperature(&tempp,0);
zt.sys_wd=(s16)(tempp*100);
 if(xZHTt)xQueueSend(xZHTt,&zt,0); 
    }
    if(1){   
    if(xCSBt)xQueueSend(xCSBt,CSB,0); 
    }
    
    vTaskDelay(30/portTICK_PERIOD_MS);
//uxTaskGetStackHighWaterMark( NULL );

        }


}