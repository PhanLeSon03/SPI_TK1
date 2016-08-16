/**************************************************************************************************************
* Copyright 2009 TEKBOX DIGITAL SOLUTIONS Company
* All Rights Reserved
*
* Module name	: platform_config.h
* Author			: Phung The Vu.
* Version            	: V1.0.0
* Date               	: 17/05/2010
*
* Module Description:
* platform specific configuration file..
*
**************************************************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PLATFORM_CONFIG_H
#define __PLATFORM_CONFIG_H

/* Includes ------------------------------------------------------------------*/
/* Application includes. */
#include "appconfig.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define T23_DEVICE

// SPI hw define
#define GPIO_CS                  					GPIOB
#define GPIO_Pin_CS              					GPIO_Pin_12 
#define RCC_APB2Periph_GPIO_CS   			RCC_APB2Periph_GPIOB

#define GPIO_WP                  					GPIOB
#define GPIO_Pin_WP              					GPIO_Pin_8 
#define RCC_APB2Periph_GPIO_WP   			RCC_APB2Periph_GPIOB

#define GPIO_HOLD                  					GPIOB
#define GPIO_Pin_HOLD              				GPIO_Pin_9 
#define RCC_APB2Periph_GPIO_HOLD   			RCC_APB2Periph_GPIOB

// USB hw define
#define USB_DISCONNECT            				GPIOC
#define USB_DISCONNECT_PIN        			GPIO_Pin_4
#define RCC_APB2Periph_GPIO_DISCONNECT      RCC_APB2Periph_GPIOC

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __PLATFORM_CONFIG_H */

