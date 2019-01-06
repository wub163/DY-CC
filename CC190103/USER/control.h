#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define STOP IN1        //机械壁障 正脉冲
#define CCBZ IN2        //叉齿壁障 负脉冲
#define SDOK IN3        //手动信号 正脉冲
#define ZBOK IN4        //栈板到位 负脉冲
#define ZBAQ IN5        //栈板安全 负脉冲
#define BIZ1 IN6        //激光外区 正脉冲
#define BIZ2 IN7        //激光内区 正脉冲

#define GO   OUT0               //低电平，高电平禁止
#define CCON OUT7               //低电平，高电平急停

#define speedmid 1400   //速度控制中位  前进： 300：170寸动  400：400  500：1000 600：2000---900：5000 1000：6000（<5800）
                        //      （负值）后退： 450：200      500：500  550：800 600：1100 650:1400 700:1700 750:2000
#define speedaq  200    //速度低速禁止
#define lrmid    1700   //左右控制中位  每100等于50
#define udmid    1400   //上下控制中位  400寸动  900满速
#define highbase 248    //高度基准值。最低位传感器高度
#define highmax  2900   //高度最大值
#define highmin  31     //最低位,能降到的最低位。
#define highslow 5      //升降对位。
#define highfast 9     //快速升降
#define lturn    -83     //左转向固定舵角
#define lcount   4160   //左转向180计数
#define rturn    77   //右转向固定舵角
#define rcount   4280   //左右转向180计数
#define lrspeed  3      //左右转向速度。
//const s16 speed[]={750,750,700,650,600,550,500,450,0,300,400,500,600,700,800,900,1000};
//AOUT(0)  SPEED   650    1260 1384  1540   2555
//AOUT(1,2)TURN
//LEFT 3021  1670     340
//RIGHT 350  1670    2978
//AOUT(3,4) UP/DOWN
//UP    384     1277    2340
//DOWN  2375    1508    490



typedef struct _SYS_ST{
  s32 st_meterhz;       //小车行驶脉冲数
  s32 st_meter;      //小车里程计
  s16 st_speed;      //小车速度
  s16 con_speed;     //小车控制速度。

  s16 st_turn;       //小车实际转向角
  s16 const_turn;       //反馈转向控制值
  s16 con_turn;         //小车控制转向。

  s16 st_high;       //叉齿高度。
  s16 con_highsp;         //升降速度值。
  s16 con_highset;      //升降高度值。   当高度为非有效值（0--3000）时，升降纯速度控制。但高度不可超出有效范围。前提高度已标零（highinit=0xaa。）
  s16 con_highwork;      //自动升降调整中。
    
  s16 st_dy;         //系统电压
  s16 st_dl;         //电机电流 
  s16 st_contp;      //控制器温度
  s16 st_mototp;     //电机温度
  u8  highinit;      //highinit:0xA0  进入设置流程。0xA1，下降找跳变点，0xA2，上升找跳变点。0xAA，初始化完成。0xAF 初始化失败。。
  u8  st_mod;        //控制方式  1，自动 0，手动。
  u8  st_stat;       //小车状态  1，工作 0，待机。         自身反馈不用
  u8  st_man;        //小车踏板有人。踏板放下时会输出有人信号  此信号不用
  u8  st_conok;        //手柄降下，小车可控 。            自身反馈不用
  u8  st_tbopen;        //踏板放下。
  u8  st_ryk;          //右侧腰靠放下。
  u8  st_lyk;          //左侧腰靠放下。
  u8  dd_com;          //当前调度命令
  u8  dd_val;          //当前调度命令参数。
  u8  dd_ok;            //收到合法调度包。
  u8  err;             //断线计数
  
  
}xSYS_D;
extern xSYS_D SYSST;
void control(void* pa);
void high_init(void);