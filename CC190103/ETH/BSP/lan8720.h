#ifndef __LAN8720_H
#define __LAN8720_H
#include "stm32f4x7_eth.h"
#include "sys.h"
#include "set.h"
//////////////////////////////////////////////////////////////////////////////////	   
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//LAN8720 ��ʼ�� 
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/8/15
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//********************************************************************************
//�޸�˵�� 
//��
////////////////////////////////////////////////////////////////////////////////// 	
	
#define LAN8720_PHY_ADDRESS  	0x00				//LAN8720 PHYоƬ��ַ.
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* վ����ӿڣ�SMI
 * վ����ӿ�(SMI)����Ӧ�ó���ͨ��2���ߣ�ʱ��(MDC)��������(MDIO)��������PHY�Ĵ�����
 * �ýӿ�֧�ַ��ʶ��32��PHY��Ӧ�ó�����Դ�32��PHY��ѡ��һ��PHY��Ȼ�������PHY��
 * ����32���Ĵ�����ѡ��һ���Ĵ��������Ϳ������ݻ����״̬��Ϣ���������ʱ����ֻ��
 * ��һ��PHY�е�һ���Ĵ�������Ѱַ����MAC��PHY���ж�д������ʱ��Ӧ�ó������޸�
 * MII�ĵ�ַ�Ĵ�����MII�����ݼĴ������ڴ��ڼ��MII��ַ�Ĵ�����MII���ݼĴ���ִ�е�
 * д�������ᱻ���ԡ�
 * ��������ʹ�õ�LAN8742AоƬ��Ӧ����һ��PHY��ռ��������һ����ַ
 * ���������ַȷ���������Բο�LAN8742A�ֲᡰ3.7.1 PHYAD[0]: PHY Address 
 * Configuration��˵����
 */
#define ETHERNET_PHY_ADDRESS       0x00 /* ����Ӳ�������� */
	 
/*
	ETH_MDIO -------------------------> PA2
	ETH_MDC --------------------------> PC1
	ETH_MII_RX_CLK/ETH_RMII_REF_CLK---> PA1
	ETH_MII_RX_DV/ETH_RMII_CRS_DV ----> PA7
	ETH_MII_RXD0/ETH_RMII_RXD0 -------> PC4
	ETH_MII_RXD1/ETH_RMII_RXD1 -------> PC5
	ETH_MII_TX_EN/ETH_RMII_TX_EN -----> PB11
	ETH_MII_TXD0/ETH_RMII_TXD0 -------> PG13
	ETH_MII_TXD1/ETH_RMII_TXD1 -------> PG14
	ETH_NRST -------------------------> PI1
*/

#define RMII_MODE 

/* ETH_MDIO */
#define ETH_MDIO_GPIO_CLK               RCC_AHB1Periph_GPIOA
#define ETH_MDIO_PORT                   GPIOA
#define ETH_MDIO_PIN                    GPIO_Pin_2
#define ETH_MDIO_AF                     GPIO_AF_ETH
#define ETH_MDIO_SOURCE                 GPIO_PinSource2

/* ETH_MDC */
#define ETH_MDC_GPIO_CLK                RCC_AHB1Periph_GPIOC
#define ETH_MDC_PORT                    GPIOC
#define ETH_MDC_PIN                     GPIO_Pin_1
#define ETH_MDC_AF                      GPIO_AF_ETH
#define ETH_MDC_SOURCE                  GPIO_PinSource1

/* ETH_RMII_REF_CLK */
#define ETH_RMII_REF_CLK_GPIO_CLK       RCC_AHB1Periph_GPIOA
#define ETH_RMII_REF_CLK_PORT           GPIOA
#define ETH_RMII_REF_CLK_PIN            GPIO_Pin_1
#define ETH_RMII_REF_CLK_AF             GPIO_AF_ETH
#define ETH_RMII_REF_CLK_SOURCE         GPIO_PinSource1

/* ETH_RMII_CRS_DV */
#define ETH_RMII_CRS_DV_GPIO_CLK        RCC_AHB1Periph_GPIOA
#define ETH_RMII_CRS_DV_PORT            GPIOA
#define ETH_RMII_CRS_DV_PIN             GPIO_Pin_7
#define ETH_RMII_CRS_DV_AF              GPIO_AF_ETH
#define ETH_RMII_CRS_DV_SOURCE          GPIO_PinSource7

