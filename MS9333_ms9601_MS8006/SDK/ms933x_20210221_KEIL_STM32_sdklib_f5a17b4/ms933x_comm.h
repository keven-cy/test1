/**
******************************************************************************
* @file    ms933x_comm.h
* @author  
* @version V1.0.0
* @date    15-Nov-2014
* @brief   This file contains all header files
* @history
*
* Copyright (c) 2009-2014, MacroSilicon Technology Co.,Ltd.
******************************************************************************/
#ifndef __MACROSILICON_MS933X_COMMON_H__
#define __MACROSILICON_MS933X_COMMON_H__

#if !defined (_PLATFORM_ARM_) && !defined (_PLATFORM_WINDOWS_) && !defined (__KEIL_C__) && !defined (__CSMC__) && !defined (_IAR_) && !defined (__STD_GCC__)
#error "Unknown compiler!"
#endif

#if defined (_PLATFORM_WINDOWS_)
#ifndef MS933X_DRV_EXTERN
    #define MS933X_DRV_EXTERN extern
#endif

#ifndef MS933X_DRV_API
    #define MS933X_DRV_API  __declspec(dllexport)
#endif

#ifndef MS933X_MW_EXTERN
    #define MS933X_MW_EXTERN extern
#endif

#ifndef MS933X_MW_API
    #define MS933X_MW_API  __declspec(dllexport)
#endif

#else //

#ifndef MS933X_DRV_EXTERN
    #define MS933X_DRV_EXTERN extern
#endif

#ifndef MS933X_DRV_API
    #define MS933X_DRV_API  
#endif

#ifndef MS933X_MW_EXTERN
    #define MS933X_MW_EXTERN extern
#endif

#ifndef MS933X_MW_API
    #define MS933X_MW_API 
#endif

#endif //end

//
#if defined (_PLATFORM_ARM_)
#include <stdio.h>
#include <math.h>
#include <string.h>

#elif defined (__STD_GCC__)
#include <stdio.h>
#include <math.h>
#include <string.h>

#elif defined (_PLATFORM_WINDOWS_)
#include <stdio.h>  
#include <math.h>
#include <string.h>
#include <windows.h>

#elif defined (__KEIL_C__)
#include <intrins.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <absacc.h>

#elif defined (__CSMC__)
#include <string.h>
#include <stdio.h>

#elif defined (_IAR_)
#include <string.h>
#include <stdio.h>

#endif


//sdk common header file include
#include "ms933x_typedef.h"
#include "ms933x_mpi_dummy.h"


#endif //__MACROSILICON_MS933X_COMMON_H__
