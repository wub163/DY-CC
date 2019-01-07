#include "netif/ethernetif.h" 
#include "lan8720.h"  
#include "netconf.h" 
#include "netif/etharp.h"  
#include "string.h"  

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/lwip_sys.h"
#include "netif/etharp.h"
#include "lwip/timer.h"

#include "stm32f4x7_eth.h"

#include <string.h>



static struct netif *s_pxNetIf = NULL;
//ETH_IRQHandler �� ethernetif_input ֮���ͬ���ź�
xSemaphoreHandle s_xSemaphore = NULL;


//���� ethernetif_input ���񣬴�����յ�������
//���ݽ��� ---> ETH_IRQHandler -----s_xSemaphore---->ethernetif_input
//ethernetif_input ��������ȼ��Ͷ�ջ
#define netifINTERFACE_TASK_STACK_SIZE		( 50 )
#define netifINTERFACE_TASK_PRIORITY		( configMAX_PRIORITIES - 1 )
/* The time to block waiting for input. */
#define emacBLOCK_TIME_WAITING_FOR_INPUT	( ( portTickType ) 100 )	//�ȴ��ź��ʱ��

#define netifGUARD_BLOCK_TIME				( 250 )			//�ȴ��ź��ʱ��

static void ethernetif_input( void * pvParameters );
//static void arp_timer(void *arg);


//��ethernetif_init()�������ڳ�ʼ��Ӳ��
//netif:�����ṹ��ָ�� 
//����ֵ:ERR_OK,����
//       ����,ʧ��
static err_t low_level_init(struct netif *netif)
{
#ifdef CHECKSUM_BY_HARDWARE
	int i; 
#endif 
	netif->hwaddr_len = ETHARP_HWADDR_LEN; //����MAC��ַ����,Ϊ6���ֽ�
	//��ʼ��MAC��ַ,����ʲô��ַ���û��Լ�����,���ǲ����������������豸MAC��ַ�ظ�
	netif->hwaddr[0]=lwipdev.mac[0]; 
	netif->hwaddr[1]=lwipdev.mac[1]; 
	netif->hwaddr[2]=lwipdev.mac[2];
	netif->hwaddr[3]=lwipdev.mac[3];
	netif->hwaddr[4]=lwipdev.mac[4];
	netif->hwaddr[5]=lwipdev.mac[5];
	netif->mtu=1500; //��������䵥Ԫ,����������㲥��ARP����

	netif->flags = NETIF_FLAG_BROADCAST|NETIF_FLAG_ETHARP|NETIF_FLAG_LINK_UP;
	

  s_pxNetIf =netif;
 
  /* create binary semaphore used for informing ethernetif of frame reception */
  if (s_xSemaphore == NULL)
  {
    s_xSemaphore= xSemaphoreCreateCounting(20,0);
  }

	ETH_MACAddressConfig(ETH_MAC_Address0, netif->hwaddr); //��STM32F4��MAC��ַ�Ĵ�����д��MAC��ַ
	ETH_DMATxDescChainInit(DMATxDscrTab, Tx_Buff, ETH_TXBUFNB);
	ETH_DMARxDescChainInit(DMARxDscrTab, Rx_Buff, ETH_RXBUFNB);

        
        
          /* Enable Ethernet Rx interrrupt */
  { 
    for(i=0; i<ETH_RXBUFNB; i++)
    {
      ETH_DMARxDescReceiveITConfig(&DMARxDscrTab[i], ENABLE);
    }
  }




#ifdef CHECKSUM_BY_HARDWARE 	//ʹ��Ӳ��֡У��
	for(i=0;i<ETH_TXBUFNB;i++)	//ʹ��TCP,UDP��ICMP�ķ���֡У��,TCP,UDP��ICMP�Ľ���֡У����DMA��������
	{
		ETH_DMATxDescChecksumInsertionConfig(&DMATxDscrTab[i], ETH_DMATxDesc_ChecksumTCPUDPICMPFull);
	}
#endif

     xTaskCreate(ethernetif_input, (char*) "Eth_if", netifINTERFACE_TASK_STACK_SIZE, NULL,
              netifINTERFACE_TASK_PRIORITY,NULL);

   
    ETH_Start(); //����MAC��DMA				
	return ERR_OK;
} 
//���ڷ������ݰ�����ײ㺯��(lwipͨ��netif->linkoutputָ��ú���)
//netif:�����ṹ��ָ��
//p:pbuf���ݽṹ��ָ��
//����ֵ:ERR_OK,��������
//       ERR_MEM,����ʧ��
static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
  static xSemaphoreHandle xTxSemaphore = NULL;
  struct pbuf *q;
  uint32_t l = 0;
  u8 *buffer ;
  
  if (xTxSemaphore == NULL)
  {
    vSemaphoreCreateBinary (xTxSemaphore);
  } 
   
  if (xSemaphoreTake(xTxSemaphore, netifGUARD_BLOCK_TIME))
  {
    buffer =  (u8 *)(DMATxDescToSet->Buffer1Addr);
    for(q = p; q != NULL; q = q->next) 
    {
      memcpy((u8_t*)&buffer[l], q->payload, q->len);
      l = l + q->len;
    }
	ETH_Prepare_Transmit_Descriptors(l);	
    xSemaphoreGive(xTxSemaphore);
  }

  return ERR_OK;
}



