
/* define macro -----------------------------------------------------------*/
//#define CT103_TXD 
//#define CT104_RXD
//#define CT105_RTS
//#define CT106_CTS
//#define CT108_DTR
//#define CT107_DSR
//#define CT109_DCD
//#define CT125_RI

//GSM hw define
#define GSM_RTS                                  GPIO_Pin_0  //PA0  --> Request To Send
#define GSM_CTS                                  GPIO_Pin_1  //PA1  --> Clear To Send
#define GSM_RX                                   GPIO_Pin_2  //PA2  --> Receive Data
#define GSM_TX                                   GPIO_Pin_3  //PA3  --> Transmit Data
#define GSM_RI                                   GPIO_Pin_8  //PA8  --> Ring Indicate
#define GSM_ONOFF                                GPIO_Pin_0  //PC0  --> ON/OFF
#define GSM_LED                                  GPIO_Pin_8  //PC8  --> indicate LED for GSM
#define GSM_SOS                                  GPIO_Pin_10 //PC10 --> SOS/Power phone, input button

#define GSM_STBUFFSIZETX 255
#define GSM_STBUFFSIZERX 255


/* Define functions --------------------------------------------------------- */
//void GSM_SendStr(const u8 * Str, u16 len);
void GSM_Init(void);
u8 *GSM_Recieve(void);
void GSM_Transmit(u8 *Data, u16 len);


//#define TEST_GSM         \
//GetInputString(buffer);                                                                      \
//GSM_Transmit(buffer,0) ;                                                                    \
//buffer1 = GSM_Recieve() ;                                                                   \
//SerialPutString(buffer1,0) ;     \
  
