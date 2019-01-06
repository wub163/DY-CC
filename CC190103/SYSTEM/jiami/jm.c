#include "stm32f4xx_flash.h"
#include "../jiami/jm.h"
//加密后的CPUID  
 #define UID_BASE            (0x1FFF7A10)          //stm32f407 身份地址。   /*!< System Control Space Base Address  */
// #define UID_BASE            (0x1FFFF7E8)        //stm32f103 身份地址。  /*!< System Control Space Base Address  */
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
//写入加密数据  
void WriteEncrypt(void)  
{   
    //第一次烧写:将UID写入到Flash中  
    if(CPUIDEncrypt==0xFFFFFFFF)  
   {  
        uint32_t CpuID[3];         
        //获取CPU唯一的ID  
        CpuID[0]=*(vu32*)UID_BASE;  
        CpuID[1]=*(vu32*)UID_BASE+4;  
        CpuID[2]=*(vu32*)UID_BASE+8;          
  
        //加密算法,很简单的加密算法  
        uint32_t EncryptCode=(CpuID[0]>>3)+(CpuID[1]>>1)+(CpuID[2]>>2);     
        FLASH_Unlock();  
        FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGAERR|FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR|FLASH_FLAG_WRPERR);  
        FLASH_ProgramWord((uint32_t)&CPUIDEncrypt, EncryptCode);  

//        FLASH_ReadOutProtection(ENABLE);

        FLASH_Lock(); 
    }  
}  
//判断加密  
u8 JudgeEncrypt(void)  
{         
    uint32_t CpuID[4];         
    //获取CPU唯一的ID  
    CpuID[0]=*(vu32*)UID_BASE;  
    CpuID[1]=*(vu32*)UID_BASE+4;  
    CpuID[2]=*(vu32*)UID_BASE+8;      
    //加密算法,很简单的加密算法  
    CpuID[3]=(CpuID[0]>>3)+(CpuID[1]>>1)+(CpuID[2]>>2);     
    //检查Flash中的UID是否合法   
    return (CPUIDEncrypt == CpuID[3]);  
} 




