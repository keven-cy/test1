/**
******************************************************************************
* @file    mculib_typedef.h
* @author  Weiwei Cheng
* @version V1.0.0
* @date    14-Aug-2020
* @brief   
*
* Copyright (c) 2018, MacroSilicon Technology Co.,Ltd.   
******************************************************************************/
#ifndef __MCULIB_TYPEDEF_H
#define __MCULIB_TYPEDEF_H

#ifndef NULL
#define NULL ((void*)0)
#endif

//#define  __CODE
#define  __XDATA
#define  __DATA
#define  __IDATA
#define  __NEAR
#ifndef  __IO
#define  __IO
#endif

typedef _Bool BOOL;

//unsigned integer type
typedef unsigned char UINT8;
typedef char          CHAR;
typedef unsigned short UINT16;

//signed integer type
typedef signed char INT8;
typedef signed short INT16;

//32bit type
typedef unsigned int UINT32;
typedef signed int INT32;

#define VOID void

#define FALSE 0
#define TRUE  1

#define DISABLE 0
#define ENABLE  1

#define LOW     0
#define HIGH    1

#define OFF     0
#define ON      1

// Helper macros.
#define _UNUSED_(arg)     ((arg) = (arg))

#ifndef _countof
#define _countof(ARRAY) (sizeof(ARRAY) / sizeof(ARRAY[0]))
#endif

#ifndef max
#define max(a, b)   (((a)>(b))?(a):(b)) 
#endif

#ifndef min
#define min(a, b)   (((a)<(b))?(a):(b))
#endif


#endif

