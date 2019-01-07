#include "FreeRTOS.h"
#include "task.h"
#include "netconf.h" 
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/init.h"
#include "netif/ethernetif.h" 
#include "lwip/timer.h"
#include "lwip/tcp_impl.h"
#include "lwip/ip_frag.h"
#include "lwip/tcpip.h" 
#include <stdio.h>
#include "bsp_lcd.h"
#include "net.h"

//__lwip_dev lwipdev;						//lwip���ƽṹ�� 
struct netif lwip_netif;				//����һ��ȫ�ֵ�����ӿ�

extern u32 memp_get_memorysize(void);	//��memp.c���涨��
extern u8_t *memp_memory;				//��memp.c���涨��.
extern u8_t *ram_heap;					//��mem.c���涨��.

u32 TCPTimer=0;			//TCP��ѯ��ʱ��
u32 ARPTimer=0;			//ARP��ѯ��ʱ��
u32 lwip_localtime;		//lwip����ʱ�������,��λ:ms

#if LWIP_DHCP
u32 DHCPfineTimer=0;	//DHCP��ϸ�����ʱ��
u32 DHCPcoarseTimer=0;	//DHCP�ֲڴ����ʱ��
#endif


 __lwip_dev lwipdev={
0,0,0,0,0,0,            //MAC��ַ
REMOTO_IP               //Զ������IP
LOCALE_IP               //����IP
REMARK_IP               //��������
GATEWA_IP               //����IP
0                       //DHCP״̬
};

//lwip��mem��memp�ڴ��ͷ�
void lwip_comm_mem_free(void)
{ 	
	vPortFree(memp_memory);
	vPortFree(ram_heap);
}
//lwip��mem��memp���ڴ�����
//����ֵ:0,�ɹ�;
//    ����,ʧ��
u8 lwip_comm_mem_malloc(void)
{
	u32 mempsize;
	u32 ramheapsize; 
	mempsize=memp_get_memorysize();			//�õ�memp_memory�����С
	memp_memory=pvPortMalloc(mempsize);	//Ϊmemp_memory�����ڴ�
	ramheapsize=LWIP_MEM_ALIGN_SIZE(MEM_SIZE)+2*LWIP_MEM_ALIGN_SIZE(4*3)+MEM_ALIGNMENT;//�õ�ram heap��С
	ram_heap=pvPortMalloc(ramheapsize);	//Ϊram_heap�����ڴ� 
	if(!memp_memory||!ram_heap)//������ʧ�ܵ�
	{
		lwip_comm_mem_free();
		return 1;
	}
	return 0;	
}
//lwip Ĭ��IP����
//lwipx:lwip���ƽṹ��ָ��
void lwip_comm_default_ip_set(__lwip_dev *lwipx)
{
	u32 sn0;
	sn0=*(vu32*)(0x1FFF7A10);//��ȡSTM32��ΨһID��ǰ24λ��ΪMAC��ַ�����ֽ�
	//Ĭ��Զ��IPΪ:192.168.1.100
//	lwipx->remoteip[0]=192;	
//	lwipx->remoteip[1]=168;
//	lwipx->remoteip[2]=1;
//	lwipx->remoteip[3]=104;
	//MAC��ַ����(�����ֽڹ̶�Ϊ:2.0.0,�����ֽ���STM32ΨһID)
	lwipx->mac[0]=2;//�����ֽ�(IEEE��֮Ϊ��֯ΨһID,OUI)��ַ�̶�Ϊ:2.0.0
	lwipx->mac[1]=0;
	lwipx->mac[2]=0;
	lwipx->mac[3]=(sn0>>16)&0XFF;//�����ֽ���STM32��ΨһID
	lwipx->mac[4]=(sn0>>8)&0XFFF;;
	lwipx->mac[5]=sn0&0XFF; 
//	//Ĭ�ϱ���IPΪ:192.168.1.30
//	lwipx->ip[0]=192;	
//	lwipx->ip[1]=168;
//	lwipx->ip[2]=1;
//	lwipx->ip[3]=30;
//	//Ĭ����������:255.255.255.0
//	lwipx->netmask[0]=255;	
//	lwipx->netmask[1]=255;
//	lwipx->netmask[2]=255;
//	lwipx->netmask[3]=0;
//	//Ĭ������:192.168.1.1
//	lwipx->gateway[0]=192;	
//	lwipx->gateway[1]=168;
//	lwipx->gateway[2]=1;
//	lwipx->gateway[3]=1;	
//	lwipx->dhcpstatus=0;//û��DHCP	
} 

