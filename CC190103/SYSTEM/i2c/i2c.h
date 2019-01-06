/*********************************************************************
File    : i2c.h
Purpose : 
**********************************************************************/
#ifndef __I2C_H__
#define __I2C_H__
/****************************** Includes *****************************/
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "set.h"

//#include "./systick/bsp_SysTick.h"
//#include "./usart/bsp_debug_usart.h"
/****************************** Defines *******************************/

#define I2CSENSORS_SPEED                 400000
#define I2C_OWN_ADDRESS           0xA0



//毫秒级延时(需要定时器支持)，或者重写Delay宏


#define Delay(x)		 vTaskDelay((x)/portTICK_PERIOD_MS)

 
#define I2Cx_FLAG_TIMEOUT             ((uint32_t) 900) //0x1100
#define I2Cx_LONG_TIMEOUT             ((uint32_t) (300 * I2Cx_FLAG_TIMEOUT)) //was300
 
 
/*引脚定义*/ 
#if lease_board     //激光小车主控板LCD屏


#define SENSORS_I2C_SCL_GPIO_PORT         GPIOH
#define SENSORS_I2C_SCL_GPIO_CLK          RCC_AHB1Periph_GPIOH
#define SENSORS_I2C_SCL_GPIO_PIN          GPIO_Pin_4
#define SENSORS_I2C_SCL_GPIO_PINSOURCE    GPIO_PinSource4
 
#define SENSORS_I2C_SDA_GPIO_PORT         GPIOH
#define SENSORS_I2C_SDA_GPIO_CLK          RCC_AHB1Periph_GPIOH
#define SENSORS_I2C_SDA_GPIO_PIN          GPIO_Pin_5
#define SENSORS_I2C_SDA_GPIO_PINSOURCE    GPIO_PinSource5

#define SENSORS_I2C_AF                    GPIO_AF_I2C2

#define SENSORS_I2C              	 I2C2
#define SENSORS_I2C_RCC_CLK               RCC_APB1Periph_I2C2

#else

#define SENSORS_I2C_SCL_GPIO_PORT         GPIOB
#define SENSORS_I2C_SCL_GPIO_CLK          RCC_AHB1Periph_GPIOB
#define SENSORS_I2C_SCL_GPIO_PIN          GPIO_Pin_6
#define SENSORS_I2C_SCL_GPIO_PINSOURCE    GPIO_PinSource6
 
#define SENSORS_I2C_SDA_GPIO_PORT         GPIOB
#define SENSORS_I2C_SDA_GPIO_CLK          RCC_AHB1Periph_GPIOB
#define SENSORS_I2C_SDA_GPIO_PIN          GPIO_Pin_7
#define SENSORS_I2C_SDA_GPIO_PINSOURCE    GPIO_PinSource7

#define SENSORS_I2C_AF                     GPIO_AF_I2C1

#define SENSORS_I2C              					 I2C1
#define SENSORS_I2C_RCC_CLK               RCC_APB1Periph_I2C1
#endif 







/*函数定义*/
#define I2C_Config() I2cMaster_Init()
																			 
void I2cMaster_Init(void);
//void Set_I2C_Retry(unsigned short ml_sec);
static unsigned short Get_I2C_Retry(void);
																			 
int Sensors_I2C_ReadRegister(unsigned char slave_addr,
                                       unsigned char reg_addr,
                                       unsigned short len, 
                                       unsigned char *data_ptr);
int Sensors_I2C_WriteRegister(unsigned char slave_addr,
                                        unsigned char reg_addr,
                                        unsigned short len, 
                                        const unsigned char *data_ptr);
//
//
//int Sensors_I2C_WriteNoRegister(unsigned char slave_addr,
//                                       unsigned char reg_addr);
//int Sensors_I2C_ReadNoRegister(unsigned char slave_addr,
//                                        unsigned short len, 
//                                         unsigned char *data_ptr);


#endif // __I2C_H__


