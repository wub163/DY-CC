#include "stm32f4xx_i2c.h"

void I2C_read_buf(u8* buf, u8 ReadAddr, u8 Reg, u16 count);
u8 I2C_read_byte(u8 ReadAddr, u8 Reg);
void I2C_write_byte(u8 ReadAddr, u8 Reg,u8 data);
void CSB_ADD(u8 old,u8 new);
void CSB_go(u8 add);
u16 CSB_jl(u8 ReadAddr);