//LWIP��ʼ��(LWIP������ʱ��ʹ��)
//����ֵ:0,�ɹ�
//      1,�ڴ����
//      2,LAN8720��ʼ��ʧ��
//      3,�������ʧ��.
void LwIP_DHCP_task(void * pvParameters);

u8 lwip_comm_init(void)
{
	struct netif *Netif_Init_Flag;		//����netif_add()����ʱ�ķ���ֵ,�����ж������ʼ���Ƿ�ɹ�
	struct ip_addr ipaddr;  			//ip��ַ
	struct ip_addr netmask; 			//��������
	struct ip_addr gw;      			//Ĭ������ 
	if(ETH_Mem_Malloc())return 1;		//�ڴ�����ʧ��(�ײ�)
	if(lwip_comm_mem_malloc())return 1;	//�ڴ�����ʧ�ܣ�Э��ջ��
	if(LAN8720_Init())return 2;			//��ʼ��LAN8720ʧ�� 
        tcpip_init( NULL, NULL );
	lwip_comm_default_ip_set(&lwipdev);	//����Ĭ��IP����Ϣ

#if LWIP_DHCP		//ʹ�ö�̬IP
	ipaddr.addr = 0;
	netmask.addr = 0;
	gw.addr = 0;
#else				//ʹ�þ�̬IP
	IP4_ADDR(&ipaddr,lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
	IP4_ADDR(&netmask,lwipdev.netmask[0],lwipdev.netmask[1] ,lwipdev.netmask[2],lwipdev.netmask[3]);
	IP4_ADDR(&gw,lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
#endif
	Netif_Init_Flag=netif_add(&lwip_netif,&ipaddr,&netmask,&gw,NULL,&ethernetif_init,&tcpip_input);//�������б������һ������,��������Eth_IF����

#if LWIP_DHCP			//���ʹ��DHCP�Ļ�
        sys_thread_new("DHCP task", LwIP_DHCP_task, NULL, DHCP_THREAD_STACKSIZE, DHCP_THREAD_PRIO);
#else				//ʹ�þ�̬IP
   	char dispBuff[100];
        sprintf(dispBuff,"IP: %3d.%3d.%3d.%3d ",*(u8*)&lwip_netif.ip_addr.addr,*((u8*)(&lwip_netif.ip_addr.addr)+1),*((u8*)(&lwip_netif.ip_addr.addr)+2),*((u8*)(&lwip_netif.ip_addr.addr)+3));
        LCD_DisplayStringLine(LINE(4),200,(uint8_t* )dispBuff);
//        xTaskCreate( TCPClient, "TCPClient",    220,     ( void * ) NULL,        4,      NULL );
        xTaskCreate(TCPSeaver , "TCPSeaver",    220,     ( void * ) NULL,        4,      NULL );

#endif
	
	if(Netif_Init_Flag==NULL)return 3;//�������ʧ�� 
	else//������ӳɹ���,����netifΪĬ��ֵ,���Ҵ�netif����
	{
		netif_set_default(&lwip_netif); //����netifΪĬ������
		netif_set_up(&lwip_netif);		//��netif����
	}
	return 0;//����OK.
}   

/* Private typedef -----------------------------------------------------------*/
typedef enum
{
  DHCP_START=0,
  DHCP_WAIT_ADDRESS,
  DHCP_ADDRESS_ASSIGNED,
  DHCP_TIMEOUT
}
DHCP_State_TypeDef;
///* Private define ------------------------------------------------------------*/
//#define MAX_DHCP_TRIES 5
//
///* Private macro -------------------------------------------------------------*/
///* Private variables ---------------------------------------------------------*/
//struct netif xnetif; /* network interface structure */

#if LWIP_DHCP
/**
  * @brief  LwIP_DHCP_Process_Handle
  * @param  None
  * @retval None
  */
void LwIP_DHCP_task(void * pvParameters)
{
   	char dispBuff[100];


  struct ip_addr ipaddr;
  struct ip_addr netmask;
  struct ip_addr gw;
  uint32_t IPaddress;
  uint8_t DHCP_state;
  DHCP_state = DHCP_START;

  for (;;)
  {
    switch (DHCP_state)
    {
      case DHCP_START:
      {
        dhcp_start(&lwip_netif);
        IPaddress = 0;
        DHCP_state = DHCP_WAIT_ADDRESS;
      }
      break;

      case DHCP_WAIT_ADDRESS:
      {
        /* Read the new IP address */
        IPaddress = lwip_netif.ip_addr.addr;

        if (IPaddress!=0)
        {
          DHCP_state = DHCP_ADDRESS_ASSIGNED;

        }
        else
        {
          /* DHCP timeout */
          if (lwip_netif.dhcp->tries <=LWIP_MAX_DHCP_TRIES)  break;
            DHCP_state = DHCP_TIMEOUT;

            /* Static address used */
	IP4_ADDR(&ipaddr,lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
	IP4_ADDR(&netmask,lwipdev.netmask[0],lwipdev.netmask[1] ,lwipdev.netmask[2],lwipdev.netmask[3]);
	IP4_ADDR(&gw,lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
            netif_set_addr(&lwip_netif, &ipaddr , &netmask, &gw);

          
        }
        
          /* Stop DHCP */
          dhcp_stop(&lwip_netif);

        sprintf(dispBuff,"IP: %3d.%3d.%3d.%3d ",*(u8*)&lwip_netif.ip_addr.addr,*((u8*)(&lwip_netif.ip_addr.addr)+1),*((u8*)(&lwip_netif.ip_addr.addr)+2),*((u8*)(&lwip_netif.ip_addr.addr)+3));
        LCD_DisplayStringLine(LINE(4),200,(uint8_t* )dispBuff);

//        xTaskCreate( TCPClient, "TCPClient",    1220,     ( void * ) NULL,        4,      NULL );
        xTaskCreate(TCPSeaver , "TCPClient",    220,     ( void * ) NULL,        4,      NULL );
        //uxTaskGetStackHighWaterMark( NULL );//ʣ��126�ֽڡ�
          vTaskDelete(NULL);
        
        
      }
      break;

      default: break;
    }

    /* wait 250 ms */
    vTaskDelay(250);
  }
}
#endif  /* USE_DHCP */




//��̫�������жϴ������������źţ������� ethernetif_input (ethernetif.c)���������֡
extern xSemaphoreHandle s_xSemaphore;

/**
  * @brief  This function handles ethernet DMA interrupt request.
  * @param  None
  * @retval None
  */
void ETH_IRQHandler(void)
{
  portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

  /* Frame received */
  if ( ETH_GetDMAFlagStatus(ETH_DMA_FLAG_R) == SET) 
  {
    /* Give the semaphore to wakeup LwIP task */
    xSemaphoreGiveFromISR( s_xSemaphore, &xHigherPriorityTaskWoken );   
  }
	
  /* Clear the interrupt flags. */
  /* Clear the Eth DMA Rx IT pending bits */
  ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
  ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);
	
  /* Switch tasks if necessary. */	
  if( xHigherPriorityTaskWoken != pdFALSE )
  {
    portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
  }
}















