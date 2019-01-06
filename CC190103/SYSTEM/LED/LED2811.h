#include "set.h"
#include "FreeRTOS.h"
#include "queue.h"


#if lease_board     //激光小车主控板LCD屏

#define WS2811_GPIO                     GPIOB
#define WS2811_GPIO_PERIPHERAL          RCC_AHB1Periph_GPIOB
#define WS2811_GPIO_RCC_PERIPHERAL      RCC_AHB1PeriphClockCmd
#define WS2811_GPIO_AF                  GPIO_AF_TIM3
#define WS2811_PIN                      GPIO_Pin_5 // TIM3_CH1
#define WS2811_PIN_SOURCE               GPIO_PinSource5

#define WS2811_TIMER                    TIM3
#define WS2811_TIMER_CH                 CCR2
#define WS2811_TIMER_PERIPHERAL         RCC_APB1Periph_TIM3
#define WS2811_TIMER_RCC_PERIPHERAL     RCC_APB1PeriphClockCmd

#define WS2811_DMA_PERIPHERAL           RCC_AHB1Periph_DMA1
#define WS2811_DMA_RCC_PERIPHERAL       RCC_AHB1PeriphClockCmd
#define WS2811_DMA_SOURCE               TIM_DMA_CC2
#define WS2811_DMA_CHANNEL              DMA_Channel_5
#define WS2811_DMA_Stream               DMA1_Stream5
#define WS2811_IRQ                      DMA1_Stream5_IRQn
#define WS2811_DMA_TC_FLAG              DMA_FLAG_TCIF5
#define WS2811_DMAHandler               DMA1_Stream5_IRQHandler 


#else
#define WS2811_GPIO                     GPIOC
#define WS2811_GPIO_PERIPHERAL          RCC_AHB1Periph_GPIOC
#define WS2811_GPIO_RCC_PERIPHERAL      RCC_AHB1PeriphClockCmd
#define WS2811_GPIO_AF                  GPIO_AF_TIM3
#define WS2811_PIN                      GPIO_Pin_6 // TIM3_CH1
#define WS2811_PIN_SOURCE               GPIO_PinSource6

#define WS2811_TIMER                    TIM3
#define WS2811_TIMER_CH                 CCR1
#define WS2811_TIMER_PERIPHERAL         RCC_APB1Periph_TIM3
#define WS2811_TIMER_RCC_PERIPHERAL     RCC_APB1PeriphClockCmd

#define WS2811_DMA_PERIPHERAL           RCC_AHB1Periph_DMA1
#define WS2811_DMA_RCC_PERIPHERAL       RCC_AHB1PeriphClockCmd
#define WS2811_DMA_SOURCE               TIM_DMA_CC1
#define WS2811_DMA_CHANNEL              DMA_Channel_5
#define WS2811_DMA_Stream               DMA1_Stream4
#define WS2811_IRQ                      DMA1_Stream4_IRQn
#define WS2811_DMA_TC_FLAG              DMA_FLAG_TCIF4
#define WS2811_DMAHandler               DMA1_Stream4_IRQHandler 

#endif 

#define WS2811_COUNT                    10      //灯珠数量
#define WS2811_BITS_PER_LED             24      //每个灯珠的数据位
#define WS2811_DELAY_BUFFER_LENGTH      80      // for 50us delay

//extern hsvColor_t *hsvColor;
extern  QueueHandle_t xDRV_LED;

void ws2811task(void* pv);
