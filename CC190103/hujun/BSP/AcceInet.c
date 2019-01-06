#include "AcceInet.h"
#include "can.h"

CanTxMsg TxMessage;			     //发送缓冲区
//////////////////////////////////////////////////////////////////////
int32_t *PSetSpeed_1;           //定义设定值
int32_t *PActualSpeed_1;        //定义实际值
uint32_t *PActualStatus_1;       //定义实际值

int32_t *PSetSpeed_2;           //定义设定值
int32_t *PActualSpeed_2;        //定义实际值
uint32_t *PActualStatus_2;       //定义实际值

int32_t *PSetSpeed_3;           //定义设定值
int32_t *PActualSpeed_3;        //定义实际值
uint32_t *PActualStatus_3;       //定义实际值

void CopleyInit(void)
{
//	PSetSpeed_1=(int32_t *)&AGV_REG[SET_SPEED_1];
//	PSetSpeed_2=(int32_t *)&AGV_REG[SET_SPEED_2];
//	PSetSpeed_3=(int32_t *)&AGV_REG[SET_SPEED_3];

//	PActualSpeed_1=(int32_t *)&AGV_REG[ACTUALSPEED_1];
//	PActualSpeed_2=(int32_t *)&AGV_REG[ACTUALSPEED_2];
//	PActualSpeed_3=(int32_t *)&AGV_REG[ACTUALSPEED_3];
//	
//	PActualStatus_1=(uint32_t *)&AGV_REG[ACTUALSTATUS_1];
//	PActualStatus_2=(uint32_t *)&AGV_REG[ACTUALSTATUS_2];
//	PActualStatus_3=(uint32_t *)&AGV_REG[ACTUALSTATUS_3];
	
}
void CopleyEnableMotor(unsigned int id,CanTxMsg *TxMessage_temp)
{
	TxMessage_temp->IDE=CAN_ID_STD;					 //标准模式
	TxMessage_temp->RTR=CAN_RTR_DATA;				 //发送的是数据
	TxMessage_temp->DLC=8;							 //数据长度为8字节
	TxMessage_temp->StdId = 0x600 + id;
	
	/*设置要发送的数据0-7*/
	TxMessage_temp->Data[0] = CMD_WRITE_2BYTE;
	TxMessage_temp->Data[1] = (unsigned char)(COPLEY_ENABLE_INDEX & 0x00ff);  // 00//COPLEY_ENABLE_INDEX=0x2300
	TxMessage_temp->Data[2] = (unsigned char)((COPLEY_ENABLE_INDEX & 0xff00) >> 8);	//23
	TxMessage_temp->Data[3] = (unsigned char)(COPLEY_ENABLE_SUBINDEX & 0x00ff);//00
	TxMessage_temp->Data[4] = (unsigned char)(COPLEY_ENABLE_CMD & 0x000000ff);//0b   0x000b
	TxMessage_temp->Data[5] = (unsigned char)((COPLEY_ENABLE_CMD & 0x0000ff00) >> 8);//00
	TxMessage_temp->Data[6] = 0x00;
	TxMessage_temp->Data[7] = 0x00;
//////	/*把报文存储到发送邮箱，发送*/
//////	CAN_Transmit(CANx, &TxMessage);
}
//
void CopleyDisableMotor(unsigned int id,CanTxMsg *TxMessage_temp)
{
	TxMessage_temp->IDE=CAN_ID_STD;					 //标准模式
	TxMessage_temp->RTR=CAN_RTR_DATA;				 //发送的是数据
	TxMessage_temp->DLC=8;							 //数据长度为8字节
	TxMessage_temp->StdId = 0x600 + id;
	
	TxMessage_temp->Data[0] = CMD_WRITE_2BYTE;
	TxMessage_temp->Data[1] = (unsigned char)(COPLEY_ENABLE_INDEX & 0x00ff);
	TxMessage_temp->Data[2] = (unsigned char)((COPLEY_ENABLE_INDEX & 0xff00) >> 8);	
	TxMessage_temp->Data[3] = (unsigned char)(COPLEY_ENABLE_SUBINDEX & 0x00ff);
	TxMessage_temp->Data[4] = (unsigned char)(COPLEY_DISABLE_CMD & 0x000000ff);
	TxMessage_temp->Data[5] = (unsigned char)((COPLEY_DISABLE_CMD & 0x0000ff00) >> 8);
	TxMessage_temp->Data[6] = 0x00;	
	TxMessage_temp->Data[7] = 0x00;	
//////	/*把报文存储到发送邮箱，发送*/
//////	CAN_Transmit(CANx, &TxMessage);
}