/* ETH_RMII_RXD0 */
#define ETH_RMII_RXD0_GPIO_CLK          RCC_AHB1Periph_GPIOC
#define ETH_RMII_RXD0_PORT              GPIOC
#define ETH_RMII_RXD0_PIN               GPIO_Pin_4
#define ETH_RMII_RXD0_AF                GPIO_AF_ETH
#define ETH_RMII_RXD0_SOURCE            GPIO_PinSource4

/* ETH_RMII_RXD1 */
#define ETH_RMII_RXD1_GPIO_CLK          RCC_AHB1Periph_GPIOC
#define ETH_RMII_RXD1_PORT              GPIOC
#define ETH_RMII_RXD1_PIN               GPIO_Pin_5
#define ETH_RMII_RXD1_AF                GPIO_AF_ETH
#define ETH_RMII_RXD1_SOURCE            GPIO_PinSource5

/* ETH_RMII_TX_EN */
#define ETH_RMII_TX_EN_GPIO_CLK         RCC_AHB1Periph_GPIOB
#define ETH_RMII_TX_EN_PORT             GPIOB
#define ETH_RMII_TX_EN_PIN              GPIO_Pin_11
#define ETH_RMII_TX_EN_AF               GPIO_AF_ETH
#define ETH_RMII_TX_EN_SOURCE           GPIO_PinSource11

/* ETH_RMII_TXD0 */
#define ETH_RMII_TXD0_GPIO_CLK          RCC_AHB1Periph_GPIOG
#define ETH_RMII_TXD0_PORT              GPIOG
#define ETH_RMII_TXD0_PIN               GPIO_Pin_13
#define ETH_RMII_TXD0_AF                GPIO_AF_ETH
#define ETH_RMII_TXD0_SOURCE            GPIO_PinSource13

/* ETH_RMII_TXD1 */
#define ETH_RMII_TXD1_GPIO_CLK          RCC_AHB1Periph_GPIOG
#define ETH_RMII_TXD1_PORT              GPIOG
#define ETH_RMII_TXD1_PIN               GPIO_Pin_14
#define ETH_RMII_TXD1_AF                GPIO_AF_ETH
#define ETH_RMII_TXD1_SOURCE            GPIO_PinSource14

/* ETH_NRST */
#define ETH_NRST_GPIO_CLK               RCC_AHB1Periph_GPIOI
#define ETH_NRST_PORT                   GPIOI
#define ETH_NRST_PIN_HIGH()             GPIO_SetBits(ETH_NRST_PORT,ETH_NRST_PIN)
#define ETH_NRST_PIN_LOW()              GPIO_ResetBits(ETH_NRST_PORT,ETH_NRST_PIN)

#if lease_board     //����С�����ذ�LCD��
#define ETH_NRST_PIN                    GPIO_Pin_8
#define LAN8720_RST 		   	PIout(8) 			//LAN8720��λ����	 
#else
#define ETH_NRST_PIN                    GPIO_Pin_1
#define LAN8720_RST 		   	PIout(1) 			//LAN8720��λ����	 
#endif 

extern ETH_DMADESCTypeDef *DMARxDscrTab;			//��̫��DMA�������������ݽṹ��ָ��
extern ETH_DMADESCTypeDef *DMATxDscrTab;			//��̫��DMA�������������ݽṹ��ָ�� 
extern uint8_t *Rx_Buff; 							//��̫���ײ���������buffersָ�� 
extern uint8_t *Tx_Buff; 							//��̫���ײ���������buffersָ��
extern ETH_DMADESCTypeDef  *DMATxDescToSet;			//DMA����������׷��ָ��
extern ETH_DMADESCTypeDef  *DMARxDescToGet; 		//DMA����������׷��ָ�� 
extern ETH_DMA_Rx_Frame_infos *DMA_RX_FRAME_infos;	//DMA�����յ���֡��Ϣָ��
 

u8 LAN8720_Init(void);
u8 LAN8720_Get_Speed(void);
u8 ETH_MACDMA_Config(void);
FrameTypeDef ETH_Rx_Packet(void);
u8 ETH_Tx_Packet(u16 FrameLength);
u32 ETH_GetCurrentTxBuffer(void);
u8 ETH_Mem_Malloc(void);
void ETH_Mem_Free(void);
#endif 

