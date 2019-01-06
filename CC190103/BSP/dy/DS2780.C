#include "DS2780.h"

powercs pcs;
void dy_bus_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能PD,PE,PF,PG时钟  
  GPIO_InitStructure.GPIO_Pin = (1<<5);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//复用输出
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化  
busww=1;	

}

void delayus(u8 i)                //1us 延时
{
u16 dd;
  dd=i*27;
  while(dd--);
}

//======================标准
//u8 rst(void)
//{
//u8 da;
//  busww=0;
//delayus(250);
//delayus(250);
//busww=1;
//delayus(100);
//da=buswr;
//delayus(200);
//delayus(100);
//return da;
//
//}
//void wh(void)
//{
//busww=0;
//delayus(12);
//busww=1;
//delayus(35);
//}
//void wl(void)
//{
//busww=0;
//delayus(48);
//busww=1;
//delayus(1);
//}
//
//
//u8 rhl(void)
//{
//u8 da=0;
//  busww=0;
//delayus(12);
//busww=1;
//delayus(5);
//da= buswr;
//delayus(30);
//
//return da;
//}
//===================高速
u8 rst(void)//4+14us 0;有芯片 1：无芯片
{
u8 da;
  busww=0;
delayus(30);//实测大于23 约26us
busww=1;
delayus(5);
da=buswr;
delayus(15);
return da;

}

void wh(void)
{
busww=0;
delayus(1);
busww=1;
delayus(5);
}
void wl(void)
{
busww=0;
delayus(5);
busww=1;
delayus(1);
}


u8 rhl(void)
{
u8 da=0;
  busww=0;
delayus(1);
busww=1;
delayus(1);
da= buswr;
delayus(2);

return da;
}

void wb(u8 da)
{
u8 daa;

  for(int i = 0; i < 8; ++i)
{
 daa=da>>i;
  if (daa&1) wh();else wl(); 
}
}

u8 rb(void)
{
  u8 da=0;
  for(int i = 0; i < 8; ++i)
{
 da=da|(rhl()<<i); 
}
return da;
}

void write(u8 add,u8 da)
{
wb(0x6c);
  wb(add);
wb(da);
}

void read(void)
{
 s16 pjdl,wd,dy,dl;
  if(!rst()) {
wb(0xCC);
wb(0X69);
wb(0X08);
pjdl=rb()<<8|rb();
wd=rb()<<8|rb();
dy=rb()<<8|rb();
dl=rb()<<8|rb();
pcs.pjdl=pjdl*.0005208;                          //分辨率是1.5625uv
pcs.wd=(float)((wd>>5)*0.125);
pcs.dy=(float)((dy>>5)*4.88*.008)-0.1;          //分辨率是4.88mv
pcs.dl=dl*.0005208;


 }
}