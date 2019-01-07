/***********************************************************************
�ļ����ƣ�Modbus.C
��    �ܣ�
��дʱ�䣺2018.3.25
�� д �ˣ�
ע    �⣺
***********************************************************************/
#include "main.h"

uint8_t g_SendMotorDt;                         //���Ͷ�ʱ

uint8_t rs485Buf[20] ;                         //
uint8_t FlagRecMotorMsg = 1;                   //���տ�������Ӧ���
int32_t position1,position2;


/* high CRC ?*/
static unsigned char auchCRCHi[] = {
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40
} ;
/* low CRC ?*/
static char auchCRCLo[] = {
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
0x40
};
unsigned short CRC16(unsigned char *puchMsg, unsigned short usDataLen ) /* ??? unsigned short ???? CRC */
{
	unsigned char uchCRCHi = 0xFF; /* CRC ???????*/
	unsigned char uchCRCLo = 0xFF; /* CRC ???????*/
	unsigned uIndex ; /* CRC ?????*/
	while (usDataLen--) /* ?????????*/
	{
		uIndex = uchCRCLo ^ *puchMsg++; /* ?? CRC */
		uchCRCLo = uchCRCHi ^ auchCRCHi[uIndex];
		uchCRCHi = auchCRCLo[uIndex];
	}
	return (uchCRCHi << 8 | uchCRCLo);
}

