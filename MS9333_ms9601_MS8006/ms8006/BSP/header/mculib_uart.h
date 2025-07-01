/**
******************************************************************************
* @file    mculib_uart.h
* @author  Weiwei Cheng
* @version V1.0.0
* @date    14-Aug-2020
* @brief   This file defines the uart interface function.  
*
* Copyright (c) 2018, MacroSilicon Technology Co.,Ltd.   
******************************************************************************/
#ifndef _MCULIB_UART_H
#define _MCULIB_UART_H
#include "mculib_typedef.h"
#include "mculib_config.h"
VOID mculib_uart_init(UINT32 u32_baud_rate);

#if 0
VOID mculib_uart1_init(UINT16 u16_baud_rate);
#endif

VOID mculib_uart_print_char(UINT8 u8_byte);
VOID mculib_uart_print_dec(UINT16 u16_word);
VOID mculib_uart_print_hex(UINT16 u16_word);
VOID mculib_uart_print_string(UINT8 *u8_string);

VOID mculib_uart_service(VOID);

//application interface
VOID mculib_uart_log(UINT8 *u8_string);
VOID mculib_uart_log1(UINT8 *u8_string, UINT16 u16_hex);
VOID mculib_uart_log2(UINT8 *u8_string, UINT16 u16_dec);


#ifdef USE_DEBUGGING_LOG
#define LOG(X)      mculib_uart_log((UINT8*)X)
#define LOG1(X, Y)  mculib_uart_log1((UINT8*)X, Y)
#define LOG2(X, Y)  mculib_uart_log2((UINT8*)X, Y)
#else
#define LOG(X)
#define LOG1(X, Y)
#define LOG2(X, Y)
#endif

#endif

