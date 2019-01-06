#ifndef __GPIO_H
#define __GPIO_H

#include "sys.h"
#define LEDR  PCout(4)
#define LEDB  PBout(0)
#define LEDY  PCout(5)   //0:点亮
#define BEEP  PCout(2)   //1:鸣响

//输出部分   正逻辑
#define BAOZ     PGout(9)  //抱闸  
#define CCUP     PIout(11) //插齿升   
#define CCDN     PCout(1)  //插齿降
#define ZDCD     PCout(13) //自动充电     
#define WRED     PGout(13) //三色红
#define WGEE     PAout(0)  //三色绿
#define WYEL     PAout(2)  //三色黄

//输入部分
#define STOP     PIin(8)        //急停开关  H 
#define LSCB     PAin(3)        //拉绳触边  L
#define ZBJC     PBin(11)       //栈板检测  L
#define ZXLW     PGin(3)        //转向零位  L/H
#define CCCX     PAin(4)        //插齿触须  L
#define AUTO1    PAin(5)        //自动旋钮  L
#define UPOK     PAin(6)        //插齿高位  L
#define AUTO2    PAin(7)        //自动备用//东药有损坏，部分设备使用此引脚。

#define manbz    2      //抱闸控制位
#define manspeed 1      //速度有效控制位
#define manupdn  0      //插齿升降控制位

void GPIOInit(void);

#endif