/****************************************************************************
* ��    �ƣ�eMBErrorCode MB_WritePluralReg(uint8_t uADR, uint16_t uRegBeginNo, int8_t uNum, int16_t uData1, int16_t uData2, int16_t uData3, int16_t uData4)
* ��    �ܣ�ModbusͨѶ��10ָ��
* ��ڲ�����uADR: Ҫд���485��ַ
						uReg��Ҫд��ļĴ�����ַ
						uData��Ҫд���ֵ
* ȫ�ֱ�����g_SendMotorDt�� ���ڼ��㷢�����ݺ�ĳ�ʱ
            FlagRecMotorMsg��   ��ʾ�Ƿ���յ�������    0��ʾδ�յ����ݣ�1��ʾ�յ����ݣ�������ȷ��2��ʾ�յ����ݣ����ݴ���
* ���ڲ�����eMBErrorCode  
            MB_ENOERR            û�д���
						MB_ETIMEDOUT         ���ճ�ʱ
            MB_EILLSTATE         Э��ջ�Ƿ�״̬
* ˵    ����
* ���÷�������
****************************************************************************/
eMBErrorCode MB_WritePluralReg(uint8_t uADR, uint16_t uRegBeginNo, uint16_t uNum, uint8_t uDateCount, int16_t uData1, int16_t uData2, int16_t uData3, int16_t uData4)
{
		uint16_t nCRC;//CRCУ��ֵ
		uint8_t uBuf[17];//����Buffer
		
    uBuf[0] = uADR;
    uBuf[1] = 0x10;
    uBuf[2] = (uint8_t)(uRegBeginNo >> 8);//��ȡ�Ĵ�����ʼ�ĸ��ֽ�
    uBuf[3] = (uint8_t)uRegBeginNo;//��ȡ�Ĵ�����ʼ�ĵ��ֽ�
    uBuf[4] = (uint8_t)(uNum >> 8);
		uBuf[5] = (uint8_t)(uNum);
		uBuf[6] = (uint8_t)(uDateCount);
		uBuf[7] = (uint8_t)(uData1 >> 8);
	  uBuf[8] = (uint8_t)(uData1);
		uBuf[9] = (uint8_t)(uData2 >> 8);
	  uBuf[10] = (uint8_t)(uData2);
		uBuf[11] = (uint8_t)(uData3 >> 8);
	  uBuf[12] = (uint8_t)(uData3);
		uBuf[13] = (uint8_t)(uData4 >> 8);
	  uBuf[14] = (uint8_t)(uData4);
    nCRC = CRC16(uBuf, 15);
    uBuf[15] = (uint8_t)nCRC;
    uBuf[16] = (uint8_t)(nCRC >> 8);
		RS485_Send_Data(uBuf, 17);

		cha_state.TurnctrlRespCnt = 0;
   
    //�ȴ�����
    g_SendMotorDt = 1;
    FlagRecMotorMsg = 0;
    while(cha_state.TurnctrlRespCnt < 30) //10ms�ȴ�
    {vTaskDelay(5);
        if(0 != FlagRecMotorMsg)
        {
						if(1 == FlagRecMotorMsg)//1��ʾ�յ����ݣ�������ȷ
						{
							FlagRecMotorMsg = 0;
							return MB_ENOERR;
						}
						if(2 == FlagRecMotorMsg)//2��ʾ�յ����ݣ����ݴ���
						{
							FlagRecMotorMsg = 0;
							return MB_EILLSTATE;
						}
						if(3 == FlagRecMotorMsg)//3��ʾ�յ����ݣ�CRC����
						{
							FlagRecMotorMsg = 0;
							return MB_CRCERR;
						}
        }
    }
	   
	return MB_ETIMEDOUT;//ִ�е��˲���˵���Ѿ����ճ�ʱ�ˡ�
	  
}
/****************************************************************************
* ��    �ƣ�eMBErrorCode MB_WriteSingleReg(uint16_t uReg, int16_t uData)
* ��    �ܣ�ModbusͨѶ��06ָ��
* ��ڲ�����uADR: Ҫд���485��ַ
						uReg��Ҫд��ļĴ�����ַ
						uData��Ҫд���ֵ
* ȫ�ֱ�����g_SendMotorDt�� ���ڼ��㷢�����ݺ�ĳ�ʱ
            FlagRecMotorMsg��   ��ʾ�Ƿ���յ�������    0��ʾδ�յ����ݣ�1��ʾ�յ����ݣ�������ȷ��2��ʾ�յ����ݣ����ݴ���
* ���ڲ�����eMBErrorCode  
            MB_ENOERR            û�д���
						MB_ETIMEDOUT         ���ճ�ʱ
            MB_EILLSTATE         Э��ջ�Ƿ�״̬
* ˵    ����
* ���÷�������
****************************************************************************/
eMBErrorCode MB_WriteSingleReg(uint16_t uReg, int16_t uData)//
{
		uint16_t nCRC;//CRCУ��ֵ
		uint8_t uBuf[8];//����Buffer
		
    uBuf[0] = 0x01;
    uBuf[1] = 0x06;
    uBuf[2] = 0x00;
    uBuf[3] = uReg;
    uBuf[4] = (uint8_t)(uData >> 8);
    uBuf[5] = (uint8_t)uData;
    nCRC = CRC16(uBuf, 6);
    uBuf[6] = (uint8_t)nCRC;
    uBuf[7] = (uint8_t)(nCRC >> 8);
	  RS485_Send_Data(uBuf, 8);

		cha_state.TurnctrlRespCnt = 0;
   
    //�ȴ�����
    g_SendMotorDt = 1;
    FlagRecMotorMsg = 0;
    while(cha_state.TurnctrlRespCnt < 10) //10ms�ȴ�
    {vTaskDelay(0);
        if(0 != FlagRecMotorMsg)
        {
						if(1 == FlagRecMotorMsg)//1��ʾ�յ����ݣ�������ȷ
						{
							FlagRecMotorMsg = 0;
							return MB_ENOERR;
						}
						if(2 == FlagRecMotorMsg)//2��ʾ�յ����ݣ����ݴ���
						{
							FlagRecMotorMsg = 0;
							return MB_EILLSTATE;
						}
						if(3 == FlagRecMotorMsg)//3��ʾ�յ����ݣ�CRC����
						{
							FlagRecMotorMsg = 0;
							return MB_CRCERR;
						}
        }
    }
	   
	return MB_ETIMEDOUT;//ִ�е��˲���˵���Ѿ����ճ�ʱ�ˡ�
	  
}
/****************************************************************************
* ��    �ƣ�eMBErrorCode MB_SetReg(void * cha_state)
* ��    �ܣ�ModbusͨѶ��03ָ��
* ��ڲ�����uADR: Ҫ��ȡ��485��ַ
			uRegBeginNo��Ҫ��ȡ�ļĴ�����ʼ��ַ
			uDataNum��Ҫ��ȡ�ļĴ�������
* ȫ�ֱ�����g_SendMotorDt�� ���ڼ��㷢�����ݺ�ĳ�ʱ
            FlagRecMotorMsg��   ��ʾ�Ƿ���յ�������    0��ʾδ�յ����ݣ�1��ʾ�յ����ݣ�������ȷ��2��ʾ�յ����ݣ����ݴ���
* ���ڲ�����eMBErrorCode  
            MB_ENOERR            û�д���
		    MB_ETIMEDOUT         ���ճ�ʱ
            MB_EILLSTATE         Э��ջ�Ƿ�״̬
* ˵    ����
* ���÷�������
****************************************************************************/
eMBErrorCode MB_SetReg(void * cha_state)//��ȡ����ٶȣ�λ�ã�ʱ��
{
		uint16_t nCRC;//CRCУ��ֵ
		uint8_t uBuf[8];//����Buffer
		AC_STA *p = (AC_STA *)cha_state;

    uBuf[0] = p->turn_moto_get.ADR;
    uBuf[1] = p->turn_moto_get.CMD;
    uBuf[2] = (uint8_t)(p->turn_moto_get.StartRegister >> 8);//��ȡ�Ĵ�����ʼ�ĸ��ֽ�
    uBuf[3] = (uint8_t)p->turn_moto_get.StartRegister;//��ȡ�Ĵ�����ʼ�ĵ��ֽ�
    uBuf[4] = (uint8_t)(p->turn_moto_get.RegisterCount >> 8);//��ȡ�Ĵ��������ĸ��ֽ�
    uBuf[5] = (uint8_t)p->turn_moto_get.RegisterCount;//��ȡ�Ĵ��������ĵ��ֽ�
    nCRC = CRC16(uBuf, 6);
    uBuf[6] = (uint8_t)nCRC;
    uBuf[7] = (uint8_t)(nCRC >> 8);
		RS485_Send_Data(uBuf, 8);

		p->TurnfreqRespCnt = 0;
    //�ȴ�����
    FlagRecMotorMsg = 0;
	  
	  g_SendMotorDt = 1;
    while(p->TurnfreqRespCnt < 10) //10ms�ȴ�
    {vTaskDelay(0);
        if(0 != FlagRecMotorMsg)
        {   
						if(1 == FlagRecMotorMsg)//1��ʾ�յ����ݣ�������ȷ
						{
							FlagRecMotorMsg = 0;
							return MB_ENOERR;
						}
						if(2 == FlagRecMotorMsg)//2��ʾ�յ����ݣ����ݴ���
						{
							FlagRecMotorMsg = 0;
							return MB_EILLSTATE;
						}
						if(3 == FlagRecMotorMsg)//3��ʾ�յ����ݣ�CRC����
						{
							FlagRecMotorMsg = 0;
							return MB_CRCERR;
						}
        }
    }
	   
	return MB_ETIMEDOUT;//ִ�е��˲���˵���Ѿ����ճ�ʱ�ˡ�
	  
}

