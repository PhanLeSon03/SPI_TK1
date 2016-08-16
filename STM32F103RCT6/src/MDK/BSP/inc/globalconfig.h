/**************************************************************************************************************
* Copyright 2009 TEKBOX DIGITAL SOLUTIONS Company
* All Rights Reserved
*
* Module name	: global.h
* Author			: Phung The Vu.
* Version            	: V1.0.0
* Date               	: 17/05/2010
*
* Module Description:
* Global define.
*
**************************************************************************************************************/
#ifndef _GLOBAL_H_
#define _GLOBAL_H_


#include "stm32f10x.h"


//*****************************************************************************
//
// Global type define 
//
//*****************************************************************************

#ifndef BYTE
typedef unsigned char BYTE; 
#endif

#ifndef	BOOL
typedef unsigned char BOOL; 
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef TRUE
#define  TRUE		1
#endif

#ifndef FALSE
#define  FALSE		0
#endif

typedef char (*funcptr)(void); 

typedef void (*notify_funcptr)(u8 *msg, u16 msg_len); 




//*****************************************************************************
//
// Debug define  
//
//*****************************************************************************


//*****************************************************************************
//
// const define 
//
//*****************************************************************************

#define CR  0x0D  //  carriage return 
#define LF  0x0A  // line feed   

#endif //  _GLOBAL_H_

