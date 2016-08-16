#include "usbInf.h"

#include "stm32f10x_it.h"
#include "usb_lib.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_hw_config.h"
#include "platform_config.h"
#include "usb_pwr.h"
#include "usb_istr.h"
#include <string.h>

/* ----------------------------------------- local define ----------------------------------------*/








/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/




/*******************************************************************************
* Function Name  	: USB_HP_CAN_TX_IRQHandler
* Return         	: None
* Parameters 		: None
* Description		: This function handles USB High Priority or CAN TX interrupts requests.
* Notes			: 
*******************************************************************************/
void USB_HP_CAN_TX_IRQHandler(void)
{

}

/*******************************************************************************
* Function Name  	: USB_LP_CAN_RX0_IRQHandler
* Return         	: None
* Parameters 		: None
* Description		: This function handles USB Low Priority or CAN RX0 interrupts requests.
* Notes			: 
*******************************************************************************/
void USB_LP_CAN_RX0_IRQHandler(void)
{
	USB_Istr();
}

/*******************************************************************************
* Function Name  	: usbInfRecieveCmd
* Return         	: None
* Parameters 		: None
* Description		: Recive a command from USB Virtual COM Interface 
* Notes			: 
*******************************************************************************/
u8 usbInfRecieveCmd( u8 * cmd_buf)
{
	u8 c = 0;
	/* reset cmd buffer */
	u8 cmd_len = 0;

	do{
		c = usbInfRecieveByte();
		if ( c == CR ){
			c = usbInfRecieveByte();			
			if(c == LF) /* end of command */
				break;
			else{
				cmd_buf[cmd_len++] = CR;
				cmd_buf[cmd_len++] = c;
			}
		}else{
			cmd_buf[cmd_len++] = c;
		}
	} while(cmd_len  < MAX_CMD_BUFFER_LEN -1);

	cmd_buf[cmd_len++] = 0;
	
	if(cmd_len >= MAX_CMD_BUFFER_LEN )
		cmd_len = 0;

	return cmd_len;

}


