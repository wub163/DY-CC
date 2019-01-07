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
	int                     s;						/*socket ��ʶ�� -1��Ч��>= 0 ��Ч*/
	int                     bconnect;				/*socket �Ƿ����ӳɹ���0 δ���ӣ�>= 1 ����*/
	xSemaphoreHandle        sxMutex;                                /*��������socket�����̰߳�ȫ�ģ�Ϊ��ʵ��socket
									��һ���߳���������ݣ����������̷߳��ͣ�
									���������������������*/
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
	//����������
//	xClientSocket.sxMutex = xSemaphoreCreateMutex();
	
//	if(xClientSocket.sxMutex == NULL )
//	{
//		vTaskDelete(NULL);       //����������ʧ�ܡ�
//	}
	while(1)
	{
		//�����ͻ���
		xClientSocket.s = socket( AF_INET,SOCK_STREAM,IPPROTO_TCP);        //ʹ��IPV4 ,TCP Э�飬tcpЭ���塣
		if(xClientSocket.s == -1 ){
			continue;
		}
		//�����ɹ�
//		optval = 1;
//		
//		setsockopt(xClientSocket.s,SOL_SOCKET,SO_KEEPALIVE,&optval,sizeof(optval));
		ServerAddr.sin_family = AF_INET;
		ServerAddr.sin_addr.s_addr = inet_addr("192.168.1.100");//inet_addr("122.224.200.89");//
		ServerAddr.sin_port = htons(9000);
		
		xClientSocket.bconnect = 0;
		//���ӷ�����
		if( connect(xClientSocket.s,(struct sockaddr*)&ServerAddr,sizeof(ServerAddr) ) == - 1)
		{
			//connect error!
			close(xClientSocket.s);
			continue;
		}
		xClientSocket.bconnect = 1;
		
		//�Ƿ���յ�����
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
				//�����ݣ��������
				 xSemaphoreTake( xClientSocket.sxMutex, portMAX_DELAY ) ;

				datalen = recv(xClientSocket.s,ClientRevBuf,BUF_SIZE,0);
				if(datalen > 0)
				{
					//���յ�����,���ԣ����͸�������
					ret = send(xClientSocket.s,ClientRevBuf,datalen,0);
					ret =ret;
				}else{
					//�������رյ��쳣
					xClientSocket.bconnect = 0 ;
					close(xClientSocket.s);
 					xSemaphoreGive( xClientSocket.sxMutex );
					break;	//�������ӷ�����	
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
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);//��ָ������������ַ�����������Ҫ��
    servaddr.sin_port = htons(SERV_PORT);
    
    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    listen(listenfd, 2);
loop:
    while (1) {
        cliaddr_len = sizeof(cliaddr);
        connfd = accept(listenfd,(struct sockaddr *)&cliaddr, &cliaddr_len);//�ȴ�����һ���������ӡ�
         struct timeval tv;
        fd_set fdsr;
       FD_ZERO(&fdsr);
        FD_SET(connfd,&fdsr);//�����趨���׽��ַ����¼���select�������������ж���ʱ�˳���
        tv.tv_sec = 1;          //ÿ��һ�ε�����������
	tv.tv_usec = 000 ;//20msһ����������
	
        do{
          while ( select(4,&fdsr,NULL,0,&tv) == 0 ){write(connfd, "58585858", 8);FD_SET(connfd,&fdsr);}//�ڵȴ��У������űꡣ

        if(read(connfd, readbuf, RECV)<=0){close(connfd);goto loop;}
        }while((*(int*)(&readbuf))!=0x58585858);                                //������
        
         tv.tv_sec = 0;          //ÿ��һ�ε�����������
	tv.tv_usec = 50000 ;//50msһ����������
       
        
        while(1){
          
          while ( select(4,&fdsr,NULL,0,&tv) == 0 )
//=======================================================================
          { 
            upload();
            write(connfd, &xSentData, sizeof(SEND_D));            //�ϴ�״̬(writebuf��SEND��)
          
          FD_SET(connfd,&fdsr);}//ÿ��һ�ε�����������
       
//========================================================================         
          n = read(connfd, readbuf, RECV);
          if(n>0){jiema(n,(RECV_D*)&readbuf);}              //Զ�����ݽ��յ㣨readbuf��RECV����
        else {close(connfd);break;}
        
        
        
        }}
}