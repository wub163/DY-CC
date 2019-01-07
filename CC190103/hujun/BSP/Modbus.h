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
    MB_ENOREG,                  /*!< illegal register address. */	//�Ƿ���ַ
    MB_EINVAL,                  /*!< illegal argument. */	//�Ƿ�����
    MB_EPORTERR,                /*!< porting layer error. */	//��ֲ�����
    MB_ENORES,                  /*!< insufficient resources. */	//��Դ����
    MB_EIO,                     /*!< I/O error. */
    MB_EILLSTATE,               /*!< protocol stack in illegal state. */  //Э��ջ�Ƿ�״̬
    MB_ETIMEDOUT,                /*!< timeout error occurred. *///��ʱ
		MB_CRCERR					 //CRC Error
} eMBErrorCode;


#define REG_STOP_INSPM    		    0x40     //����������ֹͣ��ʽ
#define REG_TARGET_PWM						0x42		 //����������ռ�ձ�
#define REG_TARGET_SPEED_INSPM    0x43     //�����ٶȱջ�����Ŀ���ٶ�
#define REG_TARGET_SPEED_INPOM    0x44	   //����λ�ñջ�����Ŀ���ٶ�
#define REG_TARGET_POS_HIGH	  	  0x46	   //���Ŀ��λ�õĸ��ֽ�
#define REG_TARGET_POS_LOW	      0x47	   //���Ŀ��λ�õĵ��ֽ�
#define REG_REALTIME_STATUS_SPD   0x22     //ʵʱ����Ƶ�ʣ�ת�٣�
#define REG_MOTOR_POLE_NUMBER     0x73     //���������

#define MOTOR_NORMAL_STOP 			0x0000   //����ֹͣ
#define MOTOR_EMERGENCY_STOP		0x0001   //��ͣ
#define MOTOR_FREE_STOP         0x0002   //����ֹͣ



extern uint8_t FlagRecMotorMsg;                         //���տ�������Ӧ���

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
