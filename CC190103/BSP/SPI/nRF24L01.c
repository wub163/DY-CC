/*===========================================================================
* ��ַ ��http://yhmcu.taobao.com/   http://www.cdebyte.com/                 *
* ���� ������  ԭ �ں͵��ӹ�����  �� �ڰ��ص��ӿƼ����޹�˾                 * 
* �ʼ� ��yihe_liyong@126.com                                                *
* �绰 ��18615799380                                                        *
============================================================================*/

#include "nRF24L01.h"

extern INT8U SPI_ExchangeByte(INT8U input); // ͨ��SPI�������ݽ���,��bsp.c

/*===========================================================================
* ���� ��L01_ReadSingleReg() => ��ȡһ���Ĵ�����ֵ                          * 
* ���� ��Addr����ȡ�ļĴ�����ַ                                             * 
* ��� ��������ֵ                                                           * 
============================================================================*/
INT8U L01_ReadSingleReg(INT8U Addr)
{
    INT8U btmp;
    
    L01_CSN_LOW();
    SPI_ExchangeByte(R_REGISTER | Addr);
    btmp = SPI_ExchangeByte(0xFF);
    L01_CSN_HIGH();
    
    return (btmp);
}

/*===========================================================================
* ���� ��L01_WriteSingleReg() => д���ݵ�һ���Ĵ���                         * 
* ���� ��Addr��д��Ĵ����ĵ�ַ��Value����д���ֵ                          * 
============================================================================*/
void L01_WriteSingleReg(INT8U Addr, INT8U Value)
{
    L01_CSN_LOW();
    SPI_ExchangeByte(W_REGISTER | Addr);
    SPI_ExchangeByte(Value);
    L01_CSN_HIGH();
}

/*===========================================================================
* ���� ��L01_WriteMultiReg() => д���ݵ�����Ĵ���                          * 
* ���� ��StartAddr,д��Ĵ������׵�ַ��pBuff,ָ���д���ֵ��Length,����    * 
============================================================================*/
void L01_WriteMultiReg(INT8U StartAddr, INT8U *pBuff, INT8U Length)
{
    INT8U i;
    
    L01_CSN_LOW();
    SPI_ExchangeByte(W_REGISTER | StartAddr);
    for (i=0; i<Length; i++)    { SPI_ExchangeByte(*(pBuff+i)); }
    L01_CSN_HIGH();
}

/*===========================================================================
* ���� ��L01_FlushTX() => ��λTX FIFOָ��                                   * 
============================================================================*/
void L01_FlushTX(void)
{
    L01_CSN_LOW();
    SPI_ExchangeByte(FLUSH_TX);
    L01_CSN_HIGH();
}

/*===========================================================================
* ���� ��L01_FlushRX() => ��λRX FIFOָ��                                   *
============================================================================*/
void L01_FlushRX(void)
{
    L01_CSN_LOW();
    SPI_ExchangeByte(FLUSH_RX);
    L01_CSN_HIGH();
}

INT8U L01_ReadStatusReg(void)
{
    INT8U Status;
    L01_CSN_LOW();
    Status = SPI_ExchangeByte(R_REGISTER + L01REG_STATUS);
    L01_CSN_HIGH();
    return (Status);
}

/*===========================================================================
* ���� ��L01_ClearIRQ() => ����ж�                                         * 
* ���� ��IRQ_Source����Ҫ������ж�Դ                                       * 
============================================================================*/
void L01_ClearIRQ(INT8U IRQ_Source)
{
    INT8U btmp = 0;

    IRQ_Source &= (1<<RX_DR) | (1<<TX_DS) | (1<<MAX_RT);
    btmp = L01_ReadStatusReg();
    
    L01_CSN_LOW();
    SPI_ExchangeByte(W_REGISTER + L01REG_STATUS);
    SPI_ExchangeByte(IRQ_Source | btmp);
    L01_CSN_HIGH();
    
    L01_ReadStatusReg();
}

/*===========================================================================
* ���� ��L01_ReadIRQSource() => ��ȡ�ж�                                    *         
* ��� ���������ж�Դ                                                       * 
============================================================================*/
INT8U L01_ReadIRQSource(void)
{
    return (L01_ReadStatusReg() & ((1<<RX_DR)|(1<<TX_DS)|(1<<MAX_RT)));
}

