//#include "FreeRTOS.h"
#include "lwip/lwip_sys.h"
//#include "task.h"
//#include "netconf.h"
#include "lwip/sockets.h"
#include "string.h"
#include "stm32f4xx.h"
#include "net.h"
#include "upload.h"
#include "set.h"
typedef struct _XTCPCLIENTSOCK{
	int                     s;						/*socket 标识符 -1无效，>= 0 有效*/
	int                     bconnect;				/*socket 是否连接成功，0 未连接，>= 1 连接*/
	xSemaphoreHandle        sxMutex;                                /*互斥量，socket不是线程安全的，为了实现socket
									在一个线程里接收数据，而在其他线程发送，
									建立互斥量来做互斥操作*/
}XTCPCLIENTSOCK;



#define RECV 40
#define SEND 82
//#define SERV_PORT 9527
    char readbuf[RECV];
    char writebuf[SEND];

void TCPClient(void *arg)
//void TCPClient(void )
{
XTCPCLIENTSOCK xClientSocket;
#define BUF_SIZE		1500
//char ClientRevBuf[BUF_SIZE];
  char ClientRevBuf[]={"{'op': 'advertise', 'topic': 'test', 'type': 'std_msgs/String'}"};

  struct sockaddr_in ServerAddr;
//	int optval = 1;
	fd_set fdsr;
	
//	(void)arg;
	xClientSocket.bconnect = 0;
	//建立互斥量
//	xClientSocket.sxMutex = xSemaphoreCreateMutex();
	
//	if(xClientSocket.sxMutex == NULL )
//	{
//		vTaskDelete(NULL);       //互斥量建立失败。
//	}
	while(1)
	{
		//创建客户端
		xClientSocket.s = socket( AF_INET,SOCK_STREAM,IPPROTO_TCP);        //使用IPV4 ,TCP 协议，tcp协议族。
		if(xClientSocket.s == -1 ){
			continue;
		}
		//创建成功
//		optval = 1;
//		
//		setsockopt(xClientSocket.s,SOL_SOCKET,SO_KEEPALIVE,&optval,sizeof(optval));
		ServerAddr.sin_family = AF_INET;
		ServerAddr.sin_addr.s_addr = inet_addr("192.168.1.100");//inet_addr("122.224.200.89");//
		ServerAddr.sin_port = htons(9000);
		
		xClientSocket.bconnect = 0;
		//连接服务器
		if( connect(xClientSocket.s,(struct sockaddr*)&ServerAddr,sizeof(ServerAddr) ) == - 1)
		{
			//connect error!
			close(xClientSocket.s);
			continue;
		}
		xClientSocket.bconnect = 1;
		
		//是否接收到数据
		while(1)
		{
			FD_ZERO(&fdsr);
			FD_SET(xClientSocket.s,&fdsr);
	                struct timeval tv;
			tv.tv_sec = 10;
			tv.tv_usec = 000 ;

			while ( select(xClientSocket.s + 1,&fdsr,NULL,0,&tv) == 0 );
				
			
			if( FD_ISSET(xClientSocket.s,& fdsr) )
			{
				int datalen;
				int ret;
				//有数据，互斥操作
				 xSemaphoreTake( xClientSocket.sxMutex, portMAX_DELAY ) ;

				datalen = recv(xClientSocket.s,ClientRevBuf,BUF_SIZE,0);
				if(datalen > 0)
				{
					//接收的数据,测试，回送给服务器
					ret = send(xClientSocket.s,ClientRevBuf,datalen,0);
					ret =ret;
				}else{
					//服务器关闭等异常
					xClientSocket.bconnect = 0 ;
					close(xClientSocket.s);
 					xSemaphoreGive( xClientSocket.sxMutex );
					break;	//重新连接服务器	
				}
                                send(xClientSocket.s,ClientRevBuf,64,0);
                                 vTaskDelay(1000/portTICK_PERIOD_MS);

				xSemaphoreGive( xClientSocket.sxMutex );
			}			
		}	
	}
}



//int main(int argc, char *argv[])
//{
//    struct sockaddr_in servaddr;
//    char buf[MAXLINE];
//    int sockfd, n;
//    char *str;
//    
//    if (argc != 2) {
//        fputs("usage: ./client message\n", stderr);
//        exit(1);
//    }
//    str = argv[1];
//    
//    sockfd = socket(AF_INET, SOCK_STREAM, 0);
//
//    bzero(&servaddr, sizeof(servaddr));
//    servaddr.sin_family = AF_INET;
//    servaddr.sin_addr.s_addr = inet_addr("192.168.1.220");//inet_addr("122.224.200.89");//
//    servaddr.sin_port = htons(SERV_PORT);
//    
//    connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
//
//    write(sockfd, str, strlen(str));
//
//    n = read(sockfd, buf, MAXLINE);
//    printf("Response from server:\n");
//    write(STDOUT_FILENO, buf, n);
//
//    close(sockfd);
//    return 0;
//}
//

void TCPSeaver(void *arg)
{
  
 
  struct sockaddr_in servaddr, cliaddr;
    socklen_t cliaddr_len;
    int listenfd, connfd;

    int n;
    listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset(&servaddr,0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//不指定具体网卡地址，满足多网卡要求。
    servaddr.sin_port = htons(SERV_PORT);
    
    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    listen(listenfd, 2);
loop:
    while (1) {
        cliaddr_len = sizeof(cliaddr);
        connfd = accept(listenfd,(struct sockaddr *)&cliaddr, &cliaddr_len);//等待接收一个具体链接。
         struct timeval tv;
        fd_set fdsr;
       FD_ZERO(&fdsr);
        FD_SET(connfd,&fdsr);//这里设定的套接字发生事件，select函数将会立刻中断延时退出。
        tv.tv_sec = 1;          //每秒一次的心跳反馈。
	tv.tv_usec = 000 ;//20ms一次心跳反馈
	
        do{
          while ( select(4,&fdsr,NULL,0,&tv) == 0 ){write(connfd, "58585858", 8);FD_SET(connfd,&fdsr);}//在等待中，发送信标。

        if(read(connfd, readbuf, RECV)<=0){close(connfd);goto loop;}
        }while((*(int*)(&readbuf))!=0x58585858);                                //口令码
        
         tv.tv_sec = 0;          //每秒一次的心跳反馈。
	tv.tv_usec = 50000 ;//50ms一次心跳反馈
       
        
        while(1){
          
          while ( select(4,&fdsr,NULL,0,&tv) == 0 )
//=======================================================================
          { 
            upload();
            write(connfd, &xSentData, sizeof(SEND_D));            //上传状态(writebuf【SEND】)
          
          FD_SET(connfd,&fdsr);}//每秒一次的心跳反馈。
       
//========================================================================         
          n = read(connfd, readbuf, RECV);
          if(n>0){jiema(n,(RECV_D*)&readbuf);}              //远程数据接收点（readbuf【RECV】）
        else {close(connfd);break;}
        
        
        
        }}
}