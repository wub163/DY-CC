#include "LED2811.h"
#include "string.h"
#include "stdlib.h"
#include "sys.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "upload.h"
#define WS2811_DATA_BUFFER_SIZE         (WS2811_BITS_PER_LED * WS2811_COUNT)
#define WS2811_DMA_BUFFER_SIZE          (WS2811_DATA_BUFFER_SIZE + WS2811_DELAY_BUFFER_LENGTH)
#define BIT_COMPARE_1 20 // timer compare value for logical 1
#define BIT_COMPARE_0 7  // timer compare value for logical 0

  QueueHandle_t xDRV_LED;


typedef struct hsvColor_s {
    uint16_t h; // 0 - 359
    uint8_t s; // 0 - 255
    uint8_t v; // 0 - 255
} hsvColor_t;


volatile u8 ws2811busy = 0;
hsvColor_t *hsvColor=NULL;      //灯带数据区
u16 *DMA_BUFFER=NULL;            //DMA数据区
u8 ws2811OK=0;

//                          H    S    V
#define LED_BLACK        {  0,   0,   0}
#define LED_WHITE        {  0, 255, 255}
#define LED_RED          {  0,   0, 255}
#define LED_ORANGE       { 30,   0, 255}
#define LED_YELLOW       { 60,   0, 255}
#define LED_LIME_GREEN   { 90,   0, 255}
#define LED_GREEN        {120,   0, 255}
#define LED_MINT_GREEN   {150,   0, 255}
#define LED_CYAN         {180,   0, 255}
#define LED_LIGHT_BLUE   {210,   0, 255}
#define LED_BLUE         {240,   0, 255}
#define LED_DARK_VIOLET  {270,   0, 255}
#define LED_MAGENTA      {300,   0, 255}
#define LED_DEEP_PINK    {0,   0, 255}

