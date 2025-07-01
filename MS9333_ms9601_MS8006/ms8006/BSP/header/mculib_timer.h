/**
******************************************************************************
* @file    mculib_timer.h
* @author  Weiwei Cheng
* @version V1.0.0
* @date    14-Aug-2020
* @brief   This file defines the timer interface driver function.  
*
* Copyright (c) 2018, MacroSilicon Technology Co.,Ltd.   
******************************************************************************/
#ifndef __MCULIB_TIMER_H
#define __MCULIB_TIMER_H

//Suggested timer base
#define TIMER_BASE_10MS     (10)
#define TIMER_BASE_100MS    (100)

#define SYS_TIMEOUT_10MS    (1)   // TIMER_BASE_10MS * 1
#define SYS_TIMEOUT_50MS    (5)   // TIMER_BASE_10MS * 5
#define SYS_TIMEOUT_100MS   (10)   
#define SYS_TIMEOUT_500MS   (50)
#define SYS_TIMEOUT_1SEC    (100)  
#define SYS_TIMEOUT_2SEC    (200)

// Timer0 is a general 16-bit timer.
VOID mculib_timer_config_ms(UINT8 u8_ms);

//
VOID mculib_delay_us(UINT8 u8_us);
VOID mculib_delay_ms(UINT8 u8_ms);


extern UINT16 g_u16_sys_timer;
extern UINT16 g_u16_key_timer;
extern UINT16 g_u16_mute_timer;
extern UINT16 g_u16_i2s_mclk_output_timer;

#endif //endif use timer module


