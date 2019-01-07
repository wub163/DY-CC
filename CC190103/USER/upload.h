#include "stm32f4xx.h"
#include "queue.h"
#define SENTHEAD 0X66
#define SENTEND  0X88
#define RECVHEAD 0X55
#define RECVEND  0XAA

#pragma   pack(1)
typedef struct _ID_ST{                        //
        u32 id;
        
        u8  com;                                //��У����
        u8  val;                                //��β                    ��0XAA��
}ID_D;



typedef struct _SEND_ST{                        //28+4=32�ֽ�
        u8  start;                              //��ͷ                    ��0X55��
        u8  len;                            //���������ݲ���)          ()
        
        s32 drvlen;                                //��ʻ���������mm��
        u32 rfid;                               //��ǰ����
        u32 rfidnext;                           //��һ����
        s16 sysdl;                              //ϵͳ����0.1A
        s16 sysdy;                              //ϵͳ��ѹ0.01V
        u16 magsbit;                            //������λλͼ
        s8  contemp;                            //�������¶�
        s8  drvtemp;                            //����¶�
        u8  sysfree;                              //ϵͳ����
        s8  ccspeed;                             //��ǰ�ٶ�-10---+10
        s8  ccturn;                             //��ǰ��λ�ǡ�-90--+90
        u8  high;                               //��ݸ߶�.  255:��ʾ�����̡���λcm
        u8  ccerr;                               //������Ϣ���ֱ�|̤��|վ��|������|������
        u8  ccauto;                              //�泵����Ȩ
        u8  funcount;                           //��ǰ�������б�š�
        u8  funid;                              //��ǰ�������������ڱ�š�
        
        
        u8  sum;                                //��У����
        u8  end;                                //��β                    ��0XAA��
}SEND_D;

typedef struct _RECV_ST{                        //26+4=30�ֽ�
        u8  start;                              //��ͷ                    ��0X55��
        u8  datalen;                            //���������ݲ���)          (76)
        
        u8  function;                           //������                   (0X73)���ȷ�ʽ
        u8  funcount;                           //��ǰ���б��
        ID_D id[4];                             //�����
        
        u8  sum;                                //��У����
        u8  end;                                //��β                    ��0XAA��
}RECV_D;




typedef struct _DRV_ST{
        s32 drv_pos[2];                         //������λ��
        s16 drv_spe[2];                         //�������ٶ�
        s16 moto_dy[2];                         //�����ֵ�ѹ
        u8  moto_zt[2];                         //��̨���״̬
        u8  moto_dl[2];                         //��̨�������
}xDRV_D;


typedef struct _CSB_ST{
        u8  csb[8];                             //ǰ��ǰ�ң����󣬺��ң���ǰ�������ǰ���Һ������������
}xCSB_D;

typedef struct _ZHT_ST{
        float dmp[4];                           //��Ԫ��
        float alj[3];                           //ŷ���ǲ���
        s16 jasd[3];                            //���ٶȲ���
        s16 jaosd[3];                           //���ٶȲ���
        s16 clj[3];                             //�����Ʋ���
        s16 sys_wd;                             //ϵͳ�¶�
        s16 sys_qy;                             //ϵͳ��ѹ
}xZHT_D;

extern QueueHandle_t xDRV,xSYS,xCSB,xZHT,xYAK,xREV,xCON;
extern  SEND_D xSentData;
extern RECV_D xComData;
//void upload_init(void);
 void upload(void);
u8 jiema(s8 n,RECV_D* com_data);