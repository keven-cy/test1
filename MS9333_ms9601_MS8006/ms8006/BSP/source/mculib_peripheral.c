/**
******************************************************************************
* @file    mculib_peripheral.c
* @author  Weiwei Cheng
* @version V1.0.0
* @date    14-Aug-2020
* @brief   This file is the peripheral interface source code.  
*
* Copyright (c) 2018, MacroSilicon Technology Co.,Ltd.   
******************************************************************************/
#include "..\header\mculib_common.h"

// chip reset port.
#define RESET_PORT     (GPIOD) // config by user
// chip reset pin
#define RESET_PIN      (GPIO_PIN_2)

#define MS9333_RST_Pin        GPIO_PIN_2
#define MS9333_RST_Port       GPIOD

// chip reset port.
//#define MUTE_PORT     (GPIOA) // config by user
// chip reset pin
//#define MUTE_PIN      (GPIO_PIN_1)



#define LOG_TX_Pin      GPIO_PIN_5  // uart tx
#define LOG_TX_Port     GPIOD

#define LOG_RX_Pin      GPIO_PIN_6  // uart rx
#define LOG_RX_Port     GPIOD

#define SCL_Pin         GPIO_PIN_4
#define SCL_Port        GPIOB

#define SDA_Pin         GPIO_PIN_5
#define SDA_Port        GPIOB

#define KEY_SEL_Pin     GPIO_PIN_6  // key1
#define KEY_SEL_Port    GPIOC

#define IR_Pin          GPIO_PIN_4
#define IR_Port         GPIOD



#define MS9601_RST_Pin        GPIO_PIN_3
#define MS9601_Port           GPIOA

#define INT_PORT            GPIOD
#define INT_PIN             GPIO_PIN_3

#define LED_PORT            GPIOC
#define LED_PIN             GPIO_PIN_3


//#define LED_RX_PORT   		(GPIOC) // config by user
//#define LED_RX_PIN    		(GPIO_PIN_7)
//#define LED_TX_PORT   		(GPIOC) // config by user
//#define LED_TX1_PIN    		(GPIO_PIN_3)
//#define LED_TX4_PIN    		    (GPIO_PIN_6)
//#define LED_51    		    (GPIO_PIN_4)
//#define LED_PASS    		(GPIO_PIN_6)




VOID mculib_MUTE_init(void)
{
    //   GPIO_InitTypeDef GPIO_InitStruct = {0};
    //   GPIO_InitStruct.Pin = MUTE_PIN;
    //   GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
//	  GPIO_InitStruct.OpenDrain = GPIO_PUSHPULL;
    //   muclib_gpio_init(MUTE_PORT, &GPIO_InitStruct);
}


//reset for macrosilicon chip
VOID mculib_chip_reset(VOID)// MS9333
{
    GPIO_InitTypeDef GPIO_InitStruct = { 0 };
    GPIO_InitStruct.Pin = RESET_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.OpenDrain = GPIO_OPENDRAIN;
    muclib_gpio_init(RESET_PORT, &GPIO_InitStruct);

    HAL_GPIO_WritePin(RESET_PORT, RESET_PIN, GPIO_PIN_RESET);
    mculib_delay_ms(1);
    HAL_GPIO_WritePin(RESET_PORT, RESET_PIN, GPIO_PIN_SET);
}



BOOL mculib_chip_read_interrupt_pin(VOID)
{

    GPIO_InitTypeDef GPIO_InitStruct = { 0 };
    GPIO_InitStruct.Pin = INT_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    muclib_gpio_init(INT_PORT, &GPIO_InitStruct);
    #if 1
    return HAL_GPIO_ReadPin(INT_PORT, INT_PIN);
    #else
    //if no use interrupt pin, must return TRUE
    return TRUE;
    #endif
}

VOID mculib_led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = { 0 };
    GPIO_InitStruct.Pin = LED_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT;
    GPIO_InitStruct.OpenDrain = GPIO_PUSHPULL;
    muclib_gpio_init(LED_PORT, &GPIO_InitStruct);
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);

}
//led control
VOID mculib_led_control(UINT8 u8LedPin, BOOL bLightUp)
{
//  if (bLightUp)
//  {
//      HAL_GPIO_WritePin(LED_PORT, u8LedPin, GPIO_PIN_RESET);
//  }
//  else
//  {
//      HAL_GPIO_WritePin(LED_PORT, u8LedPin, GPIO_PIN_SET);
//  }
}



VOID mculib_hdmi_tx_led_control(UINT8 u8_channel, BOOL b_on)
{
    switch ( u8_channel )
    {
        case 0:
//      if (b_on)
//      {
//  		HAL_GPIO_WritePin(LED_TX_PORT, LED_TX1_PIN, GPIO_PIN_RESET);
//      }
//      else
//      {
//          HAL_GPIO_WritePin(LED_TX_PORT, LED_TX1_PIN, GPIO_PIN_SET);
//      }
            break;
        case 1:
            if (b_on)
            {
            }
            else
            {
            }
            break;
        case 2:
            if (b_on)
            {

            }
            else
            {

            }
            break;
        case 3:
//      if (b_on)
//      {
//          HAL_GPIO_WritePin(LED_TX_PORT, LED_TX4_PIN, GPIO_PIN_RESET);
//      }
//      else
//      {
//          HAL_GPIO_WritePin(LED_TX_PORT, LED_TX4_PIN, GPIO_PIN_SET);
//      }
            break;
    }
}

VOID mculib_hdmi_rx_led_control(BOOL b_on)
{
//  if (b_on)
//  {
//      HAL_GPIO_WritePin(LED_RX_PORT, LED_RX_PIN, GPIO_PIN_RESET);
//  }
//  else
//  {
//      HAL_GPIO_WritePin(LED_RX_PORT, LED_RX_PIN, GPIO_PIN_SET);
//  }
}

void mculib_chip_reset_ms9601(void)
{
    HAL_GPIO_WritePin(MS9601_Port, MS9601_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(MS9601_Port, MS9601_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(5);
}
void mculib_Led_toggle(void)
{
     HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
}