/*===========================================================================
* ���� ��L01_ReadTopFIFOWidth() => ��ȡFIFO���ݿ���                         * 
============================================================================*/
INT8U L01_ReadTopFIFOWidth(void)
{
    INT8U btmp;
    
    L01_CSN_LOW();
    SPI_ExchangeByte(R_RX_PL_WID);
    btmp = SPI_ExchangeByte(0xFF);
    L01_CSN_HIGH();
    
    return (btmp);
}

/*===========================================================================
* ���� ��L01_ReadRXPayload() => ��ȡ���յ�������                            * 
* ���� ��pBuff��ָ���յ�������                                              * 
* ��� �����ݳ���                                                           * 
============================================================================*/
INT8U L01_ReadRXPayload(INT8U *pBuff)
{
    INT8U width, PipeNum;
    PipeNum = (L01_ReadSingleReg(L01REG_STATUS)>>1) & 0x07;
    width = L01_ReadTopFIFOWidth();

    L01_CSN_LOW();
    SPI_ExchangeByte(R_RX_PAYLOAD);
    for (PipeNum=0; PipeNum<width; PipeNum++)
    {
        *(pBuff+PipeNum) = SPI_ExchangeByte(0xFF);
    }
    L01_CSN_HIGH();
    L01_FlushRX();
    return (width);
}

/*===========================================================================
* ���� ��L01_WriteTXPayload_Ack() => д���ݵ�TXFIFO(��ACK����)              * 
* ���� ��pBuff��ָ���д������ݣ�nBytes��д�����ݵĳ���                    * 
============================================================================*/
void L01_WriteTXPayload_Ack(INT8U *pBuff, INT8U nBytes)
{
    INT8U btmp;
    INT8U length = (nBytes>32) ? 32 : nBytes;

    L01_FlushTX();
    L01_CSN_LOW();
    SPI_ExchangeByte(W_TX_PAYLOAD);
    for (btmp=0; btmp<length; btmp++)   { SPI_ExchangeByte(*(pBuff+btmp)); }
    L01_CSN_HIGH();
}

/*===========================================================================
* ���� ��L01_WriteTXPayload_Ack() => д���ݵ�TXFIFO(����ACK����)            * 
* ���� ��Data��ָ���д������ݣ�Data_Length��д�����ݵĳ���                * 
============================================================================*/
void L01_WriteTXPayload_NoAck(INT8U *Data, INT8U Data_Length)
{
    if ((Data_Length>32) || (Data_Length==0))   { return; }
        
    L01_CSN_LOW();
    SPI_ExchangeByte(W_TX_PAYLOAD_NOACK);
    while (Data_Length--)                       { SPI_ExchangeByte(*Data++); }
    L01_CSN_HIGH();
}

/*===========================================================================
* ���� ��L01_SetTXAddr() => ���÷���������ַ                                * 
* ���� ��pAddrָ����Ҫ���õĵ�ַ���ݣ�Addr_Length����ַ����                 * 
============================================================================*/
void L01_SetTXAddr(INT8U *pAddr, INT8U Addr_Length)
{
    INT8U Length = (Addr_Length>5) ? 5 : Addr_Length;
    L01_WriteMultiReg(L01REG_TX_ADDR, pAddr, Length);
}

/*===========================================================================
* ���� ��L01_SetRXAddr() => ���ý���������ַ                                * 
* ���� ��PipeNum���ܵ��ţ�pAddrָ����Ҫ���õ�ַ���ݣ�Addr_Length����ַ����  * 
============================================================================*/
void L01_SetRXAddr(INT8U PipeNum, INT8U *pAddr, INT8U Addr_Length)
{
    INT8U Length = (Addr_Length>5) ? 5 : Addr_Length;
    INT8U pipe = (PipeNum>5) ? 5 : PipeNum;

    L01_WriteMultiReg(L01REG_RX_ADDR_P0 + pipe, pAddr, Length);
}