void CopleyCurvilinear (unsigned int id)
{
//	//TxMessage.StdId= 0x600 + id;
//  TxMessage.IDE=CAN_ID_STD;					 
//  TxMessage.RTR=CAN_RTR_DATA;				 //发送的是数据
//  TxMessage.DLC=8;
	
	TxMessage.StdId = 0x600 + id;
	TxMessage.Data[0] = CMD_WRITE_2BYTE;
	TxMessage.Data[1] = (unsigned char)(COPLEY_CURV_INDEX & 0x00ff);
	TxMessage.Data[2] = (unsigned char)((COPLEY_CURV_INDEX & 0xff00) >> 8);	
	TxMessage.Data[3] = (unsigned char)(COPLEY_CURV_SUBINDEX & 0x00ff);
	TxMessage.Data[4] = (unsigned char)(2 & 0x000000ff);
	TxMessage.Data[5] = (unsigned char)((2& 0x0000ff00) >> 8);
	TxMessage.Data[6] = 0x00;	
	TxMessage.Data[7] = 0x00;	
	/*把报文存储到发送邮箱，发送*/
	CAN_Transmit(CAN1, &TxMessage);
}
unsigned char CopleySetMotorSpd_A(unsigned int id,int spda) // 0x2341_0
{
	TxMessage.StdId = 0x600 + id;
	TxMessage.Data[0] = CMD_WRITE_4BYTE;
	TxMessage.Data[1] = (unsigned char)(0x2100 & 0x00ff);
	TxMessage.Data[2] = (unsigned char)((0x2100 & 0xff00) >> 8);	
	TxMessage.Data[3] = (unsigned char)(0x00 & 0x00ff);
	TxMessage.Data[4] = (unsigned char)(spda & 0x000000ff);
	TxMessage.Data[5] = (unsigned char)((spda & 0x0000ff00) >> 8);
	TxMessage.Data[6] = (unsigned char)((spda & 0x00ff0000) >> 16);
	TxMessage.Data[7] = (unsigned char)((spda & 0xff000000) >> 24);	
	/*把报文存储到发送邮箱，发送*/
	CAN_Transmit(CAN1, &TxMessage);
	return 0;
}
unsigned char CopleySetMotorSpd_B(unsigned int id,int spda) // 0x2341_0
{
	TxMessage.StdId = 0x600 + id;
	TxMessage.Data[0] = CMD_WRITE_4BYTE;
	TxMessage.Data[1] = (unsigned char)(0x2101 & 0x00ff);
	TxMessage.Data[2] = (unsigned char)((0x2101 & 0xff00) >> 8);	
	TxMessage.Data[3] = (unsigned char)(0x00 & 0x00ff);
	TxMessage.Data[4] = (unsigned char)(spda & 0x000000ff);
	TxMessage.Data[5] = (unsigned char)((spda & 0x0000ff00) >> 8);
	TxMessage.Data[6] = (unsigned char)((spda & 0x00ff0000) >> 16);
	TxMessage.Data[7] = (unsigned char)((spda & 0xff000000) >> 24);	
	/*把报文存储到发送邮箱，发送*/
	CAN_Transmit(CAN1, &TxMessage);
	return 0;
}

