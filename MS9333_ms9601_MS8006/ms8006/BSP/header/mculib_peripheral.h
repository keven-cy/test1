/**
******************************************************************************
* @file    mculib_peripheral.h
* @author  Weiwei Cheng
* @version V1.0.0
* @date    14-Aug-2020
* @brief   This file defines the peripheral interface driver function.  
*
* Copyright (c) 2018, MacroSilicon Technology Co.,Ltd.   
******************************************************************************/
#ifndef __MCULIB_PERIPHERAL_H
#define __MCULIB_PERIPHERAL_H

//reset for macrosilicon chip
VOID mculib_chip_reset(VOID);

BOOL mculib_chip_read_interrupt_pin(VOID);

//led control
VOID mculib_led_control(UINT8 u8LedNum, BOOL bLight);
VOID mculib_hdmi_rx_led_control(BOOL b_on);
VOID mculib_hdmi_tx_led_control(UINT8 u8_channel, BOOL b_on);
VOID mculib_led_init(void);
VOID mculib_MUTE_init(void);
void mculib_chip_reset_ms9601(void);
void mculib_Led_toggle(void);
#endif
