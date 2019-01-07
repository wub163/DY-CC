#include "stm32f4xx_flash.h"
#include "../jiami/jm.h"
//���ܺ��CPUID  
 #define UID_BASE            (0x1FFF7A10)          //stm32f407 ��ݵ�ַ��   /*!< System Control Space Base Address  */
// #define UID_BASE            (0x1FFFF7E8)        //stm32f103 ��ݵ�ַ��  /*!< System Control Space Base Address  */
 const static u32 CPUIDEncrypt = 0xFFFFFFFF;  

//void My_Code_Data_Protection(void)
//{
//        FLASH_Unlock();
//        FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGAERR|FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR|FLASH_FLAG_WRPERR);  
//
//        if(FLASH_GetReadOutProtectionStatus() == RESET)
//               FLASH_ReadOutProtection(ENABLE);
//
//        FLASH_EnableWriteProtection((uint32_t)0x00000FFF);
//                FLASH_Lock(); 
//
//}
//
//д���������  
void WriteEncrypt(void)  
{   
    //��һ����д:��UIDд�뵽Flash��  
    if(CPUIDEncrypt==0xFFFFFFFF)  
   {  
        uint32_t CpuID[3];         
        //��ȡCPUΨһ��ID  
        CpuID[0]=*(vu32*)UID_BASE;  
        CpuID[1]=*(vu32*)UID_BASE+4;  
        CpuID[2]=*(vu32*)UID_BASE+8;          
  
        //�����㷨,�ܼ򵥵ļ����㷨  
        uint32_t EncryptCode=(CpuID[0]>>3)+(CpuID[1]>>1)+(CpuID[2]>>2);     
        FLASH_Unlock();  
        FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGAERR|FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR|FLASH_FLAG_WRPERR);  
        FLASH_ProgramWord((uint32_t)&CPUIDEncrypt, EncryptCode);  

//        FLASH_ReadOutProtection(ENABLE);

        FLASH_Lock(); 
    }  
}  
//�жϼ���  
u8 JudgeEncrypt(void)  
{         
    uint32_t CpuID[4];         
    //��ȡCPUΨһ��ID  
    CpuID[0]=*(vu32*)UID_BASE;  
    CpuID[1]=*(vu32*)UID_BASE+4;  
    CpuID[2]=*(vu32*)UID_BASE+8;      
    //�����㷨,�ܼ򵥵ļ����㷨  
    CpuID[3]=(CpuID[0]>>3)+(CpuID[1]>>1)+(CpuID[2]>>2);     
    //���Flash�е�UID�Ƿ�Ϸ�   
    return (CPUIDEncrypt == CpuID[3]);  
} 




