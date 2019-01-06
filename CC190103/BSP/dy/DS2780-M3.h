#include "stm32f4xx.h"
#define dbh GPIO_SetBits(GPIOC,GPIO_Pin_1)
#define dbl GPIO_ResetBits(GPIOC,GPIO_Pin_1)
#define busww PCout(1)           //位操作，速度快三分之一
#define buswr PCin(1)
void bus_init(void);
void wb(u8 da);
u8 rb(void);
void write(u8 add,u8 da);
void read(void);

typedef struct power{
 float pjdl;
float wd;
float dy;
float dl;
}powercs;
extern powercs pcs;

//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).M4同M3类似,只是寄存器地址变了.
//IO口操作宏定义
#define BITBAND(addr, bitnum) (PERIPH_BB_BASE+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+0x0c) //0x40020014
#define GPIOB_ODR_Addr    (GPIOB_BASE+0x0c) //0x40020414 
#define GPIOC_ODR_Addr    (GPIOC_BASE+0x0c) //0x40020814 
#define GPIOD_ODR_Addr    (GPIOD_BASE+0x0c) //0x40020C14 
#define GPIOE_ODR_Addr    (GPIOE_BASE+0x0c) //0x40021014 
#define GPIOF_ODR_Addr    (GPIOF_BASE+0x0c) //0x40021414    
#define GPIOG_ODR_Addr    (GPIOG_BASE+0x0c) //0x40021814   
#define GPIOH_ODR_Addr    (GPIOH_BASE+0x0c) //0x40021C14    
#define GPIOI_ODR_Addr    (GPIOI_BASE+0x0c) //0x40022014     

#define GPIOA_IDR_Addr    (GPIOA_BASE+0x08) //0x40020010 
#define GPIOB_IDR_Addr    (GPIOB_BASE+0x08) //0x40020410 
#define GPIOC_IDR_Addr    (GPIOC_BASE+0x08) //0x40020810 
#define GPIOD_IDR_Addr    (GPIOD_BASE+0x08) //0x40020C10 
#define GPIOE_IDR_Addr    (GPIOE_BASE+0x08) //0x40021010 
#define GPIOF_IDR_Addr    (GPIOF_BASE+0x08) //0x40021410 
#define GPIOG_IDR_Addr    (GPIOG_BASE+0x08) //0x40021810 
#define GPIOH_IDR_Addr    (GPIOH_BASE+0x08) //0x40021C10 
#define GPIOI_IDR_Addr    (GPIOI_BASE+0x08) //0x40022010 
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入

#define PHout(n)   BIT_ADDR(GPIOH_ODR_Addr,n)  //输出 
#define PHin(n)    BIT_ADDR(GPIOH_IDR_Addr,n)  //输入

#define PIout(n)   BIT_ADDR(GPIOI_ODR_Addr,n)  //输出 
#define PIin(n)    BIT_ADDR(GPIOI_IDR_Addr,n)  //输入