/****************************************************************************
* ��    �ƣ�eMBErrorCode MB_Get03(void * cha_state)
* ��    �ܣ�ModbusͨѶ��03ָ��
* ��ڲ�����uADR: Ҫ��ȡ��485��ַ
			uRegBeginNo��Ҫ��ȡ�ļĴ�����ʼ��ַ
			uDataNum��Ҫ��ȡ�ļĴ�������
* ȫ�ֱ�����g_SendMotorDt�� ���ڼ��㷢�����ݺ�ĳ�ʱ
            FlagRecMotorMsg��   ��ʾ�Ƿ���յ�������    0��ʾδ�յ����ݣ�1��ʾ�յ����ݣ�������ȷ��2��ʾ�յ����ݣ����ݴ���
* ���ڲ�����eMBErrorCode  
            MB_ENOERR            û�д���
		    MB_ETIMEDOUT         ���ճ�ʱ
            MB_EILLSTATE         Э��ջ�Ƿ�״̬
* ˵    ����
* ���÷�������
****************************************************************************/
eMBErrorCode MB_Get03(void)//��ȡ���λ�á�����
{
		uint16_t nCRC;//CRCУ��ֵ
		uint8_t uBuf[8];//����Buffer

    uBuf[0] = 0x01;
    uBuf[1] = 0x03;
    uBuf[2] = 0x00;//��ȡ�Ĵ�����ʼ�ĸ��ֽ�
    uBuf[3] = 0x24;//��ȡ�Ĵ�����ʼ�ĵ��ֽ�
    uBuf[4] = 0x00;//��ȡ�Ĵ��������ĸ��ֽ�
    uBuf[5] = 0x02;//��ȡ�Ĵ��������ĵ��ֽ�
    nCRC = CRC16(uBuf, 6);
    uBuf[6] = (uint8_t)nCRC;
    uBuf[7] = (uint8_t)(nCRC >> 8);
		RS485_Send_Data(uBuf, 8);

		cha_state.TurnfreqRespCnt = 0;
    //�ȴ�����
    FlagRecMotorMsg = 0;
	  
	  g_SendMotorDt = 1;
    while(cha_state.TurnfreqRespCnt < 10) //10ms�ȴ�
    {vTaskDelay(0);
        if(0 != FlagRecMotorMsg)
        {   
						if(1 == FlagRecMotorMsg)//1��ʾ�յ����ݣ�������ȷ
						{
							FlagRecMotorMsg = 0;
							return MB_ENOERR;
						}
						if(2 == FlagRecMotorMsg)//2��ʾ�յ����ݣ����ݴ���
						{
							FlagRecMotorMsg = 0;
							return MB_EILLSTATE;
						}
						if(3 == FlagRecMotorMsg)//3��ʾ�յ����ݣ�CRC����
						{
							FlagRecMotorMsg = 0;
							return MB_CRCERR;
						}
        }
    }
	   
	return MB_ETIMEDOUT;//ִ�е��˲���˵���Ѿ����ճ�ʱ�ˡ�
	  
}
/********************************************************************************************************
**������Ϣ :void TurnMotorProcess(void)
**�������� :
**������� :position  ת������λ��
**������� :0�ɹ� or ������
*********************************************************************************************************/
//void TurnMotorProcess(void * cha_state)
void TurnMotorProcess(void)
{
		//int32_t position1,position2;
	  //AC_STA *p = (AC_STA *)cha_state;
		
		if(rs485Buf[1] == 0x03)
		{
				cha_state.turn_moto_getpro.Revolutions = (int16_t)(((uint16_t)rs485Buf[3]<<8 | (uint16_t)rs485Buf[4])*0.1);//�������Ƶ��
				cha_state.turn_moto_getpro.TurnctrlFlag =(uint8_t)((uint16_t)rs485Buf[5]<<8 | (uint16_t)rs485Buf[6]);	//λ���Ƿ����	
				position1 = (int32_t)((uint32_t)rs485Buf[7]<<24 | (uint32_t)rs485Buf[8]<<16 | (uint32_t)rs485Buf[9]<<8 | (uint32_t)rs485Buf[10]);//��ǰ���λ��
				position2 = (int32_t)(position1 - cha_state.zero_Value);
				cha_state.turn_moto_getpro.RealPosition = position2;
				cha_state.turn_moto_getpro.RemainingTime =(uint32_t)((uint32_t)rs485Buf[11]<<24 | (uint32_t)rs485Buf[12]<<16 | (uint32_t)rs485Buf[13]<<8 | (uint32_t)rs485Buf[14]);//ʣ��ʱ����ɵ�λ
		}
}
/*
�������ܣ���ȡ�������������Ӧ���ݰ�
������   nByte---���յ����ֽ�
*/
void GetResp(unsigned char *buff,unsigned int length)//���λ�ö�ȡ
{
		//int32_t position1,position2;
		uint16_t nCRC;//CRCУ��ֵ

                memcpy(rs485Buf,buff,length);
    if(WHEEL_DRIVE_ADR == rs485Buf[0])
    {
        switch(rs485Buf[1])
        {
						case 0x03://�����ּĴ���
								if(length == 17)
								{
										nCRC = CRC16(rs485Buf, 15);
										if(rs485Buf[15] != (uint8_t)nCRC && rs485Buf[16] != (uint8_t)(nCRC >> 8))
										{
											FlagRecMotorMsg = 3;//CRC�쳣
										}
										else
										{
											FlagRecMotorMsg = 1;//����
										}
										
										//memcpy(&cha_state.RX,&rs485Buf,13);
										//TurnMotorProcess(&cha_state);
										TurnMotorProcess();
										break;
										//gHalData.WheelHal[rs485Buf[0]-1].FdbSwitchFreq = (short)((unsigned short)rs485Buf[3]<<8 | (unsigned short)rs485Buf[4])*0.1;	
							  }
								if(length == 9)
								{
										nCRC = CRC16(rs485Buf, 7);
										if(rs485Buf[7] != (uint8_t)nCRC && rs485Buf[8] != (uint8_t)(nCRC >> 8))
										{
											FlagRecMotorMsg = 3;//CRC�쳣
										}
										else
										{
											FlagRecMotorMsg = 1;//����
										}
										position1 = (int32_t)((uint32_t)rs485Buf[3]<<24 | (uint32_t)rs485Buf[4]<<16 | (uint32_t)rs485Buf[5]<<8 | (uint32_t)rs485Buf[6]);
										position2 = (int32_t)(position1 - cha_state.zero_Value);
										cha_state.turn_moto_getpro.RealPosition =(int32_t)position2;
										break;
							  }
								//break;
						case 0x06://д�����Ĵ���
						case 0x10://д����Ĵ���
								if(length == 8){
										nCRC = CRC16(rs485Buf, 6);
										if(rs485Buf[6] != (uint8_t)nCRC && rs485Buf[7] != (uint8_t)(nCRC >> 8))
										{
											FlagRecMotorMsg = 3;//CRC�쳣
										}
										else
										{
											FlagRecMotorMsg = 1;//����
										}
								}
								break;
						case 0x83://�����Ĵ�������
						case 0x86://д���Ĵ�����
						case 0x90://д��Ĵ�����
								if(length == 5){
										nCRC = CRC16(rs485Buf, 3);
										if(rs485Buf[3] != (uint8_t)nCRC && rs485Buf[4] != (uint8_t)(nCRC >> 8))
										{
											FlagRecMotorMsg = 3;//CRC�쳣
										}
										else
										{
											FlagRecMotorMsg = 2;//�쳣
										}
								}
								break;
						default:
								
								break;
         }
    }
}								


