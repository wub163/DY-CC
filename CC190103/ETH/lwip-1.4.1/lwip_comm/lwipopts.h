/**
	LWIP�����ļ�,ͨ�����ļ�����ѡ��LWIP�Ĺ���,
	���ļ�����opt.h�ļ�,opt.h��LWIP"����"ʱԭװ���ļ�,��Adam���˱�д
	��lwipopt.h��������ST��,��������lwip�г�����Ҫ�Ķ��ĵط�
	�����ST�����е�����ѡ����ӽ�����,�������ļ���߶������˵�
	ѡ���Ժ���Ϊ׼,���ǹ��е�ѡ�������Ǹ��Ե�����Ϊ׼
*/

#include "set.h"


#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

/**
 * SYS_LIGHTWEIGHT_PROT==1: if you want inter-task protection for certain
 * critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */
#define SYS_LIGHTWEIGHT_PROT    0

#define ETHARP_TRUST_IP_MAC     0
#define IP_REASSEMBLY           0
#define IP_FRAG                 0
#define ARP_QUEUEING            1//0
#define TCP_LISTEN_BACKLOG      1

#define LWIP_ALLOW_MEM_FREE_FROM_OTHER_CONTEXT 1

//NO_SYS==1:��ʹ�ò���ϵͳ
#define NO_SYS                  0  //��ʹ��UCOS����ϵͳ

//ʹ��4�ֽڶ���ģʽ
#define MEM_ALIGNMENT           4  

//MEM_SIZE:heap�ڴ�Ĵ�С,�����Ӧ�����д������ݷ��͵Ļ����ֵ������ô�һ�� 
#define MEM_SIZE                8000 //�ڴ�Ѵ�С

//MEMP_NUM_PBUF:memp�ṹ��pbuf����,���Ӧ�ô�ROM���߾�̬�洢�����ʹ�������ʱ,���ֵӦ�����ô�һ��
#define MEMP_NUM_PBUF           10

//MEMP_NUM_UDP_PCB:UDPЭ����ƿ�(PCB)����.ÿ�����UDP"����"��Ҫһ��PCB.
#define MEMP_NUM_UDP_PCB        6

//MEMP_NUM_TCP_PCB:ͬʱ���������TCP����
#define MEMP_NUM_TCP_PCB        10

//MEMP_NUM_TCP_PCB_LISTEN:�ܹ�������TCP��������
#define MEMP_NUM_TCP_PCB_LISTEN 6

//MEMP_NUM_TCP_SEG:���ͬʱ�ڶ����е�TCP������
#define MEMP_NUM_TCP_SEG        15

//MEMP_NUM_SYS_TIMEOUT:�ܹ�ͬʱ�����timeout����
#define MEMP_NUM_SYS_TIMEOUT    8


/* ---------- Pbufѡ��---------- */
//PBUF_POOL_SIZE:pbuf�ڴ�ظ���. 
#define PBUF_POOL_SIZE          10

//PBUF_POOL_BUFSIZE:ÿ��pbuf�ڴ�ش�С. 
#define PBUF_POOL_BUFSIZE       512


/* ---------- TCPѡ��---------- */
#define LWIP_TCP                1  //Ϊ1��ʹ��TCP
#define TCP_TTL                 255//����ʱ��

/*��TCP�����ݶγ�������ʱ�Ŀ���λ,���豸���ڴ��С��ʱ�����ӦΪ0*/
#define TCP_QUEUE_OOSEQ         0

//���TCP�ֶ�
#define TCP_MSS                 (1500 - 40)	  //TCP_MSS = (MTU - IP��ͷ��С - TCP��ͷ��С

//TCP���ͻ�������С(bytes).
#define TCP_SND_BUF             (4*TCP_MSS)

//TCP_SND_QUEUELEN: TCP���ͻ�������С(pbuf).���ֵ��СΪ(2 * TCP_SND_BUF/TCP_MSS) */
#define TCP_SND_QUEUELEN        (2* TCP_SND_BUF/TCP_MSS)

//TCP���ʹ���
#define TCP_WND                 (2*TCP_MSS)


/* ---------- ICMPѡ��---------- */
#define LWIP_ICMP                 1 //ʹ��ICMPЭ��


/* ---------- DHCPѡ��---------- */
//��ʹ��DHCPʱ��λӦ��Ϊ1,LwIP 0.5.1�汾��û��DHCP����.
///////#define LWIP_DHCP               0

/* ---------- UDPѡ�� ---------- */ 
#define LWIP_UDP                1 //ʹ��UDP����
#define UDP_TTL                 255 //UDP���ݰ�����ʱ��


/* ---------- Statistics options ---------- */
#define LWIP_STATS 0
#define LWIP_PROVIDE_ERRNO 1


/*
   --------------------------------------
   ---------- ֡У���ѡ��----------
   --------------------------------------
*/

//STM32F4x7����ͨ��Ӳ��ʶ��ͼ���IP,UDP��ICMP��֡У���

#define CHECKSUM_BY_HARDWARE //����CHECKSUM_BY_HARDWARE,ʹ��Ӳ��֡У��


