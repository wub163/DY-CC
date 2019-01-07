#include "lan.h" 
#include "malloc.h"
#include "string.h" 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//TCP Client ���Դ���	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/8/15
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   
 
//TCP Client�������ݻ�����
u8      *tcp_client_recvbuf[4];
u16     *lase_data;
//TCP������������������
const u8 *tcp_client_sendbuf="MD0000076000000\r\n";

//TCP Client ����ȫ��״̬��Ǳ���
//bit7:0,û������Ҫ����;1,������Ҫ����
//bit6:0,û���յ�����;1,�յ�������.
//bit5:0,û�������Ϸ�����;1,�����Ϸ�������.
//bit4~0:����
u8 tcp_client_flag;
//const u8 TCPIP[]={192,168,1,220};
//#define	TCP_CLIENT_PORT			8888	//����tcp clientҪ���ӵ�Զ�˶˿�
const u8 TCPIP[]={192,168,1,250};
#define	TCP_CLIENT_PORT			10940	//����tcp clientҪ���ӵ�Զ�˶˿�

//����Զ��IP��ַ
void tcp_client_set_remoteip(void)
{
	lwipdev.remoteip[0]=TCPIP[0];
	lwipdev.remoteip[1]=TCPIP[1];
	lwipdev.remoteip[2]=TCPIP[2]; 
        lwipdev.remoteip[3]=TCPIP[3]; 
}
 

