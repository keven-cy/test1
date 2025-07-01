/**
******************************************************************************
* @file    mculib_key.h
* @author  Weiwei Cheng
* @version V1.0.0
* @date    14-Aug-2020
* @brief   This file defines the key interface driver function.  
*
* Copyright (c) 2018, MacroSilicon Technology Co.,Ltd.   
******************************************************************************/
#ifndef __MCULIB_KEY_H
#define __MCULIB_KEY_H


typedef enum _KEY_MAP_
{
    KEY_MAP_SHORT0 = 0,
    KEY_MAP_SHORT1,
    KEY_MAP_SHORT2,
    KEY_MAP_SHORT3,
    KEY_MAP_SHORT4,
    KEY_MAP_SHORT5,
    KEY_MAP_SHORT6,
    KEY_MAP_SHORT7,
    KEY_MAP_CONTINUITY0,
    KEY_MAP_CONTINUITY1,
    KEY_MAP_CONTINUITY2,
    KEY_MAP_CONTINUITY3,
    KEY_MAP_CONTINUITY4,
    KEY_MAP_CONTINUITY5,
    KEY_MAP_CONTINUITY6,
    KEY_MAP_CONTINUITY7,
    KEY_MAP_LONG0,
    KEY_MAP_LONG1,
    KEY_MAP_LONG2,
    KEY_MAP_LONG3,
    KEY_MAP_LONG4,
    KEY_MAP_LONG5,
    KEY_MAP_LONG6,
    KEY_MAP_LONG7,
    
    KEY_NONE = 0xFF
} KEY_MAP_E;

/* count scan time. per time base on APP */
/* 50 */
#define KEY_RESCAN_LONG         (60)    //suggest 3s, if key detect polling time is 10ms, 3s = 60 * 50ms
#define KEY_RESCAN_SINGLE       (40)    //suggest 2s, if key detect polling time is 10ms, 2s = 40 * 50ms
#define KEY_RESCAN_CONTINUITY   (1)     //suggest 50ms, if key detect polling time is 10ms, 0.5s = 1 * 50ms


VOID mculib_key_init(VOID);
UINT8 mculib_key_detect(VOID);
BOOL  mculib_dial_switch_deal(VOID);

#endif