static struct pbuf * low_level_input(struct netif *netif)
{
  struct pbuf *p, *q;
  u16_t len;
  uint32_t l=0,i =0;
  FrameTypeDef frame;
  u8 *buffer;
  __IO ETH_DMADESCTypeDef *DMARxNextDesc;
  
  p = NULL;
  
  /* Get received frame */
  frame = ETH_Get_Received_Frame_interrupt();
  
  /* check that frame has no error */
  if ((frame.descriptor->Status & ETH_DMARxDesc_ES) == (uint32_t)RESET)
  {
    
    /* Obtain the size of the packet and put it into the "len" variable. */
    len = frame.length;
    buffer = (u8 *)frame.buffer;

    /* We allocate a pbuf chain of pbufs from the pool. */
    p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
 
    /* Copy received frame from ethernet driver buffer to stack buffer */
    if (p != NULL)
    { 
      for (q = p; q != NULL; q = q->next)
      {
        memcpy((u8_t*)q->payload, (u8_t*)&buffer[l], q->len);
        l = l + q->len;
      } 
    }
  }
  
  /* Release descriptors to DMA */
  /* Check if received frame with multiple DMA buffer segments */
  if (DMA_RX_FRAME_infos->Seg_Count > 1)
  {
    DMARxNextDesc = DMA_RX_FRAME_infos->FS_Rx_Desc;
  }
  else
  {
    DMARxNextDesc = frame.descriptor;
  }
  
  /* Set Own bit in Rx descriptors: gives the buffers back to DMA */
  for (i=0; i<DMA_RX_FRAME_infos->Seg_Count; i++)
  {  
    DMARxNextDesc->Status = ETH_DMARxDesc_OWN;
    DMARxNextDesc = (ETH_DMADESCTypeDef *)(DMARxNextDesc->Buffer2NextDescAddr);
  }
  
  /* Clear Segment_Count */
  DMA_RX_FRAME_infos->Seg_Count =0;
  
  
  /* When Rx Buffer unavailable flag is set: clear it and resume reception */
  if ((ETH->DMASR & ETH_DMASR_RBUS) != (u32)RESET)  
  {
    /* Clear RBUS ETHERNET DMA flag */
    ETH->DMASR = ETH_DMASR_RBUS;
      
    /* Resume DMA reception */
    ETH->DMARPDR = 0;
  }
  return p;
}


//������������(lwipֱ�ӵ���)
//netif:�����ṹ��ָ��
//����ֵ:ERR_OK,��������
//       ERR_MEM,����ʧ��
void ethernetif_input( void * pvParameters )
{
  struct pbuf *p;



  for( ;; )
  {
    if (xSemaphoreTake( s_xSemaphore, emacBLOCK_TIME_WAITING_FOR_INPUT)==pdTRUE)
    {
      p = low_level_input( s_pxNetIf );
      if (ERR_OK != s_pxNetIf->input( p, s_pxNetIf))
      {
        pbuf_free(p);
        p=NULL;
      }
    }
//    uxTaskGetStackHighWaterMark( NULL );
  }
}  
//ʹ��low_level_init()��������ʼ������
//netif:�����ṹ��ָ��
//����ֵ:ERR_OK,����
//       ����,ʧ��
err_t ethernetif_init(struct netif *netif)
{
	LWIP_ASSERT("netif!=NULL",(netif!=NULL));
#if LWIP_NETIF_HOSTNAME			//LWIP_NETIF_HOSTNAME 
	netif->hostname="lwip";  	//��ʼ������
#endif 
	netif->name[0]=IFNAME0; 	//��ʼ������netif��name�ֶ�
	netif->name[1]=IFNAME1; 	//���ļ��ⶨ�����ﲻ�ù��ľ���ֵ
	netif->output=etharp_output;//IP�㷢�����ݰ�����
	netif->linkoutput=low_level_output;//ARPģ�鷢�����ݰ�����
	low_level_init(netif); 		//�ײ�Ӳ����ʼ������
        etharp_init();
        sys_timeout(ARP_TMR_INTERVAL, arp_timer, NULL);


	return ERR_OK;
}











