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

//__lwip_dev lwipdev;						//lwip控制结构体 
struct netif lwip_netif;				//定义一个全局的网络接口

extern u32 memp_get_memorysize(void);	//在memp.c里面定义
extern u8_t *memp_memory;				//在memp.c里面定义.
extern u8_t *ram_heap;					//在mem.c里面定义.

u32 TCPTimer=0;			//TCP查询计时器
u32 ARPTimer=0;			//ARP查询计时器
u32 lwip_localtime;		//lwip本地时间计数器,单位:ms

#if LWIP_DHCP
u32 DHCPfineTimer=0;	//DHCP精细处理计时器
u32 DHCPcoarseTimer=0;	//DHCP粗糙处理计时器
#endif


 __lwip_dev lwipdev={
0,0,0,0,0,0,            //MAC地址
REMOTO_IP               //远端主机IP
LOCALE_IP               //本机IP
REMARK_IP               //网络掩码
GATEWA_IP               //网关IP
0                       //DHCP状态
};

//lwip中mem和memp内存释放
void lwip_comm_mem_free(void)
{ 	
	vPortFree(memp_memory);
	vPortFree(ram_heap);
}
//lwip中mem和memp的内存申请
//返回值:0,成功;
//    其他,失败
u8 lwip_comm_mem_malloc(void)
{
	u32 mempsize;
	u32 ramheapsize; 
	mempsize=memp_get_memorysize();			//得到memp_memory数组大小
	memp_memory=pvPortMalloc(mempsize);	//为memp_memory申请内存
	ramheapsize=LWIP_MEM_ALIGN_SIZE(MEM_SIZE)+2*LWIP_MEM_ALIGN_SIZE(4*3)+MEM_ALIGNMENT;//得到ram heap大小
	ram_heap=pvPortMalloc(ramheapsize);	//为ram_heap申请内存 
	if(!memp_memory||!ram_heap)//有申请失败的
	{
		lwip_comm_mem_free();
		return 1;
	}
	return 0;	
}
//lwip 默认IP设置
//lwipx:lwip控制结构体指针
void lwip_comm_default_ip_set(__lwip_dev *lwipx)
{
	u32 sn0;
	sn0=*(vu32*)(0x1FFF7A10);//获取STM32的唯一ID的前24位作为MAC地址后三字节
	//默认远端IP为:192.168.1.100
//	lwipx->remoteip[0]=192;	
//	lwipx->remoteip[1]=168;
//	lwipx->remoteip[2]=1;
//	lwipx->remoteip[3]=104;
	//MAC地址设置(高三字节固定为:2.0.0,低三字节用STM32唯一ID)
	lwipx->mac[0]=2;//高三字节(IEEE称之为组织唯一ID,OUI)地址固定为:2.0.0
	lwipx->mac[1]=0;
	lwipx->mac[2]=0;
	lwipx->mac[3]=(sn0>>16)&0XFF;//低三字节用STM32的唯一ID
	lwipx->mac[4]=(sn0>>8)&0XFFF;;
	lwipx->mac[5]=sn0&0XFF; 
//	//默认本地IP为:192.168.1.30
//	lwipx->ip[0]=192;	
//	lwipx->ip[1]=168;
//	lwipx->ip[2]=1;
//	lwipx->ip[3]=30;
//	//默认子网掩码:255.255.255.0
//	lwipx->netmask[0]=255;	
//	lwipx->netmask[1]=255;
//	lwipx->netmask[2]=255;
//	lwipx->netmask[3]=0;
//	//默认网关:192.168.1.1
//	lwipx->gateway[0]=192;	
//	lwipx->gateway[1]=168;
//	lwipx->gateway[2]=1;
//	lwipx->gateway[3]=1;	
//	lwipx->dhcpstatus=0;//没有DHCP	
} 

//LWIP初始化(LWIP启动的时候使用)
//返回值:0,成功
//      1,内存错误
//      2,LAN8720初始化失败
//      3,网卡添加失败.
void LwIP_DHCP_task(void * pvParameters);

u8 lwip_comm_init(void)
{
	struct netif *Netif_Init_Flag;		//调用netif_add()函数时的返回值,用于判断网络初始化是否成功
	struct ip_addr ipaddr;  			//ip地址
	struct ip_addr netmask; 			//子网掩码
	struct ip_addr gw;      			//默认网关 
	if(ETH_Mem_Malloc())return 1;		//内存申请失败(底层)
	if(lwip_comm_mem_malloc())return 1;	//内存申请失败（协议栈）
	if(LAN8720_Init())return 2;			//初始化LAN8720失败 
        tcpip_init( NULL, NULL );
	lwip_comm_default_ip_set(&lwipdev);	//设置默认IP等信息

#if LWIP_DHCP		//使用动态IP
	ipaddr.addr = 0;
	netmask.addr = 0;
	gw.addr = 0;
#else				//使用静态IP
	IP4_ADDR(&ipaddr,lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);
	IP4_ADDR(&netmask,lwipdev.netmask[0],lwipdev.netmask[1] ,lwipdev.netmask[2],lwipdev.netmask[3]);
	IP4_ADDR(&gw,lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);
#endif
	Netif_Init_Flag=netif_add(&lwip_netif,&ipaddr,&netmask,&gw,NULL,&ethernetif_init,&tcpip_input);//向网卡列表中添加一个网口,并且生成Eth_IF任务。

#if LWIP_DHCP			//如果使用DHCP的话
        sys_thread_new("DHCP task", LwIP_DHCP_task, NULL, DHCP_THREAD_STACKSIZE, DHCP_THREAD_PRIO);
#else				//使用静态IP
   	char dispBuff[100];
        sprintf(dispBuff,"IP: %3d.%3d.%3d.%3d ",*(u8*)&lwip_netif.ip_addr.addr,*((u8*)(&lwip_netif.ip_addr.addr)+1),*((u8*)(&lwip_netif.ip_addr.addr)+2),*((u8*)(&lwip_netif.ip_addr.addr)+3));
        LCD_DisplayStringLine(LINE(4),200,(uint8_t* )dispBuff);
//        xTaskCreate( TCPClient, "TCPClient",    220,     ( void * ) NULL,        4,      NULL );
        xTaskCreate(TCPSeaver , "TCPSeaver",    220,     ( void * ) NULL,        4,      NULL );

#endif
	
	if(Netif_Init_Flag==NULL)return 3;//网卡添加失败 
	else//网口添加成功后,设置netif为默认值,并且打开netif网口
	{
		netif_set_default(&lwip_netif); //设置netif为默认网口
		netif_set_up(&lwip_netif);		//打开netif网口
	}
	return 0;//操作OK.
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
        //uxTaskGetStackHighWaterMark( NULL );//剩余126字节。
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




//以太网接收中断处理函数，发送信号，由任务 ethernetif_input (ethernetif.c)来处理接收帧
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