//TCP Client ����
void tcp_client_test(void)
{
 	struct tcp_pcb *tcppcb;  	//����һ��TCP���������ƿ�
	struct ip_addr rmtipaddr;  	//Զ��ip��ַ
	
	u8 res=0;		
	u16 t=0; 
	u8 connflag=0;		//���ӱ��
	
	tcp_client_set_remoteip();//��ѡ��IP
	tcppcb=tcp_new();	//����һ���µ�pcb
	if(tcppcb)			//�����ɹ�
	{
		IP4_ADDR(&rmtipaddr,lwipdev.remoteip[0],lwipdev.remoteip[1],lwipdev.remoteip[2],lwipdev.remoteip[3]); 
		tcp_connect(tcppcb,&rmtipaddr,TCP_CLIENT_PORT,tcp_client_connected);  //���ӵ�Ŀ�ĵ�ַ��ָ���˿���,�����ӳɹ���ص�tcp_client_connected()����
 	}else res=1;
	while(res==0)
	{

          if(keyup)//KEY0������,��������
		{
                        if(tcp_client_recvbuf[3]==NULL){
                          
                          tcp_client_recvbuf[0]=mymalloc(SRAMCCM,TCP_CLIENT_RX_BUFSIZE);
                          tcp_client_recvbuf[1]=mymalloc(SRAMCCM,TCP_CLIENT_RX_BUFSIZE);
                          tcp_client_recvbuf[2]=mymalloc(SRAMCCM,TCP_CLIENT_RX_BUFSIZE);
                          tcp_client_recvbuf[3]=mymalloc(SRAMCCM,TCP_CLIENT_RX_BUFSIZE);
			
                          if(tcp_client_recvbuf[3]==NULL)
                          {
                          
                          myfree(SRAMCCM,tcp_client_recvbuf[0]);
                          myfree(SRAMCCM,tcp_client_recvbuf[1]);
                          myfree(SRAMCCM,tcp_client_recvbuf[2]);
                          myfree(SRAMCCM,tcp_client_recvbuf[3]);
                          }
                          else tcp_client_flag|=1<<7;//�����״
                        }
                        else tcp_client_flag|=1<<7;//�����״
		}
          if(key0)//KEY0������,��������
		{
			tcp_client_flag&=!(1<<5);//�����״
		}
          
	if(tcp_client_flag&1<<6)//�Ƿ��յ�����?
		{
                  
//                  ((*(u16*)tcp_client_recvbuf))
                  
                  


                  tcp_client_flag&=~(1<<6);//��������Ѿ���������.
                  return;      

		}
		if(tcp_client_flag&1<<5)//�Ƿ�������?
			connflag=1;//���������
		else
			connflag=0;	//������ӶϿ���
		
		lwip_periodic_handle();
		t++;
		if(t==20000)
		{
			if(connflag==0&&(tcp_client_flag&1<<5)==0)//δ������,��������
			{ 
				tcp_client_connection_close(tcppcb,0);//�ر�����
				tcppcb=tcp_new();	//����һ���µ�pcb
				if(tcppcb)			//�����ɹ�
				{ 
					tcp_connect(tcppcb,&rmtipaddr,TCP_CLIENT_PORT,tcp_client_connected);//���ӵ�Ŀ�ĵ�ַ��ָ���˿���,�����ӳɹ���ص�tcp_client_connected()����
				}
			}
			t=0;
		}		
	}
	tcp_client_connection_close(tcppcb,0);//�ر�TCP Client����
} 
//lwIP TCP���ӽ�������ûص�����
err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
	struct tcp_client_struct *es=NULL;  
	if(err==ERR_OK)   
	{
		es=(struct tcp_client_struct*)mem_malloc(sizeof(struct tcp_client_struct));  //�����ڴ�
		if(es) //�ڴ�����ɹ�
		{
 			es->state=ES_TCPCLIENT_CONNECTED;//״̬Ϊ���ӳɹ�
			es->pcb=tpcb;  
			es->p=NULL; 
			tcp_arg(tpcb,es);        			//ʹ��es����tpcb��callback_arg
			tcp_recv(tpcb,tcp_client_recv);  	//��ʼ��LwIP��tcp_recv�ص�����   
			tcp_err(tpcb,tcp_client_error); 	//��ʼ��tcp_err()�ص�����
			tcp_sent(tpcb,tcp_client_sent);		//��ʼ��LwIP��tcp_sent�ص�����
			tcp_poll(tpcb,tcp_client_poll,1); 	//��ʼ��LwIP��tcp_poll�ص����� 
 			tcp_client_flag|=1<<5; 				//������ӵ���������
			err=ERR_OK;
		}else
		{ 
			tcp_client_connection_close(tpcb,es);//�ر�����
			err=ERR_MEM;	//�����ڴ�������
		}
	}else
	{
		tcp_client_connection_close(tpcb,0);//�ر�����
	}
	return err;
}
//lwIP tcp_recv()�����Ļص�����
err_t tcp_client_recv(void *arg,struct tcp_pcb *tpcb,struct pbuf *p,err_t err)
{ 
	u8 buf=0;
        u32 data_len=0;
	struct pbuf *q;
	struct tcp_client_struct *es;
	err_t ret_err; 
	LWIP_ASSERT("arg != NULL",arg != NULL);
	es=(struct tcp_client_struct *)arg; 
	if(p==NULL)//����ӷ��������յ��յ�����֡�͹ر�����
	{
		es->state=ES_TCPCLIENT_CLOSING;//��Ҫ�ر�TCP ������ 
 		es->p=p; 
		ret_err=ERR_OK;
	}else if(err!= ERR_OK)//�����յ�һ���ǿյ�����֡,����err!=ERR_OK
	{ 
		if(p)pbuf_free(p);//�ͷŽ���pbuf
		ret_err=err;
	}else if(es->state==ES_TCPCLIENT_CONNECTED)	//����������״̬ʱ
	{
		if(p!=NULL)//����������״̬���ҽ��յ������ݲ�Ϊ��ʱ
		{

                      for( buf = 0; buf < 4; ++buf)
                           {
                             if(*(u16*)tcp_client_recvbuf[buf]==0)break;
                           }
                      if(buf<4){
                  memset(tcp_client_recvbuf[buf],0,TCP_CLIENT_RX_BUFSIZE);  //���ݽ��ջ���������
			for(q=p;q!=NULL;q=q->next)  //����������pbuf����
			{
				//�ж�Ҫ������TCP_CLIENT_RX_BUFSIZE�е������Ƿ����TCP_CLIENT_RX_BUFSIZE��ʣ��ռ䣬�������
				//�Ļ���ֻ����TCP_CLIENT_RX_BUFSIZE��ʣ�೤�ȵ����ݣ�����Ļ��Ϳ������е�����
				if(q->len > (TCP_CLIENT_RX_BUFSIZE-data_len-2)) memcpy(tcp_client_recvbuf[buf]+data_len+2,q->payload,(TCP_CLIENT_RX_BUFSIZE-data_len));//��������
				else memcpy(tcp_client_recvbuf[buf]+data_len+2,q->payload,q->len);
				data_len += q->len;  	
				if(data_len > TCP_CLIENT_RX_BUFSIZE+2) break; //����TCP�ͻ��˽�������,����	
			}
                        *(u16*)tcp_client_recvbuf[buf]=p->tot_len;
  			}
			tcp_client_flag|=1<<6;		//��ǽ��յ�������
                        tcp_recved(tpcb,p->tot_len);//���ڻ�ȡ��������,֪ͨLWIP���Ի�ȡ�������� //pcbӦ��
                        pbuf_free(p);  	//�ͷ��ڴ�
			ret_err=ERR_OK;
		}
	}else  //���յ����ݵ��������Ѿ��ر�,
	{ 
		tcp_recved(tpcb,p->tot_len);//���ڻ�ȡ��������,֪ͨLWIP���Ի�ȡ��������
		es->p=NULL;
		pbuf_free(p); //�ͷ��ڴ�
		ret_err=ERR_OK;
	}
	return ret_err; 
} 
//lwIP tcp_poll�Ļص�����
err_t tcp_client_poll(void *arg, struct tcp_pcb *tpcb)
{
	err_t ret_err;
	struct tcp_client_struct *es; 
	es=(struct tcp_client_struct*)arg;
	if(es!=NULL)  //���Ӵ��ڿ��п��Է�������
	{
		if(tcp_client_flag&(1<<7))	//�ж��Ƿ�������Ҫ���� 
		{
			es->p=pbuf_alloc(PBUF_TRANSPORT, strlen((char*)tcp_client_sendbuf),PBUF_POOL);	//�����ڴ� 
			pbuf_take(es->p,(char*)tcp_client_sendbuf,strlen((char*)tcp_client_sendbuf));	//��tcp_client_sentbuf[]�е����ݿ�����es->p_tx��
			tcp_client_senddata(tpcb,es);//��tcp_client_sentbuf[]���渴�Ƹ�pbuf�����ݷ��ͳ�ȥ
			tcp_client_flag&=~(1<<7);	//������ݷ��ͱ�־
			if(es->p)pbuf_free(es->p);	//�ͷ��ڴ�
		}else if(es->state==ES_TCPCLIENT_CLOSING)
		{ 
 			tcp_client_connection_close(tpcb,es);//�ر�TCP����
		} 
		ret_err=ERR_OK;
	}else
	{ 
		tcp_abort(tpcb);//��ֹ����,ɾ��pcb���ƿ�
		ret_err=ERR_ABRT;
	}
	return ret_err;
} 
//lwIP tcp_sent�Ļص�����(����Զ���������յ�ACK�źź�������)
err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
	struct tcp_client_struct *es;
	LWIP_UNUSED_ARG(len);
	es=(struct tcp_client_struct*)arg;
	if(es->p)tcp_client_senddata(tpcb,es);//��������
	return ERR_OK;
}
//�˺���������������
void tcp_client_senddata(struct tcp_pcb *tpcb, struct tcp_client_struct * es)
{
	struct pbuf *ptr; 
 	err_t wr_err=ERR_OK;
	while((wr_err==ERR_OK)&&es->p&&(es->p->len<=tcp_sndbuf(tpcb))) //��Ҫ���͵����ݼ��뵽���ͻ��������
	{
		ptr=es->p;
		wr_err=tcp_write(tpcb,ptr->payload,ptr->len,1);
		if(wr_err==ERR_OK)
		{  
			es->p=ptr->next;			//ָ����һ��pbuf
			if(es->p)pbuf_ref(es->p);	//pbuf��ref��һ
			pbuf_free(ptr);				//�ͷ�ptr 
		}else if(wr_err==ERR_MEM)es->p=ptr;
		tcp_output(tpcb);		//�����ͻ�������е������������ͳ�ȥ
	} 
} 
//�ر��������������
void tcp_client_connection_close(struct tcp_pcb *tpcb, struct tcp_client_struct * es)
{
	//�Ƴ��ص�
	tcp_abort(tpcb);//��ֹ����,ɾ��pcb���ƿ�
	tcp_arg(tpcb,NULL);  
	tcp_recv(tpcb,NULL);
	tcp_sent(tpcb,NULL);
	tcp_err(tpcb,NULL);
	tcp_poll(tpcb,NULL,0);  
	if(es)mem_free(es); 
	tcp_client_flag&=~(1<<5);//������ӶϿ���
}

