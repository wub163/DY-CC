//#include "netconf.h"
#include "stm32f4xx.h"


#define powerruntemp    -15        //极限低温
#define powerchtemp     0          //充电极限低温
#define powervol    23000          //极限低压
#define miniturn    5



//=====================
//mp3提示设置
#define mute                    0       //静音


//240-250为警示区
#define mp3_powerlow            240       //电压过低提示
#define mp3_powertemplow        241       //电池温度低提示。
#define mp3_stop                242       //急停按下
#define mp3_stop_lock           243       //急停已锁。
#define initmp3vol              10        //系统默认音量
#define initmp3num              1         //系统背景音乐。



#define speedinc       4
#define speeddec        8