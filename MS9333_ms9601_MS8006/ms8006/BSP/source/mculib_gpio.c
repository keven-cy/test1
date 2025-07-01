/**
******************************************************************************
* @file    mculib_gpio.c
* @author  Weiwei Cheng
* @version V1.0.0
* @date    14-Aug-2020
* @brief   This file is the gpio interface source code.  
*
* Copyright (c) 2018, MacroSilicon Technology Co.,Ltd.   
******************************************************************************/
#include "..\header\mculib_common.h"


VOID mculib_clock_init(VOID)
{
    HAL_Init();
    
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};	
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HIRC;
    RCC_OscInitStruct.HIRCState = RCC_HIRC_ON;
    RCC_OscInitStruct.HIRCCalibrationValue = RCC_HIRCCALIBRATION_24M;

    __HAL_RCC_HIRC_CALIBRATIONVALUE_ADJUST(RCC_OscInitStruct.HIRCCalibrationValue);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HIRC;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APBCLKDivider = RCC_PCLK_DIV1;

    MODIFY_REG(RCC->PCLKDIV, RCC_PCLKDIV_APBCKDIV, RCC_ClkInitStruct.APBCLKDivider);
    MODIFY_REG(RCC->HCLKDIV, RCC_HCLKDIV_AHBCKDIV, RCC_ClkInitStruct.AHBCLKDivider);
    __HAL_RCC_SYSCLK_CONFIG(RCC_ClkInitStruct.SYSCLKSource);

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
}

void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */
    HAL_IncTick();
  /* USER CODE END SysTick_IRQn 0 */
}

// GPIO config
VOID muclib_gpio_init(GPIO_TypeDef *e_port, GPIO_InitTypeDef *GPIO_Init)
{
    if (((e_port == GPIOC) && ((GPIO_Init->Pin == GPIO_PIN_7) ||(GPIO_Init->Pin == GPIO_PIN_All))) ||
        ((e_port == GPIOD) && ((GPIO_Init->Pin == GPIO_PIN_1) ||(GPIO_Init->Pin == GPIO_PIN_All))) )
    {
        mculib_delay_ms(100);
		mculib_delay_ms(100);
		mculib_delay_ms(100);
		mculib_delay_ms(100);
        RCC->UNLOCK = 0x55AA6699;
        RCC->SWDIOCR = 0x5A690000;
    }
    HAL_GPIO_Init(e_port, GPIO_Init); 
}
