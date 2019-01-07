/*===========================================================================
* ��ַ ��http://yhmcu.taobao.com/   http://www.cdebyte.com/                 *
* ���� ������  ԭ �ں͵��ӹ�����  �� �ڰ��ص��ӿƼ����޹�˾                 * 
* �ʼ� ��yihe_liyong@126.com                                                *
* �绰 ��18615799380                                                        *
============================================================================*/

#ifndef _nRF24L01_H_
#define _nRF24L01_H_

#include "STM8l10x_conf.h"
#include "MyTypedef.h"
#include "nRF24L01_Reg.h"

#define REPEAT_CNT      15  // 0-15, repeat transmit count
#define INIT_ADDR       1, 2, 3, 4, 5

// nRF24L01P��ؿ������Ŷ��壬 CSN(PB4), IRQ(PA3), CE(PA2) 
#define PORT_L01_CSN    GPIOB
#define PIN_L01_CSN     GPIO_Pin_4

#define PORT_L01_IRQ    GPIOA
#define PIN_L01_IRQ     GPIO_Pin_3

#define PORT_L01_CE     GPIOA
#define PIN_L01_CE      GPIO_Pin_2

#define L01_CSN_LOW()   GPIO_ResetBits(PORT_L01_CSN, PIN_L01_CSN)
#define L01_CSN_HIGH()  GPIO_SetBits(PORT_L01_CSN, PIN_L01_CSN)

#define L01_CE_LOW()    GPIO_ResetBits(PORT_L01_CE, PIN_L01_CE)
#define L01_CE_HIGH()   GPIO_SetBits(PORT_L01_CE, PIN_L01_CE)

#define L01_IRQ_READ()  GPIO_ReadInputDataBit(PORT_L01_IRQ, PIN_L01_IRQ)

// nRF24L01P��غ����ӿ�
// ��ʼ��L01
void L01_Init(void);

// ��λTX FIFOָ��      
void L01_FlushTX(void);

// ��λRX FIFOָ��     
void L01_FlushRX(void);     

// ��ȡ�ж�
INT8U L01_ReadIRQSource(void);          

// ����ж�
#define IRQ_ALL  ((1<<RX_DR) | (1<<TX_DS) | (1<<MAX_RT))
void L01_ClearIRQ(INT8U IRQ_Source); 
   
// ��ȡFIFO���ݿ��
INT8U L01_ReadTopFIFOWidth(void);

// ��ȡ���յ�������       
INT8U L01_ReadRXPayload(INT8U *pBuff);  

// ����L01ģʽ 
typedef enum{ TX_MODE, RX_MODE } L01MD; 
void L01_SetTRMode(L01MD mode);

// ����L01����                 
typedef enum{ SPD_250K, SPD_1M, SPD_2M } L01SPD;
void L01_SetSpeed(L01SPD speed);

// ����L01����                 
typedef enum{ P_F18DBM, P_F12DBM, P_F6DBM, P_0DBM } L01PWR;
void L01_SetPower(L01PWR power);

// ����L01Ƶ��                
void L01_WriteHoppingPoint(INT8U FreqPoint);    

INT8U L01_ReadStatusReg(void);

// д���ݵ�һ���Ĵ���
void L01_WriteSingleReg(INT8U Addr, INT8U Value);

// ��ȡһ���Ĵ�����ֵ   
INT8U L01_ReadSingleReg(INT8U Addr);

// ��ȡ����Ĵ�����ֵ                 
void L01_ReadMultiReg(INT8U StartAddr, INT8U nBytes, INT8U *pBuff);

// д���ݵ�����Ĵ���
void L01_WriteMultiReg(INT8U StartAddr, INT8U *pBuff, INT8U Length);

// д���ݵ�TXFIFO(��ACK����)
void L01_WriteTXPayload_Ack(INT8U *pBuff, INT8U nBytes);

// д���ݵ�TXFIFO(����ACK����)
void L01_WriteTXPayload_NoAck(INT8U *Data, INT8U Data_Length);

// ���÷��������ַ
void L01_SetTXAddr(INT8U *pAddr, INT8U Addr_Length);

// ���ý��������ַ
void L01_SetRXAddr(INT8U PipeNum, INT8U *pAddr, INT8U Addr_Length);

#endif//_nRF24L01_H_

/*===========================================================================
-----------------------------------�ļ�����----------------------------------
===========================================================================*/