unsigned char CopleySetMotorReg(unsigned int id,unsigned short reg,int data) //
{
	TxMessage.StdId = 0x600 + id;
	TxMessage.Data[0] = CMD_WRITE_4BYTE;
	TxMessage.Data[1] = (unsigned char)(reg & 0x00ff);
	TxMessage.Data[2] = (unsigned char)((reg & 0xff00) >> 8);	
	TxMessage.Data[3] = (unsigned char)(0x00 & 0x00ff);
	TxMessage.Data[4] = (unsigned char)(data & 0x000000ff);
	TxMessage.Data[5] = (unsigned char)((data & 0x0000ff00) >> 8);
	TxMessage.Data[6] = (unsigned char)((data & 0x00ff0000) >> 16);
	TxMessage.Data[7] = (unsigned char)((data & 0xff000000) >> 24);	
	/*把报文存储到发送邮箱，发送*/
	CAN_Transmit(CAN1, &TxMessage);
	return 0;
}
unsigned char CopleyCmdWrite_2_Byte(unsigned int id,unsigned short Reg_Index,unsigned short Reg_Subindex,int Reg_Cmd) //
{
  TxMessage.StdId= 0x600 + id;
  TxMessage.IDE=CAN_ID_STD;					 
  TxMessage.RTR=CAN_RTR_DATA;				 //发送的是数据
  TxMessage.DLC=8;							 //数据长度为8字节
	
	/*设置要发送的数据0-7*/
	TxMessage.Data[0] = CMD_WRITE_2BYTE;
	TxMessage.Data[1] = (unsigned char)(Reg_Index & 0x00ff);  // 00//COPLEY_ENABLE_INDEX=0x2300
	TxMessage.Data[2] = (unsigned char)((Reg_Index & 0xff00) >> 8);	//23
	TxMessage.Data[3] = (unsigned char)(Reg_Subindex & 0x00ff);//00
	TxMessage.Data[4] = (unsigned char)(Reg_Cmd & 0x000000ff);//0b   0x000b
	TxMessage.Data[5] = (unsigned char)((Reg_Cmd & 0x0000ff00) >> 8);//00
	TxMessage.Data[6] = 0x00;
	TxMessage.Data[7] = 0x00;
	/*把报文存储到发送邮箱，发送*/
	CAN_Transmit(CAN1, &TxMessage);
	return 0;
}
unsigned char CopleyCmdWrite_4_Byte(unsigned int id,unsigned short Reg_Index,unsigned short Reg_Subindex,int Reg_Cmd) //
{
	TxMessage.StdId = 0x600 + id;
	TxMessage.Data[0] = CMD_WRITE_4BYTE;
	TxMessage.Data[1] = (unsigned char)(Reg_Index & 0x00ff);
	TxMessage.Data[2] = (unsigned char)((Reg_Index & 0xff00) >> 8);	
	TxMessage.Data[3] = (unsigned char)(Reg_Subindex & 0x00ff);
	TxMessage.Data[4] = (unsigned char)(Reg_Cmd & 0x000000ff);
	TxMessage.Data[5] = (unsigned char)((Reg_Cmd & 0x0000ff00) >> 8);
	TxMessage.Data[6] = (unsigned char)((Reg_Cmd & 0x00ff0000) >> 16);
	TxMessage.Data[7] = (unsigned char)((Reg_Cmd & 0xff000000) >> 24);	
	/*把报文存储到发送邮箱，发送*/
	CAN_Transmit(CAN1, &TxMessage);
	return 0;
}
unsigned char CopleyReadMotorReg(unsigned int id,unsigned short reg,CanTxMsg *TxMessage_temp) //
{
	TxMessage_temp->IDE=CAN_ID_STD;					 //标准模式
	TxMessage_temp->RTR=CAN_RTR_DATA;				 //发送的是数据
	TxMessage_temp->DLC=8;							 //数据长度为8字节
	TxMessage_temp->StdId = 0x600 + id;
	
	TxMessage_temp->Data[0] = CMD_READ_BYTE;
	TxMessage_temp->Data[1] = (unsigned char)(reg & 0x00ff);
	TxMessage_temp->Data[2] = (unsigned char)((reg & 0xff00) >> 8);	
	TxMessage_temp->Data[3] = (unsigned char)(0x00 & 0x00ff);
	TxMessage_temp->Data[4] = 0x00;
	TxMessage_temp->Data[5] = 0x00;
	TxMessage_temp->Data[6] = 0x00;
	TxMessage_temp->Data[7] = 0x00;
////////	/*把报文存储到发送邮箱，发送*/
////////	CAN_Transmit(CANx, &TxMessage);
	return 0;
}
//
unsigned char CopleySetMotorSpd(unsigned int id,int spd,CanTxMsg *TxMessage_temp) // 0x2341_0
{
	TxMessage_temp->IDE=CAN_ID_STD;					 //标准模式
	TxMessage_temp->RTR=CAN_RTR_DATA;				 //发送的是数据
	TxMessage_temp->DLC=8;							 //数据长度为8字节
	TxMessage_temp->StdId = 0x600 + id;
	TxMessage_temp->Data[0] = CMD_WRITE_4BYTE;
	TxMessage_temp->Data[1] = (unsigned char)(SPD_SET_INDEX & 0x00ff);
	TxMessage_temp->Data[2] = (unsigned char)((SPD_SET_INDEX & 0xff00) >> 8);	
	TxMessage_temp->Data[3] = (unsigned char)(SPD_SET_SUBINDEX & 0x00ff);
	TxMessage_temp->Data[4] = (unsigned char)(spd & 0x000000ff);
	TxMessage_temp->Data[5] = (unsigned char)((spd & 0x0000ff00) >> 8);
	TxMessage_temp->Data[6] = (unsigned char)((spd & 0x00ff0000) >> 16);
	TxMessage_temp->Data[7] = (unsigned char)((spd & 0xff000000) >> 24);	
//////	/*把报文存储到发送邮箱，发送*/
//////	CAN_Transmit(CANx, &TxMessage);
	return 0;
}	
//
unsigned char CopleyGetMotorSpd(unsigned int id,CanTxMsg *TxMessage_temp) // 0x2341_0
{
	TxMessage_temp->IDE=CAN_ID_STD;					 //标准模式
	TxMessage_temp->RTR=CAN_RTR_DATA;				 //发送的是数据
	TxMessage_temp->DLC=8;							 //数据长度为8字节
	TxMessage_temp->StdId = 0x600 + id;
	TxMessage_temp->Data[0] = CMD_READ_BYTE;
	TxMessage_temp->Data[1] = (unsigned char)(SPD_GET_INDEX & 0x00ff);
	TxMessage_temp->Data[2] = (unsigned char)((SPD_GET_INDEX & 0xff00) >> 8);	
	TxMessage_temp->Data[3] = (unsigned char)(SPD_GET_SUBINDEX & 0x00ff);
	TxMessage_temp->Data[4] = 0x00;
	TxMessage_temp->Data[5] = 0x00;
	TxMessage_temp->Data[6] = 0x00;
	TxMessage_temp->Data[7] = 0x00;
//////	/*把报文存储到发送邮箱，发送*/
//////	CAN_Transmit(CANx, &TxMessage);
	return 0;
}
//
unsigned char CopleyGetMotorPos(unsigned int id,CanTxMsg *TxMessage_temp) // 0x6063_0
{
	TxMessage_temp->IDE=CAN_ID_STD;					 //标准模式
	TxMessage_temp->RTR=CAN_RTR_DATA;				 //发送的是数据
	TxMessage_temp->DLC=8;							 //数据长度为8字节
	TxMessage_temp->StdId = 0x600 + id;
	TxMessage_temp->Data[0] = CMD_READ_BYTE;
	TxMessage_temp->Data[1] = (unsigned char)(POS_GET_INDEX & 0x00ff);
	TxMessage_temp->Data[2] = (unsigned char)((POS_GET_INDEX & 0xff00) >> 8);	
	TxMessage_temp->Data[3] = (unsigned char)(POS_GET_SUBINDEX & 0x00ff);
	TxMessage_temp->Data[4] = 0x00;
	TxMessage_temp->Data[5] = 0x00;
	TxMessage_temp->Data[6] = 0x00;
	TxMessage_temp->Data[7] = 0x00;
//////	/*把报文存储到发送邮箱，发送*/
//////	CAN_Transmit(CANx, &TxMessage);
	return 0;
}
//
unsigned char CopleySetVp(unsigned int id,short vp) // 0x2381_1
{
	TxMessage.StdId = 0x600 + id;
	TxMessage.Data[0] = CMD_WRITE_2BYTE;
	TxMessage.Data[1] = (unsigned char)(SPD_LOOP_SET_P_INDEX & 0x00ff);
	TxMessage.Data[2] = (unsigned char)((SPD_LOOP_SET_P_INDEX & 0xff00) >> 8);	
	TxMessage.Data[3] = (unsigned char)(SPD_LOOP_SET_P_SUBINDEX & 0x00ff);
	TxMessage.Data[4] = (unsigned char)(vp & 0x000000ff);
	TxMessage.Data[5] = (unsigned char)((vp & 0x0000ff00) >> 8);
	TxMessage.Data[6] = 0x00;	
	TxMessage.Data[7] = 0x00;	
	/*把报文存储到发送邮箱，发送*/
	CAN_Transmit(CAN1, &TxMessage);
	return 0;
}
//
unsigned char CopleyClearError(unsigned int id) // 0x2181_0
{
	TxMessage.StdId = 0x600 + id;
	TxMessage.Data[0] = CMD_READ_BYTE;
	TxMessage.Data[1] = (unsigned char)(COPLEY_CLEAR_ERROR_INDEX & 0x00ff);
	TxMessage.Data[2] = (unsigned char)((COPLEY_CLEAR_ERROR_INDEX & 0xff00) >> 8);	
	TxMessage.Data[3] = (unsigned char)(COPLEY_CLEAR_ERROR_SUBINDEX & 0x00ff);
	TxMessage.Data[4] = (unsigned char)(COPLEY_CLEAR_ERROR_CMD & 0x000000ff);
	TxMessage.Data[5] = (unsigned char)((COPLEY_CLEAR_ERROR_CMD & 0x0000ff00) >> 8);
	TxMessage.Data[6] = (unsigned char)((COPLEY_CLEAR_ERROR_CMD & 0x00ff0000) >> 16);
	TxMessage.Data[7] = (unsigned char)((COPLEY_CLEAR_ERROR_CMD & 0xff000000) >> 24);
	/*把报文存储到发送邮箱，发送*/
	CAN_Transmit(CAN1, &TxMessage);
	return 0;
}
//
unsigned char CopleySetVi(unsigned int id,short vi) // 0x2381_2
{
	TxMessage.StdId = 0x600 + id;
	TxMessage.Data[0] = CMD_WRITE_2BYTE;
	TxMessage.Data[1] = (unsigned char)(SPD_LOOP_SET_I_INDEX & 0x00ff);
	TxMessage.Data[2] = (unsigned char)((SPD_LOOP_SET_I_INDEX & 0xff00) >> 8);	
	TxMessage.Data[3] = (unsigned char)(SPD_LOOP_SET_I_SUBINDEX & 0x00ff);
	TxMessage.Data[4] = (unsigned char)(vi & 0x000000ff);
	TxMessage.Data[5] = (unsigned char)((vi & 0x0000ff00) >> 8);
	TxMessage.Data[6] = 0x00;	
	TxMessage.Data[7] = 0x00;	
	/*把报文存储到发送邮箱，发送*/
	CAN_Transmit(CAN1, &TxMessage);
	return 0;
}
////////////////////////////////////////////
