
#include "csb_os.h"
void I2C_read_buf(u8* buf, u8 ReadAddr, u8 Reg, u16 count)
{
        while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

        I2C_GenerateSTART(I2C1,ENABLE);

        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
        I2C_Send7bitAddress(I2C1, ReadAddr, I2C_Direction_Transmitter);

        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
        I2C_SendData(I2C1,Reg);

        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
        I2C_GenerateSTART(I2C1,ENABLE);

        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
        I2C_Send7bitAddress(I2C1, ReadAddr, I2C_Direction_Receiver);

        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

        
        for(s16 i=0;i<count;i++)
       {
        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
       buf[i]=I2C_ReceiveData(I2C1);
       if(i== count-2)I2C_AcknowledgeConfig(I2C1, DISABLE);
       }

         I2C_GenerateSTOP(I2C1, ENABLE);
         I2C_AcknowledgeConfig(I2C1,ENABLE);
}
u8 I2C_read_byte(u8 ReadAddr, u8 Reg)
{       u8 aa;
        while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

        I2C_GenerateSTART(I2C1,ENABLE);

        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
        I2C_Send7bitAddress(I2C1, ReadAddr, I2C_Direction_Transmitter);

        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
        I2C_SendData(I2C1,Reg);

        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
        I2C_GenerateSTART(I2C1,ENABLE);

        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
        I2C_Send7bitAddress(I2C1, ReadAddr, I2C_Direction_Receiver);

        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
      I2C_AcknowledgeConfig(I2C1, DISABLE);
        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
       aa=I2C_ReceiveData(I2C1);

         I2C_GenerateSTOP(I2C1, ENABLE);
         I2C_AcknowledgeConfig(I2C1,ENABLE);
         return aa;
}
void I2C_write_byte(u8 ReadAddr, u8 Reg,u8 data)
{extern vu8 mtime;
mtime=0;
//while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));
        I2C_GenerateSTART(I2C1,ENABLE);

        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))if(mtime>5)return;
        I2C_Send7bitAddress(I2C1, ReadAddr, I2C_Direction_Transmitter);

        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))if(mtime>5)return;
        I2C_SendData(I2C1,Reg);

        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))if(mtime>5)return;
        I2C_SendData(I2C1,data);
        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))if(mtime>5)return;
//         I2C_GenerateSTOP(I2C1, ENABLE);
}
void CSB_ADD(u8 old,u8 new)
{ extern vu8 mtime;
        mtime=0;
        I2C_write_byte(old,2,0xc3);
while(mtime<100);
        I2C_write_byte(old,2,0x73);
while(mtime<200);
        I2C_write_byte(old,2,0x9a);
while(mtime<201);
        I2C_write_byte(old,2,0x92);
while(mtime<202);
        I2C_write_byte(old,2,0x9e);
while(mtime<203);
        I2C_write_byte(old,2,new);
while(1){;}
 }

void CSB_go(u8 add)
{
  // I2C_write_byte(add,2,0x73);///���������scl��ֹ���ͣ�����ʱscl�Ծɻ����͡�
  I2C_write_byte(add,2,0xb0);///A0:��ǿ B0:�����¶Ȳ�� B4:���¶Ȳ�� C9-CC:����
                              ///�¶ȼ���18B20�����𻵡�

}

u16 CSB_jl(u8 ReadAddr)
{       u16 aa;
extern vu8 mtime;///����0.5ms�����5msû��ɣ�ǿ���˳���
mtime=0;
//        while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY));

        I2C_GenerateSTART(I2C1,ENABLE);

        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))if(mtime>5)return 0xffff;
        I2C_Send7bitAddress(I2C1, ReadAddr, I2C_Direction_Transmitter);

        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))if(mtime>5)return 0xffff;
        I2C_SendData(I2C1,2);

        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))if(mtime>5)return 0xffff;
        I2C_GenerateSTART(I2C1,ENABLE);

        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))if(mtime>5)return 0xffff;
        I2C_Send7bitAddress(I2C1, ReadAddr, I2C_Direction_Receiver);

        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))if(mtime>5)return 0xffff;
        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))if(mtime>5)return 0xffff;
       aa=I2C_ReceiveData(I2C1);
      I2C_AcknowledgeConfig(I2C1, DISABLE);
      while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))if(mtime>5){I2C_AcknowledgeConfig(I2C1,ENABLE);return 0xffff;}
       aa=((aa<<8)|I2C_ReceiveData(I2C1));

 //        I2C_GenerateSTOP(I2C1, ENABLE);
         I2C_AcknowledgeConfig(I2C1,ENABLE);
         return aa;
}