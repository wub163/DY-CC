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
//ETH_IRQHandler 与 ethernetif_input 之间的同步信号
xSemaphoreHandle s_xSemaphore = NULL;


//创建 ethernetif_input 任务，处理接收到的数据
//数据接收 ---> ETH_IRQHandler -----s_xSemaphore---->ethernetif_input
//ethernetif_input 任务的优先级和堆栈
#define netifINTERFACE_TASK_STACK_SIZE		( 50 )
#define netifINTERFACE_TASK_PRIORITY		( configMAX_PRIORITIES - 1 )
/* The time to block waiting for input. */
#define emacBLOCK_TIME_WAITING_FOR_INPUT	( ( portTickType ) 100 )	//等待信号最长时间

#define netifGUARD_BLOCK_TIME				( 250 )			//等待信号最长时间

static void ethernetif_input( void * pvParameters );
//static void arp_timer(void *arg);


//由ethernetif_init()调用用于初始化硬件
//netif:网卡结构体指针 
//返回值:ERR_OK,正常
//       其他,失败
static err_t low_level_init(struct netif *netif)
{
#ifdef CHECKSUM_BY_HARDWARE
	int i; 
#endif 
	netif->hwaddr_len = ETHARP_HWADDR_LEN; //设置MAC地址长度,为6个字节
	//初始化MAC地址,设置什么地址由用户自己设置,但是不能与网络中其他设备MAC地址重复
	netif->hwaddr[0]=lwipdev.mac[0]; 
	netif->hwaddr[1]=lwipdev.mac[1]; 
	netif->hwaddr[2]=lwipdev.mac[2];
	netif->hwaddr[3]=lwipdev.mac[3];
	netif->hwaddr[4]=lwipdev.mac[4];
	netif->hwaddr[5]=lwipdev.mac[5];
	netif->mtu=1500; //最大允许传输单元,允许该网卡广播和ARP功能

	netif->flags = NETIF_FLAG_BROADCAST|NETIF_FLAG_ETHARP|NETIF_FLAG_LINK_UP;
	

  s_pxNetIf =netif;
 
  /* create binary semaphore used for informing ethernetif of frame reception */
  if (s_xSemaphore == NULL)
  {
    s_xSemaphore= xSemaphoreCreateCounting(20,0);
  }

	ETH_MACAddressConfig(ETH_MAC_Address0, netif->hwaddr); //向STM32F4的MAC地址寄存器中写入MAC地址
	ETH_DMATxDescChainInit(DMATxDscrTab, Tx_Buff, ETH_TXBUFNB);
	ETH_DMARxDescChainInit(DMARxDscrTab, Rx_Buff, ETH_RXBUFNB);

        
        
          /* Enable Ethernet Rx interrrupt */
  { 
    for(i=0; i<ETH_RXBUFNB; i++)
    {
      ETH_DMARxDescReceiveITConfig(&DMARxDscrTab[i], ENABLE);
    }
  }




#ifdef CHECKSUM_BY_HARDWARE 	//使用硬件帧校验
	for(i=0;i<ETH_TXBUFNB;i++)	//使能TCP,UDP和ICMP的发送帧校验,TCP,UDP和ICMP的接收帧校验在DMA中配置了
	{
		ETH_DMATxDescChecksumInsertionConfig(&DMATxDscrTab[i], ETH_DMATxDesc_ChecksumTCPUDPICMPFull);
	}
#endif

     xTaskCreate(ethernetif_input, (char*) "Eth_if", netifINTERFACE_TASK_STACK_SIZE, NULL,
              netifINTERFACE_TASK_PRIORITY,NULL);

   
    ETH_Start(); //开启MAC和DMA				
	return ERR_OK;
} 
//用于发送数据包的最底层函数(lwip通过netif->linkoutput指向该函数)
//netif:网卡结构体指针
//p:pbuf数据结构体指针
//返回值:ERR_OK,发送正常
//       ERR_MEM,发送失败
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


//网卡接收数据(lwip直接调用)
//netif:网卡结构体指针
//返回值:ERR_OK,发送正常
//       ERR_MEM,发送失败
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
//使用low_level_init()函数来初始化网络
//netif:网卡结构体指针
//返回值:ERR_OK,正常
//       其他,失败
err_t ethernetif_init(struct netif *netif)
{
	LWIP_ASSERT("netif!=NULL",(netif!=NULL));
#if LWIP_NETIF_HOSTNAME			//LWIP_NETIF_HOSTNAME 
	netif->hostname="lwip";  	//初始化名称
#endif 
	netif->name[0]=IFNAME0; 	//初始化变量netif的name字段
	netif->name[1]=IFNAME1; 	//在文件外定义这里不用关心具体值
	netif->output=etharp_output;//IP层发送数据包函数
	netif->linkoutput=low_level_output;//ARP模块发送数据包函数
	low_level_init(netif); 		//底层硬件初始化函数
        etharp_init();
        sys_timeout(ARP_TMR_INTERVAL, arp_timer, NULL);


	return ERR_OK;
}