#ifdef CHECKSUM_BY_HARDWARE
  //CHECKSUM_GEN_IP==0: Ӳ������IP���ݰ���֡У���
  #define CHECKSUM_GEN_IP                 0
  //CHECKSUM_GEN_UDP==0: Ӳ������UDP���ݰ���֡У���
  #define CHECKSUM_GEN_UDP                0
  //CHECKSUM_GEN_TCP==0: Ӳ������TCP���ݰ���֡У���
  #define CHECKSUM_GEN_TCP                0 
  //CHECKSUM_CHECK_IP==0: Ӳ����������IP���ݰ�֡У���
  #define CHECKSUM_CHECK_IP               0
  //CHECKSUM_CHECK_UDP==0: Ӳ����������UDP���ݰ�֡У���
  #define CHECKSUM_CHECK_UDP              0
  //CHECKSUM_CHECK_TCP==0: Ӳ����������TCP���ݰ�֡У���
  #define CHECKSUM_CHECK_TCP              0
#else
  /* CHECKSUM_GEN_IP==1: Generate checksums in software for outgoing IP packets.*/
  #define CHECKSUM_GEN_IP                 1
  /* CHECKSUM_GEN_UDP==1: Generate checksums in software for outgoing UDP packets.*/
  #define CHECKSUM_GEN_UDP                1
  /* CHECKSUM_GEN_TCP==1: Generate checksums in software for outgoing TCP packets.*/
  #define CHECKSUM_GEN_TCP                1
  /* CHECKSUM_CHECK_IP==1: Check checksums in software for incoming IP packets.*/
  #define CHECKSUM_CHECK_IP               1
  /* CHECKSUM_CHECK_UDP==1: Check checksums in software for incoming UDP packets.*/
  #define CHECKSUM_CHECK_UDP              1
  /* CHECKSUM_CHECK_TCP==1: Check checksums in software for incoming TCP packets.*/
  #define CHECKSUM_CHECK_TCP              1
#endif


/*
   ----------------------------------------------
   ---------- SequentialAPIѡ��----------
   ----------------------------------------------
*/

//LWIP_NETCONN==1:ʹ��NETCON����(Ҫ��ʹ��api_lib.c)
#define LWIP_NETCONN                    1

/*
   ------------------------------------
   ---------- Socket APIѡ��----------
   ------------------------------------
*/
//LWIP_SOCKET==1:ʹ��Sicket API(Ҫ��ʹ��sockets.c)
#define LWIP_SOCKET                    1


#define LWIP_COMPAT_MUTEX               1

#define LWIP_SO_RCVTIMEO                1 //ͨ������LWIP_SO_RCVTIMEOʹ��netconn�ṹ����recv_timeout,ʹ��recv_timeout���Ա��������߳�


/*
   ----------------------------------------
   ---------- Lwip����ѡ��----------
   ----------------------------------------
*/
#define LWIP_DEBUG                      0
#define LWIP_NOASSERT                   //��������ö��Թر�
#ifdef LWIP_DEBUG

#define LWIP_DBG_MIN_LEVEL         	0
#define ETHARP_DEBUG                    LWIP_DBG_OFF
#define PPP_DEBUG                  	LWIP_DBG_OFF
#define MEM_DEBUG                  	LWIP_DBG_OFF
#define MEMP_DEBUG                 	LWIP_DBG_OFF
#define PBUF_DEBUG                 	LWIP_DBG_OFF
#define API_LIB_DEBUG              	LWIP_DBG_OFF
#define API_MSG_DEBUG              	LWIP_DBG_OFF
#define TCPIP_DEBUG                	LWIP_DBG_OFF
#define NETIF_DEBUG                	LWIP_DBG_OFF
#define SOCKETS_DEBUG              	LWIP_DBG_OFF
#define DNS_DEBUG                  	LWIP_DBG_OFF
#define AUTOIP_DEBUG               	LWIP_DBG_OFF
#define DHCP_DEBUG                 	LWIP_DBG_OFF
#define IP_DEBUG                   	LWIP_DBG_OFF
#define IP_REASS_DEBUG             	LWIP_DBG_OFF
#define ICMP_DEBUG                 	LWIP_DBG_OFF
#define IGMP_DEBUG                 	LWIP_DBG_OFF
#define UDP_DEBUG                  	LWIP_DBG_OFF
#define TCP_DEBUG                  	LWIP_DBG_OFF
#define TCP_INPUT_DEBUG            	LWIP_DBG_OFF
#define TCP_OUTPUT_DEBUG           	LWIP_DBG_OFF
#define TCP_RTO_DEBUG              	LWIP_DBG_OFF
#define TCP_CWND_DEBUG             	LWIP_DBG_OFF
#define TCP_WND_DEBUG              	LWIP_DBG_OFF
#define TCP_FR_DEBUG               	LWIP_DBG_OFF
#define TCP_QLEN_DEBUG             	LWIP_DBG_OFF
#define TCP_RST_DEBUG              	LWIP_DBG_OFF
#endif

#define LWIP_DBG_TYPES_ON         (LWIP_DBG_ON|LWIP_DBG_TRACE|LWIP_DBG_STATE|LWIP_DBG_FRESH|LWIP_DBG_HALT)


/*
   ---------------------------------
   ---------- OS options ----------
   ---------------------------------
*/

#define TCPIP_THREAD_STACKSIZE          200
#define DHCP_THREAD_STACKSIZE           200
#define TCPIP_MBOX_SIZE                 5
#define DEFAULT_UDP_RECVMBOX_SIZE       2000
#define DEFAULT_TCP_RECVMBOX_SIZE       2000
#define DEFAULT_ACCEPTMBOX_SIZE         2000
#define DEFAULT_THREAD_STACKSIZE        500
#define TCPIP_THREAD_PRIO               (configMAX_PRIORITIES - 2)
#define DHCP_THREAD_PRIO               (configMAX_PRIORITIES - 3)



#endif /* __LWIPOPTS_H__ */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
