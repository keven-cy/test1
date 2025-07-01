/**
******************************************************************************
* @file    mculib_iap.c
* @author  Weiwei Cheng
* @version V1.0.0
* @date    14-Aug-2020
* @brief   This file is the iap interface source code.  
*
* Copyright (c) 2018, MacroSilicon Technology Co.,Ltd.   
******************************************************************************/
#include "..\header\mculib_common.h"

void Error_Handler(void)
{
	printf("Error Address is 0x%x\n", uiAddress);
	printf("Error Data is 0x%x\n", *(uint8_t*)(uiAddress));	
}

VOID mculib_iap_erase(void)
{
	sFlashEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
	sFlashEraseInit.PageAddress = FLASH_PROGRAM_ADDRESS_START;
	sFlashEraseInit.NbPages	= (FLASH_PROGRAM_ADDRESS_END - FLASH_PROGRAM_ADDRESS_START)/FLASH_PAGE_SIZE + 1;
	if(HAL_FLASH_Erase(&sFlashEraseInit, &uiErrorPage) != HAL_OK)
	{
		Error_Handler();		
	}	
}
		
UINT8 mculib_iap_read_byte(UINT32 u32_address)
{
    return *(__IO uint8_t*)u32_address;;
}

VOID mculib_iap_read(UINT32 u32_address, UINT8 u8_length, UINT8 *p_u8_buffer)
{
    UINT8 u8_i;

    for (u8_i = 0; u8_i < u8_length; u8_i ++)
    {
        *(p_u8_buffer + u8_i) = mculib_iap_read_byte(u32_address + u8_i);
    }
}

VOID mculib_iap_write_byte(UINT32 u32_address,UINT8 iapdata)
{
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, u32_address, iapdata);
}

VOID mculib_iap_write(UINT32 u32_address, UINT8 u8_length, UINT8 *p_u8_buffer)
{
    UINT8 u8_i;

    for (u8_i = 0; u8_i < u8_length; u8_i ++)
    {
        mculib_iap_write_byte(u32_address + u8_i, *(p_u8_buffer + u8_i));
    }
}
