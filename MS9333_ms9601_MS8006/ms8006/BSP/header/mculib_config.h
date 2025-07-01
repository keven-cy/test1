/**
******************************************************************************
* @file    mculib_config.h
* @author  Weiwei Cheng
* @version V1.0.0
* @date    14-Aug-2020
* @brief   This file include user's configuration. 
*                    Configure step:
*                    1. Config MCU type and FOSC
*                    2. Choose used modules according to the real  circuit board
*
* Copyright (c) 2020, MacroSilicon Technology Co.,Ltd.   
******************************************************************************/
#ifndef __MCULIB_CONFIG_H
#define __MCULIB_CONFIG_H


#ifndef _PLATFORM_ARM_
#define _PLATFORM_ARM_ //for mculib_typedef
#endif


#define USE_DEBUGGING_LOG         //if not use uart printf log, should " //#define"
#define USE_DEBUGGING_INTERACTION //if not use uart debug, should " //#define"

#endif
