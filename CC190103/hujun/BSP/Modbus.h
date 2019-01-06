/**
  ******************************************************************************
  * @file    modbus.h
  * @author  wang ke
  * @version V0.1.0
  * @date    2015/6/17
  * @brief   Library configuration file.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2015 HIT ROBOT GROUP</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MODBUS_H
#define __MODBUS_H

#include "main.h"

typedef enum
{
    MB_ENOERR,                  /*!< no error. */
    MB_ENOREG,                  /*!< illegal register address. */	//非法地址
    MB_EINVAL,                  /*!< illegal argument. */	//非法参数
    MB_EPORTERR,                /*!< porting layer error. */	//移植层错误
    MB_ENORES,                  /*!< insufficient resources. */	//资源不足
    MB_EIO,                     /*!< I/O error. */
    MB_EILLSTATE,               /*!< protocol stack in illegal state. */  //协议栈非法状态
    MB_ETIMEDOUT,                /*!< timeout error occurred. *///超时
		MB_CRCERR					 //CRC Error
} eMBErrorCode;


#define REG_STOP_INSPM    		    0x40     //设置驱动器停止方式
#define REG_TARGET_PWM						0x42		 //设置驱动器占空比
#define REG_TARGET_SPEED_INSPM    0x43     //设置速度闭环控制目标速度
#define REG_TARGET_SPEED_INPOM    0x44	   //设置位置闭环控制目标速度
#define REG_TARGET_POS_HIGH	  	  0x46	   //电机目标位置的高字节
#define REG_TARGET_POS_LOW	      0x47	   //电机目标位置的低字节
#define REG_REALTIME_STATUS_SPD   0x22     //实时换向频率（转速）
#define REG_MOTOR_POLE_NUMBER     0x73     //电机极个数

#define MOTOR_NORMAL_STOP 			0x0000   //正常停止
#define MOTOR_EMERGENCY_STOP		0x0001   //急停
#define MOTOR_FREE_STOP         0x0002   //自由停止



extern uint8_t FlagRecMotorMsg;                         //接收控制器响应完成

unsigned short CRC16(unsigned char *puchMsg, unsigned short usDataLen );
void Com1SendStr(uint8_t *pData,uint8_t nSize);

eMBErrorCode MB_WriteSingleReg(uint16_t uReg, int16_t uData);
eMBErrorCode MB_WritePluralReg(uint8_t uADR, uint16_t uRegBeginNo, uint16_t uNum, uint8_t uDateCount, int16_t uData1, int16_t uData2, int16_t uData3, int16_t uData4);

extern void GetRs485Resp(uint8_t nByte);
extern void uart3_sendstr(char* str);
extern eMBErrorCode MB_Get03(void);
extern int32_t position1,position2;

extern void GetResp(unsigned char *buff,unsigned int length);






#endif