/*===========================================================================
* ���� ��L01_SetSpeed() => ����L01����                                      * 
* ���� ��speed��=SPD_250K(250K), =SPD_1M(1M), =SPD_2M(2M)                   * 
============================================================================*/
void L01_SetSpeed(L01SPD speed)
{
	INT8U btmp = L01_ReadSingleReg(L01REG_RF_SETUP);

	btmp &= ~((1<<5) | (1<<3));
	
	switch (speed)
	{
	    case SPD_250K:  btmp |= (1<<5);             break;  // 250K
        case SPD_1M:    btmp &= ~((1<<5) | (1<<3)); break;  // 1M
        case SPD_2M:    btmp |= (1<<3);             break;  // 2M
        default:        break;                                     
	        
	}
	L01_WriteSingleReg(L01REG_RF_SETUP, btmp);
}

/*===========================================================================
* ���� ��L01_SetPower() => ����L01����                                      * 
* ���� ��power, =P_F18DBM(18DB),=P_F12DBM(12DB),=P_F6DBM(6DB),=P_0DBM(0DB)  *
============================================================================*/
void L01_SetPower(L01PWR power)
{
    INT8U btmp = L01_ReadSingleReg(L01REG_RF_SETUP) & ~0x07;
    
    switch(power)
    {
        case P_F18DBM:  btmp |= PWR_18DB; break;    // 18DBM
        case P_F12DBM:  btmp |= PWR_12DB; break;    // 12DBM
        case P_F6DBM:   btmp |= PWR_6DB;  break;    // 6DBM
        case P_0DBM:    btmp |= PWR_0DB;  break;    // 0DBM
        default:        break;
    }
    L01_WriteSingleReg(L01REG_RF_SETUP, btmp);
}

/*===========================================================================
* ���� ��L01_WriteHoppingPoint() => ����L01Ƶ��                             * 
* ���� ��FreqPoint�������õ�Ƶ��                                            * 
============================================================================*/
void L01_WriteHoppingPoint(INT8U FreqPoint)
{
    L01_WriteSingleReg(L01REG_RF_CH, FreqPoint & 0x7F);
}

/*===========================================================================
* ���� ��L01_SetTRMode() => ����L01ģʽ                                     * 
* ���� ��mode��=TX_MODE, TX mode; =RX_MODE, RX mode                         * 
============================================================================*/
void L01_SetTRMode(L01MD mode)
{
    INT8U controlreg = L01_ReadSingleReg(L01REG_CONFIG);
    if      (mode == TX_MODE)       { controlreg &= ~(1<<PRIM_RX); }
    else if (mode == RX_MODE)       { controlreg |= (1<<PRIM_RX); }

    L01_WriteSingleReg(L01REG_CONFIG, controlreg);
}

/*===========================================================================
* ���� ��L01_Init() => ��ʼ��L01                                             * 
============================================================================*/
void L01_Init(void)
{
    INT8U addr[5] = { INIT_ADDR };

    L01_CE_LOW();    
    L01_ClearIRQ(IRQ_ALL);
    
    // ʹ�ܹܵ�0��̬������
    L01_WriteSingleReg(L01REG_DYNPD, (1<<0));
    L01_WriteSingleReg(L01REG_FEATRUE, 0x06);
    L01_ReadSingleReg(L01REG_DYNPD);
    L01_ReadSingleReg(L01REG_FEATRUE);

    L01_WriteSingleReg(L01REG_CONFIG, (1<<EN_CRC)|(1<<PWR_UP));
    L01_WriteSingleReg(L01REG_EN_AA, (1<<ENAA_P0));     // �Զ�Ӧ�𣨹ܵ�0��
    L01_WriteSingleReg(L01REG_EN_RXADDR, (1<<ERX_P0));  // ʹ�ܽ��գ��ܵ�0��
    L01_WriteSingleReg(L01REG_SETUP_AW, AW_5BYTES);     // ��ַ���� 5byte
    L01_WriteSingleReg(L01REG_RETR, ARD_4000US|(REPEAT_CNT&0x0F));
                  
    L01_WriteSingleReg(L01REG_RF_CH, 60);               // ��ʼ��Ƶ��
    L01_WriteSingleReg(L01REG_RF_SETUP, 0x26); 
    L01_SetTXAddr(&addr[0], 5);                         // ���õ�ַ�����ͣ�
    L01_SetRXAddr(0, &addr[0], 5);                      // ���õ�ַ�����գ�
}

/*===========================================================================
-----------------------------------�ļ�����----------------------------------
===========================================================================*/