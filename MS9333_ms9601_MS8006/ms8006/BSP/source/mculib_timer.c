/**
******************************************************************************
* @file    mculib_timer.c
* @author  Weiwei Cheng
* @version V1.0.0
* @date    14-Aug-2020
* @brief   This file is the timer interface source code.  
*
* Copyright (c) 2018, MacroSilicon Technology Co.,Ltd.   
******************************************************************************/
#include "..\header\mculib_common.h"

// modify by user
UINT16 g_u16_sys_timer=0;
UINT16 g_u16_key_timer;
UINT16 g_u16_mute_timer;
UINT16 g_u16_i2s_mclk_output_timer;

#define BASETIMx_IRQn                             TIM11_IRQn
BASETIM_HandleTypeDef sBaseTimHandle = {0};
VOID mculib_timer_config_ms(UINT8 u8_ms)
{
    __HAL_RCC_BASETIM_CLK_ENABLE();

    /* Set the Time Base configuration */
    TIM11->CR = BASETIM_TIMER_SELECT | BASETIM_AUTORELOAD_ENABLE | BASETIM_MAXCNTLEVEL_32BIT |\
                BASETIM_REPEAT_MODE  | BASETIM_PRESCALER_DIV1;
    
    /* Timer Mode1 AutoReload's load register is different from Mode0 */
    TIM11->LOAD = BASETIM_MAXCNTVALUE_32BIT - SystemCoreClock/(1000/u8_ms);    
    TIM11->BGLOAD = BASETIM_MAXCNTVALUE_32BIT - SystemCoreClock/(1000/u8_ms);

    /* Enable the BASETIM interrupt */
    TIM11->CR |= BASETIM_CR_INTEN;
    /* Start the base timer */
    TIM11->CR |= BASETIM_CR_TR;
    NVIC_EnableIRQ(BASETIMx_IRQn);
}
extern UINT16 g_LedTimer;
VOID TIM11_IRQHandler(VOID)
{
    /* USER CODE BEGIN TIM11_IRQn 0 */
    if((TIM11->MSKINTSR & BASETIM_MSKINTSR_TF) == BASETIM_MSKINTSR_TF)
    {
        if((TIM11->CR & BASETIM_CR_INTEN) == BASETIM_CR_INTEN)
        {
            TIM11->INTCLR |= BASETIM_INTCLR_INTCLR;
        }
    }
    g_u16_sys_timer++;
    g_u16_key_timer++;
	g_u16_mute_timer++;
	g_u16_i2s_mclk_output_timer ++;
    g_LedTimer++;
    /* USER CODE END TIM11_IRQn 0 */
}

//
VOID mculib_delay_us(UINT8 u8_us)
{
    while (--u8_us)
    {
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
    }
}

// 
VOID mculib_delay_ms(UINT8 u8_ms)
{
    do
    {
        mculib_delay_us(250);
        mculib_delay_us(250);
        mculib_delay_us(250);
        mculib_delay_us(250);
        u8_ms --;
    }
    while (u8_ms);
}

