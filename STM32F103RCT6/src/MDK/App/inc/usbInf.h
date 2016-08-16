#ifndef _USB_INF_H_
#define _USB_INF_H_

#include "usb_lib.h"
#include "usb_prop.h"
#include "usb_pwr.h"

#define MAX_CMD_BUFFER_LEN	20

/*******************************************************************************
* Function Name  	: usbInfInit
* Return         	: None
* Parameters 		: None
* Description		: Init USB Virtual COM Interface 
* Notes			: 
*******************************************************************************/
void usbInfInit(void);

/*******************************************************************************
* Function Name  	: usbInfRxBufFlush
* Return         	: None
* Parameters 		: None
* Description		: Flush buffer for USB reception 
* Notes			: 
*******************************************************************************/
extern void usbInfRxBufFlush(void);

/*******************************************************************************
* Function Name  	: usbInfRxCount
* Return         	: None
* Parameters 		: None
* Description		: the number of valid data in rx buffer
* Notes			: 
*******************************************************************************/
extern uint32_t usbInfRxCount(void);

/*******************************************************************************
* Function Name  	: usbInfRecieveCmd
* Return         	: None
* Parameters 		: None
* Description		: Recive a command from USB Virtual COM Interface 
* Notes			: 
*******************************************************************************/
u8 usbInfRecieveCmd( u8 * cmd_buf);

/*******************************************************************************
* Function Name  	: usbInfRecieveCmd
* Return         	: None
* Parameters 		: None
* Description		: Recive a byte from USB Virtual COM Interface 
* Notes			: 
*******************************************************************************/
extern u8 usbInfRecieveByte(void);

#endif //_USB_INF_H_