struct rgbColor24bpp_s {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

typedef union {
    struct rgbColor24bpp_s rgb;
    uint8_t raw[3];
} rgbColor24bpp_t;


const hsvColor_t hsv_black       = LED_BLACK;
const hsvColor_t hsv_white       = LED_WHITE;
const hsvColor_t hsv_red         = LED_RED;
const hsvColor_t hsv_orange      = LED_ORANGE;
const hsvColor_t hsv_yellow      = LED_YELLOW;
const hsvColor_t hsv_limeGreen   = LED_LIME_GREEN;
const hsvColor_t hsv_green       = LED_GREEN;
const hsvColor_t hsv_mintGreen   = LED_MINT_GREEN;
const hsvColor_t hsv_cyan        = LED_CYAN;
const hsvColor_t hsv_lightBlue   = LED_LIGHT_BLUE;
const hsvColor_t hsv_blue        = LED_BLUE;
const hsvColor_t hsv_darkViolet  = LED_DARK_VIOLET;
const hsvColor_t hsv_magenta     = LED_MAGENTA;
const hsvColor_t hsv_deepPink    = LED_DEEP_PINK;


void ws2811LedStripHardwareInit(void);
void ws2811LedStripDMAEnable(void);

rgbColor24bpp_t* hsvToRgb24(const hsvColor_t* c)
{
    static rgbColor24bpp_t r;
    uint16_t val = c->v;
    uint16_t sat = 255 - c->s;
    uint32_t base;
    uint16_t hue = c->h;

    if (sat == 0) { // Acromatic color (gray). Hue doesn't mind.
        r.rgb.r = val;
        r.rgb.g = val;
        r.rgb.b = val;
    } else {

        base = ((255 - sat) * val) >> 8;

        switch (hue / 60) {
            case 0:
            r.rgb.r = val;
            r.rgb.g = (((val - base) * hue) / 60) + base;
            r.rgb.b = base;
            break;
            case 1:
            r.rgb.r = (((val - base) * (60 - (hue % 60))) / 60) + base;
            r.rgb.g = val;
            r.rgb.b = base;
            break;

            case 2:
            r.rgb.r = base;
            r.rgb.g = val;
            r.rgb.b = (((val - base) * (hue % 60)) / 60) + base;
            break;

            case 3:
            r.rgb.r = base;
            r.rgb.g = (((val - base) * (60 - (hue % 60))) / 60) + base;
            r.rgb.b = val;
            break;

            case 4:
            r.rgb.r = (((val - base) * (hue % 60)) / 60) + base;
            r.rgb.g = base;
            r.rgb.b = val;
            break;

            case 5:
            r.rgb.r = val;
            r.rgb.g = base;
            r.rgb.b = (((val - base) * (60 - (hue % 60))) / 60) + base;
            break;

        }
    }
    return &r;
}


void WS2811_DMAHandler(void)
{
    if (DMA_GetFlagStatus(WS2811_DMA_Stream,WS2811_DMA_TC_FLAG)) {
        DMA_Cmd(WS2811_DMA_Stream, DISABLE);
        DMA_ClearFlag(WS2811_DMA_Stream,WS2811_DMA_TC_FLAG);
        ws2811busy = 0;
    }
}

void setStripColor(const hsvColor_t *color)
{
    uint16_t index;
    for (index = 0; index < WS2811_COUNT; index++) {
        *(hsvColor+index) = *color;
    }
}


void ws2811UpdateStrip(void)
{
    static uint32_t waitCounter = 0;
    static rgbColor24bpp_t *rgb24;
    u16 dmaBufferOffset=0;
    u16 ledIndex = 0;                       // reset led index
    
    if(ws2811OK){
    // wait until previous transfer completes
    while(ws2811busy) {
        waitCounter++;
    }


    // fill transmit buffer with correct compare values to achieve
    // correct pulse widths according to color values
    while (ledIndex < WS2811_COUNT)
    {
        rgb24 = hsvToRgb24(&hsvColor[ledIndex]);

    uint32_t grb = (rgb24->rgb.g << 16) | (rgb24->rgb.r << 8) | (rgb24->rgb.b);

    for (int8_t index = 23; index >= 0; index--) {
        DMA_BUFFER[dmaBufferOffset++] = (grb & (1 << index)) ? BIT_COMPARE_1 : BIT_COMPARE_0;
    }

        ledIndex++;
    }

    ws2811LedStripDMAEnable();
    }}



void ws2811LedStripHardwareInit(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    uint16_t prescalerValue;

    WS2811_GPIO_RCC_PERIPHERAL(WS2811_GPIO_PERIPHERAL, ENABLE);

    /* GPIOA Configuration: TIM3 Channel 1 as alternate function push-pull */
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = WS2811_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(WS2811_GPIO, &GPIO_InitStructure);
    GPIO_PinAFConfig(WS2811_GPIO, WS2811_PIN_SOURCE , WS2811_GPIO_AF);

    WS2811_TIMER_RCC_PERIPHERAL(WS2811_TIMER_PERIPHERAL, ENABLE);
    /* Compute the prescaler value */
    prescalerValue = (uint16_t) (SystemCoreClock / 45000000) - 1;
    /* Time base configuration */
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period =  (29-1); // 45/30=1.5(750kHz)
    TIM_TimeBaseStructure.TIM_Prescaler = prescalerValue;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(WS2811_TIMER, &TIM_TimeBaseStructure);

    /* PWM1 Mode configuration: Channel1 */
    TIM_OCStructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

#if lease_board     //激光小车主控板
    TIM_OC2Init(WS2811_TIMER, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(WS2811_TIMER, TIM_OCPreload_Enable);
#else
    TIM_OC1Init(WS2811_TIMER, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(WS2811_TIMER, TIM_OCPreload_Enable);
#endif 

    TIM_CtrlPWMOutputs(WS2811_TIMER, ENABLE);

    /* configure DMA */
    /* DMA clock enable */
    WS2811_DMA_RCC_PERIPHERAL(WS2811_DMA_PERIPHERAL, ENABLE);

    /* DMA1 Channel Config */
    DMA_DeInit(WS2811_DMA_Stream );

    DMA_StructInit(&DMA_InitStructure);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&WS2811_TIMER->WS2811_TIMER_CH;
    DMA_InitStructure.DMA_Memory0BaseAddr  = (uint32_t)DMA_BUFFER;
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_BufferSize = WS2811_DMA_BUFFER_SIZE;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_Channel = WS2811_DMA_CHANNEL;

    DMA_Init(WS2811_DMA_Stream , &DMA_InitStructure);

    TIM_DMACmd(WS2811_TIMER, WS2811_DMA_SOURCE, ENABLE);

    DMA_ITConfig(WS2811_DMA_Stream , DMA_IT_TC, ENABLE);

    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = WS2811_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 9;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

//    setStripColor(&hsv_white);
    setStripColor(&hsv_deepPink);
    ws2811UpdateStrip();}
    
void ws2811LedStripDMAEnable(void)
{
    ws2811busy =1;  
    DMA_SetCurrDataCounter(WS2811_DMA_Stream, WS2811_DMA_BUFFER_SIZE);  // load number of bytes to be transferred
    TIM_SetCounter(WS2811_TIMER, 0);
    TIM_Cmd(WS2811_TIMER, ENABLE);
    DMA_Cmd(WS2811_DMA_Stream, ENABLE);
}



void ws2811Init(void)
{
    ws2811busy=0;
    hsvColor=pvPortMalloc(WS2811_COUNT*sizeof(hsvColor_t));
    DMA_BUFFER=pvPortMalloc(WS2811_DMA_BUFFER_SIZE*sizeof(*DMA_BUFFER));
    if((hsvColor==NULL)||(DMA_BUFFER==NULL))ws2811busy=0;
    else 
    {ws2811OK=1;
    memset(DMA_BUFFER, 0, WS2811_DMA_BUFFER_SIZE*sizeof(*DMA_BUFFER));
    ws2811LedStripHardwareInit();}
}


/*/============================
变光子函数
ws：起点位置
we：终点位置
mode：1，h。2，s。3，v
ms：变量起点
me：变量终点(不在变量范围内的点，不参与变化。ms<me)
p：变量（变化量）
返回：0，没完成，1，到达变量起点，2，到达变量终点。
//============================*/
u8 slow(u16 ws,u16 we,u8 mode,u16 ms,u16 me,s8 p)
{
u16 ledwz;//led位置
  s16 count,temp,zt=0;
  if((ms<me)&&(p!=0)){
  count=we-ws+1;if(count<=0)count+=WS2811_COUNT;
  zt=1;if(p>0)zt=2;
for(int i = 0; i < count; ++i)
{
ledwz=ws+i;if(ledwz>=WS2811_COUNT)ledwz-=WS2811_COUNT;//显示回转。
  switch(mode)
  {case 1:
    {temp=hsvColor[ledwz].h+p;
    if((temp>ms)&&(temp<me)){hsvColor[ledwz].h=temp;zt=0;}break;}
  case 2:
    {temp=hsvColor[ledwz].s+p;
    if((temp>ms)&&(temp<me)){hsvColor[ledwz].s=temp;zt=0;}break;}
  case 3:
    {temp=hsvColor[ledwz].v+p;
    if((temp>ms)&&(temp<me)){hsvColor[ledwz].v=temp;zt=0;}break;}
  default:;
  }
  
}

  
  }
  return zt;
}


/*/============================
移位子函数
ws：起点位置
we：终点位置 (ws<we)
p：位移量(每次位移量)
返回：s8 本次运行后的实际位移量。+小值移动，-大值移动。
//============================*/
s8 move(u16 ws,u16 we,s8 p)
{
  hsvColor_t temp[4];
  s8 zt=0;
  if(abs(p)<(we+1-ws)){//合法性判断。+1是理解与实际的统一。
  if(p>0){
    for (u8 i=0;i<p;++i)
    {temp[i]=hsvColor[ws+i];}
     for (u16 i=0;i<(we+1-p);++i)
     {hsvColor[ws+i]=hsvColor[ws+p+i];}
     for (u8 i=0;i<p;++i)
     {hsvColor[we+1-p+i]=temp[i];}
     zt=p;
       }
  else{
  if(p<0){p=-p;
    for (u8 i=0;i<p;++i)
    {temp[i]=hsvColor[we+1-p];}

    for (u16 i=0;i<(we+1-p);++i)
     {hsvColor[we-i]=hsvColor[we-p-i];}
    for (u8 i=0;i<p;++i)
     {hsvColor[ws+i]=temp[i];}
  
  
  zt=-p;
  }
  }}
  return zt;
}





void ws2811task(void* pv)          //灯带任务
{ 
  xDRV_D motosta;
u8 aa=0;
  s8 sp1=1,sp2=-1,sp3=1;
  TickType_t xLastWakeTime;
  ws2811Init();
vTaskDelay(2000/portTICK_PERIOD_MS);
  xDRV_LED=xQueueCreate( 2, sizeof(xDRV_D ) );     //驱动器

 xLastWakeTime = xTaskGetTickCount ();
   
  while(1){
   
                    vTaskDelayUntil( &xLastWakeTime, 50/portTICK_PERIOD_MS);//LED的固定刷新频率。
                    
                    xQueueReceive(xDRV_LED,&motosta,0); //接收驱动器状态。

                    
                    if(abs(motosta.drv_spe[0]+motosta.drv_spe[1])<100){ //停止状态。


                    }
                    if((motosta.drv_spe[0]+motosta.drv_spe[1])>100){ //前进状态。
                    }
                    if((motosta.drv_spe[0]+motosta.drv_spe[1])<-100){ //后退状态。
                    }
                    
                    
////                    if((aa++)<80){
//                    if(0){
//                    switch(slow(3,5,1,00,360,sp1))
//                    {
//                    case 1:
//                    sp1=9;break;
//                    case 2:
//                    sp1=-9;break;}
//                    switch(slow(6,12,2,00,255,sp2))
//                    {
//                    case 1:
//                    sp2=5;break;
//                    case 2:
//                    sp2=-5;break;}
//                    switch(slow(13,20,3,00,50,sp3))
//                    {
//                    case 1:
//                    sp3=1;break;
//                    case 2:
//                    sp3=-1;break;}
//                    }
//                    else {
//                    aa=251;
//                    move(0,10,2);
//                    }   
                    slow(1,14,1,0,360,1);
                    
                    
                 ws2811UpdateStrip();
                 
//                 uxTaskGetStackHighWaterMark( NULL );

              }
          
 
  
  
}


