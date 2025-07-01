/**
******************************************************************************
* @file    mculib_iap.h
* @author  Weiwei Cheng
* @version V1.0.0
* @date    14-Aug-2020
* @brief   This file defines the iap interface driver function.  
*
* Copyright (c) 2020, MacroSilicon Technology Co.,Ltd.   
******************************************************************************/
#ifndef __MCULIB_IAP_H
#define __MCULIB_IAP_H


#define FLASH_PROGRAM_ADDRESS_START				0xB000
#define FLASH_PROGRAM_ADDRESS_END					0xB200

static FLASH_EraseInitTypeDef 	sFlashEraseInit;
static uint32_t uiErrorPage;
static uint32_t uiAddress = 0;


VOID mculib_iap_erase(void);
void Error_Handler(void);
UINT8 mculib_iap_read_byte(UINT32 u32_address);
VOID mculib_iap_read(UINT32 u32_address, UINT8 u8_length, UINT8 *p_u8_buffer);

VOID mculib_iap_write_byte(UINT32 u32_address,UINT8 iapdata);
VOID mculib_iap_write(UINT32 u32_address, UINT8 u8_length, UINT8 *p_u8_buffer);

#endif









