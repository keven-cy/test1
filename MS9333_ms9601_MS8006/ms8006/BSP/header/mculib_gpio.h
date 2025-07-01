/**
******************************************************************************
* @file    mculib_gpio.h
* @author  Weiwei Cheng
* @version V1.0.0
* @date    14-Aug-2020
* @brief   This file defines the gpio interface driver function.  
*
* Copyright (c) 2020, MacroSilicon Technology Co.,Ltd.   
******************************************************************************/
#ifndef __MCULIB_GPIO_H
#define __MCULIB_GPIO_H

#if 0
typedef enum
{
    GPIO_P0    = ((UINT8)0x00),  /*!< Port 0 selected */
    GPIO_P1    = ((UINT8)0x01),  /*!< Port 1 selected */
    GPIO_P2    = ((UINT8)0x02),  /*!< Port 2 selected */
    #ifdef MCU_MS8005
    GPIO_P3    = ((UINT8)0x03),  /*!< Port 3 selected */
    GPIO_P4    = ((UINT8)0x04)   /*!< Port 4 selected */
    #endif
} GPIO_TypeDef;

typedef enum
{
    GPIO_PIN_0    = ((UINT8)0x01),  /*!< Pin 0 selected */
    GPIO_PIN_1    = ((UINT8)0x02),  /*!< Pin 1 selected */
    GPIO_PIN_2    = ((UINT8)0x04),  /*!< Pin 2 selected */
    GPIO_PIN_3    = ((UINT8)0x08),  /*!< Pin 3 selected */
    GPIO_PIN_4    = ((UINT8)0x10),  /*!< Pin 4 selected */
    GPIO_PIN_5    = ((UINT8)0x20),  /*!< Pin 5 selected */
    GPIO_PIN_6    = ((UINT8)0x40),  /*!< Pin 6 selected */
    GPIO_PIN_7    = ((UINT8)0x80),  /*!< Pin 7 selected */
    GPIO_PIN_LNIB = ((UINT8)0x0F),  /*!< Low nibble pins selected */
    GPIO_PIN_HNIB = ((UINT8)0xF0),  /*!< High nibble pins selected */
    GPIO_PIN_ALL  = ((UINT8)0xFF)   /*!< All pins selected */
}GPIO_Pin_TypeDef;
#endif


#define GPIO_PUN    0x02
#define GPIO_IEN    0x04
#define GPIO_ODN    0x10
#define GPIO_OEN    0x80

typedef enum
{
    GPIO_MODE_IN_FL_NO_IT      = (UINT8)0x00,   
    GPIO_MODE_OUT_PP_FAST      = (UINT8)0x01,     
    GPIO_MODE_OUT_OD_HIZ_FAST  = (UINT8)0x02,    
}GPIO_Mode_TypeDef;

VOID mculib_clock_init(VOID);

VOID muclib_gpio_init(GPIO_TypeDef *e_port, GPIO_InitTypeDef *GPIO_Init);

#ifdef MCU_MS8005
//----------------------------------------------------------
//  P0
//----------------------------------------------------------
sbit P0_0       = P0^0;
sbit P0_1       = P0^1;
sbit P0_2       = P0^2;
sbit P0_3       = P0^3;
sbit P0_4       = P0^4;
sbit P0_5       = P0^5;
sbit P0_6       = P0^6;
sbit P0_7       = P0^7;
//----------------------------------------------------------
//----------------------------------------------------------

//----------------------------------------------------------
//  P1  
//----------------------------------------------------------
sbit P1_0       = P1^0;
sbit P1_1       = P1^1;
sbit P1_2       = P1^2;
sbit P1_3       = P1^3;
sbit P1_4       = P1^4;
sbit P1_5       = P1^5;
sbit P1_6       = P1^6;
sbit P1_7       = P1^7;
//----------------------------------------------------------
//----------------------------------------------------------

//----------------------------------------------------------
//  P2 
//----------------------------------------------------------
sbit P2_0       = P2^0;
sbit P2_1       = P2^1;
sbit P2_2       = P2^2;
sbit P2_3       = P2^3;
sbit P2_4       = P2^4;
sbit P2_5       = P2^5;
sbit P2_6       = P2^6;
sbit P2_7       = P2^7;
//----------------------------------------------------------
//----------------------------------------------------------

//----------------------------------------------------------
//  P3 
//----------------------------------------------------------
sbit P3_0       = P3^0;
sbit P3_1       = P3^1;
sbit P3_2       = P3^2;
sbit P3_3       = P3^3;
sbit P3_4       = P3^4;
sbit P3_5       = P3^5;
sbit P3_6       = P3^6;
sbit P3_7       = P3^7;
//----------------------------------------------------------
//----------------------------------------------------------

//----------------------------------------------------------
//  P4 
//----------------------------------------------------------
sbit P4_0       = P4^0;
sbit P4_1       = P4^1;
sbit P4_2       = P4^2;
sbit P4_3       = P4^3;
sbit P4_4       = P4^4;
sbit P4_5       = P4^5;
sbit P4_6       = P4^6;
sbit P4_7       = P4^7;
//----------------------------------------------------------
//----------------------------------------------------------
#endif

#endif

