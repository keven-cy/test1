/**
******************************************************************************
* @file    mculib_key.c
* @author  Weiwei Cheng
* @version V1.0.0
* @date    14-Aug-2020
* @brief   This file is the key interface source code.  
*
* Copyright (c) 2018, MacroSilicon Technology Co.,Ltd.   
******************************************************************************/
#include "..\header\mculib_common.h"

/*key module port & pin*/
#define KEY_IO_PORT         (GPIOC) //config by user
#define KEY_INPUT_PINS      (GPIO_PIN_6|GPIO_PIN_5|GPIO_PIN_4) // kyq

//if the key pins in the differnet port
#define PIN_KEY0 ((UINT8)HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6)) //config by user
#define PIN_KEY1 ((UINT8)HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5))
#define PIN_KEY2 ((UINT8)HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_4))

//#define DIAL_SWITCH1 ((UINT8)HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1))

#define KEY_SWITCH0         (0x01)
#define KEY_SWITCH1         (0x02)
#define KEY_SWITCH2         (0x04)
#define KEY_SWITCH3         (0x08)
#define KEY_SWITCH4         (0x10)
#define KEY_SWITCH5         (0x20)
#define KEY_SWITCH6         (0x40)
#define KEY_SWITCH7         (0x80)

#define KEY_GLOBAL_SWITCH   (KEY_SWITCH0 | KEY_SWITCH1|KEY_SWITCH2) //config by user

static UINT8  g_u8keyTrigger;
static UINT8  g_u8keyCount = KEY_GLOBAL_SWITCH;
static UINT16 g_u16keyTimes = 0;
static BOOL   g_bAntiShakeFlag = TRUE;

#if 1
VOID mculib_key_init(VOID)
{
//    // config GPIO mode if need
//    GPIO_InitTypeDef GPIO_InitStruct = {0};
//    GPIO_InitStruct.Pin = KEY_INPUT_PINS;
//    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//	GPIO_InitStruct.Pull = GPIO_PULLUP;
//    muclib_gpio_init(KEY_IO_PORT, &GPIO_InitStruct);
}

UINT8 mculib_key_detect(VOID)
{
    UINT8  u8keyPressed = KEY_NONE;

#if 0
    UINT8 u8keyVal = KEY_IO_PORT & KEY_GLOBAL_SWITCH; 
#else //if the key pin in the differnet port
    UINT8 u8keyVal = (PIN_KEY0 << 0 | PIN_KEY1<<1| PIN_KEY2<<2) & KEY_GLOBAL_SWITCH;
#endif
    
    g_u8keyTrigger = u8keyVal & (u8keyVal ^ g_u8keyCount);
    g_u8keyCount   = u8keyVal;
    
    if (g_bAntiShakeFlag)
    {
        switch(g_u8keyTrigger)
        {
        case KEY_SWITCH0:
            u8keyPressed = KEY_MAP_SHORT0;
            break;
        case KEY_SWITCH1:
            u8keyPressed = KEY_MAP_SHORT1;
            break;
        case KEY_SWITCH2:
//            u8keyPressed = KEY_MAP_SHORT2;
            break;
        default:
            break;
        }
    }
    
    //reset long key time count
    if(g_u8keyTrigger != 0x00)
    {
        g_u16keyTimes = 0;
        g_bAntiShakeFlag = TRUE;
    }
    
    #if 1 //if support continuity key
    // key1 continuity pressed sample
    if ((g_u8keyCount & KEY_SWITCH0) == 0)
    {
        g_u16keyTimes ++;
        if (g_u16keyTimes > KEY_RESCAN_SINGLE)
        {
            u8keyPressed  = KEY_MAP_CONTINUITY0;
            g_u16keyTimes = KEY_RESCAN_SINGLE - KEY_RESCAN_CONTINUITY;
            g_bAntiShakeFlag = FALSE;
        }
    }
    #endif
    
    #if 1 //if support long key
    // key6 long pressed sample
    if ((g_u8keyCount & KEY_SWITCH1) == 0)
    {
        if(g_bAntiShakeFlag)
        {
            g_u16keyTimes ++;
            if (g_u16keyTimes > KEY_RESCAN_LONG)
            {
                u8keyPressed  = KEY_MAP_LONG1;
                g_u16keyTimes = 0;
                g_bAntiShakeFlag = FALSE;
            }
        }
    }
    #endif
    
    return u8keyPressed;
}
#endif

#if 0
BOOL mculib_dial_switch_deal(VOID)
{
    return DIAL_SWITCH1;
}
#endif
