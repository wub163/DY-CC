#include "stm32f4xx.h"
#include "queue.h"
#define SENTHEAD 0X66
#define SENTEND  0X88
#define RECVHEAD 0X55
#define RECVEND  0XAA

#pragma   pack(1)
typedef struct _ID_ST{                        //
        u32 id;
        
        u8  com;                                //和校验码
        u8  val;                                //包尾                    （0XAA）
}ID_D;



typedef struct _SEND_ST{                        //28+4=32字节
        u8  start;                              //包头                    （0X55）
        u8  len;                            //包长（内容部分)          ()
        
        s32 drvlen;                                //行驶里程数。（mm）
        u32 rfid;                               //当前卡号
        u32 rfidnext;                           //下一卡号
        s16 sysdl;                              //系统电流0.1A
        s16 sysdy;                              //系统电压0.01V
        u16 magsbit;                            //磁条点位位图
        s8  contemp;                            //控制器温度
        s8  drvtemp;                            //电机温度
        u8  sysfree;                              //系统空闲
        s8  ccspeed;                             //当前速度-10---+10
        s8  ccturn;                             //当前舵位角。-90--+90
        u8  high;                               //插齿高度.  255:表示超量程。单位cm
        u8  ccerr;                               //错误信息。手柄|踏板|站人|左腰靠|右腰靠
        u8  ccauto;                              //叉车控制权
        u8  funcount;                           //当前命令序列编号。
        u8  funid;                              //当前运行命令序列内编号。
        
        
        u8  sum;                                //和校验码
        u8  end;                                //包尾                    （0XAA）
}SEND_D;

typedef struct _RECV_ST{                        //26+4=30字节
        u8  start;                              //包头                    （0X55）
        u8  datalen;                            //包长（内容部分)          (76)
        
        u8  function;                           //功能码                   (0X73)调度方式
        u8  funcount;                           //当前队列编号
        ID_D id[4];                             //命令段
        
        u8  sum;                                //和校验码
        u8  end;                                //包尾                    （0XAA）
}RECV_D;




typedef struct _DRV_ST{
        s32 drv_pos[2];                         //左右轮位置
        s16 drv_spe[2];                         //左右轮速度
        s16 moto_dy[2];                         //左右轮电压
        u8  moto_zt[2];                         //四台电机状态
        u8  moto_dl[2];                         //四台电机电流
}xDRV_D;


typedef struct _CSB_ST{
        u8  csb[8];                             //前左，前右，后左，后右，左前，左后，右前，右后各超声波距离
}xCSB_D;

typedef struct _ZHT_ST{
        float dmp[4];                           //四元数
        float alj[3];                           //欧拉角参数
        s16 jasd[3];                            //加速度参数
        s16 jaosd[3];                           //角速度参数
        s16 clj[3];                             //磁力计参数
        s16 sys_wd;                             //系统温度
        s16 sys_qy;                             //系统气压
}xZHT_D;

extern QueueHandle_t xDRV,xSYS,xCSB,xZHT,xYAK,xREV,xCON;
extern  SEND_D xSentData;
extern RECV_D xComData;
//void upload_init(void);
 void upload(void);
u8 jiema(s8 n,RECV_D* com_data);