//lwIP tcp_err�����Ļص�����
void tcp_client_error(void *arg,err_t err)
{  
//err=err;	//�������ǲ����κδ���
} 


void lasejm (void )
{
static u8 *lasetemp=NULL;
static u16 num=0;
u8 *tempbuf=NULL;
u8 *sou,*dec;
u8 count=0;
  tempbuf=mymalloc(SRAMCCM,70);
for(u8 i = 0; i < 4; ++i)
{

  if(*(u16*)tcp_client_recvbuf[i])
  {
  if(*(u16*)tcp_client_recvbuf[i]>50)
  {
  if(lasetemp==NULL)lasetemp=mymalloc(SRAMCCM,2400);
  sou=tcp_client_recvbuf[i]+2;
  dec=tempbuf;
  if(*(u16*)tcp_client_recvbuf[i]>1200)num=0;else num=1344;          //��ֹ�����ҡ�
  while(sou<(tcp_client_recvbuf[i]+*(u16*)tcp_client_recvbuf[i]))
  {
    while(*sou!=0x0a){*(dec++)=*(sou++);count++;}count--;
    if((count<67)&&(count>18)){memcpy((lasetemp+num),tempbuf,count);num+=count;}
    dec=tempbuf;count=0;sou++;
  }
  }
  *(u16*)tcp_client_recvbuf[i]=0;}
  if(num>2000)break;
}
  myfree(SRAMCCM,tempbuf);tempbuf=NULL;
if(num>2000){
if(lase_data==NULL)lase_data=mymalloc(SRAMCCM,762*2);
if(lase_data!=NULL){
  for(int i = 0; i < 761; ++i)
  {

 lase_data[760-i]=((u16)0|(((lasetemp[i*3]-0x30)&0x0f)<<12)|(((lasetemp[i*3+1]-0x30)&0x3f)<<6)|((lasetemp[i*3+2]-0x30)&0x3f));

  }
 
}
  myfree(SRAMCCM,lasetemp);lasetemp=NULL;num=0;}//�ڴ��ͷź����㣬��Ұָ�롣
}


void conn (void)
{
 	struct tcp_pcb *tcppcb;  	//����һ��TCP���������ƿ�
	struct ip_addr rmtipaddr;  	//Զ��ip��ַ

    
        tcppcb=tcp_new();	//����һ���µ�pcb
				if(tcppcb)			//�����ɹ�
				{ 
		IP4_ADDR(&rmtipaddr,192,168,1,220); 
		tcp_connect(tcppcb,&rmtipaddr,9000,tcp_client_connected);  //���ӵ�Ŀ�ĵ�ַ��ָ���˿���,�����ӳɹ���ص�tcp_client_connected()����
				}


}


















