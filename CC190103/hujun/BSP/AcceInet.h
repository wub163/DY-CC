#ifndef __ACCEINET_H
#define	__ACCEINET_H
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

#define SPD_SET_LEN  8
#define COPLEY_ENABLE_INDEX    0x2300
#define COPLEY_ENABLE_SUBINDEX 0x00
#define COPLEY_ENABLE_CMD      0x000b // 0x000b
#define COPLEY_DISABLE_CMD     0x0000
#define COPLEY_CLEAR_ERROR_INDEX 0x2181
#define COPLEY_CLEAR_ERROR_SUBINDEX 0x00
#define COPLEY_CLEAR_ERROR_CMD 0xffffffff
#define SPD_SET_INDEX          0x2341
#define SPD_SET_SUBINDEX       0x00
#define SPD_GET_INDEX          0x6069
#define SPD_GET_SUBINDEX       0x00
#define POS_GET_INDEX          0x6063
#define POS_GET_SUBINDEX       0x00
#define SPD_LOOP_SET_P_INDEX   0x60f9// 0x2381
#define SPD_LOOP_SET_P_SUBINDEX  0x01
#define SPD_LOOP_SET_I_INDEX   0x60f9
#define SPD_LOOP_SET_I_SUBINDEX  0X02
#define SPD_LOOP_GET_P_INDEX   0x60f9
#define SPD_LOOP_GET_P_SUBINDEX  0x01
#define SPD_LOOP_GET_I_INDEX   0x60f9
#define SPD_LOOP_GET_I_SUBINDEX  0X02

#define COPLEY_CURV_INDEX    0x6086
#define COPLEY_CURV_SUBINDEX 0x00

#define CMD_WRITE_1BYTE 			0x2F
#define CMD_WRITE_2BYTE 			0x2B
#define CMD_WRITE_3BYTE 			0x27
#define CMD_WRITE_4BYTE 			0x23
#define CMD_READ_BYTE 				0x40

#define COPLEY_ACT_INDEX    			0x2100			//acceleration
#define COPLEY_ACT_SUBINDEX 			0x00
#define COPLEY_DET_INDEX    			0x2101			//Deceleration
#define COPLEY_DET_SUBINDEX 			0x00

#define COPLEY_STATUS_INDEX    		0x1002			//STATUS



//////////////////////////////////////////////////////////////////////
void CopleyInit(void);
void CopleyEnableMotor(unsigned int id,CanTxMsg *TxMessage_temp);
void CopleyDisableMotor(unsigned int id,CanTxMsg *TxMessage_temp);
unsigned char CopleySetMotorSpd(unsigned int id,int spd,CanTxMsg *TxMessage_temp);
unsigned char CopleyGetMotorSpd(unsigned int id,CanTxMsg *TxMessage_temp);
unsigned char CopleyGetMotorPos(unsigned int id,CanTxMsg *TxMessage_temp);
unsigned char CopleySetVp(unsigned int id,short vp);
unsigned char CopleyClearError(unsigned int id);
unsigned char CopleySetVi(unsigned int id,short vi);
void CopleyCurvilinear (unsigned int id);
unsigned char CopleySetMotorSpd_A(unsigned int id,int spda);
unsigned char CopleySetMotorSpd_B(unsigned int id,int spda);
unsigned char CopleySetMotorReg(unsigned int id,unsigned short reg,int data);
unsigned char CopleyCmdWrite_2_Byte(unsigned int id,unsigned short Reg_Index,unsigned short Reg_Subindex,int Reg_Cmd);
unsigned char CopleyCmdWrite_4_Byte(unsigned int id,unsigned short Reg_Index,unsigned short Reg_Subindex,int Reg_Cmd);
unsigned char CopleyReadMotorReg(unsigned int id,unsigned short reg,CanTxMsg *TxMessage_temp);

extern CanTxMsg TxMessage;
#endif

////////////////////////////////////////////
