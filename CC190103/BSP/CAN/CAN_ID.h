#include "sys.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "queue.h"
#include "can.h"
#define broadcast  1
typedef struct _CANID_ST{
      u8 ID[8];         //id读取卡号
      u8 WID[8];        //id写卡号
      u8 IDRSSI;          //ID卡信号强度 读卡状态+卡信号强度（0-8）
      u8 IDWRT;          //id写状态。   写卡指令的返回状态（0，成功。1，写错误或卡锁定。2，无卡。）
      u8 IDCOT;          //ID卡指令。   当前卡指令        （0，读卡状态，1，写卡状态。）
      u8 IDERR;         //ID读卡器，无信号时的.1秒频率的错误累加。
      u8 IDCL;          //1,已处理卡代码。
}xCANID_D;

extern  xCANID_D CANID;
extern QueueHandle_t xCANID;  //从中断接收

extern void ID_task(void *pv);//启动读卡器命令

