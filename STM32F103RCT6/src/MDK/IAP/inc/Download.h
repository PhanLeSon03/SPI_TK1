#include "ymodem.h"
#include "Spi_flash_inf.h"

#define FILE_LOG_SIZE 0x10000
#ifndef LENGTH_SECTOR
#define LENGTH_SECTOR 0x10000
#endif

#define BUFFER_SECTOR 0x00000
#define LENGTH_GPS_GSM_FILE 0x100000
#define LAST_ADDR_MEM           0x7FFFFF
#define ADDR_GSM_FILE            0x400000
extern uint8_t file_name[FILE_NAME_LENGTH];
extern Ymodem_Receive_Process dfuDevice, extDownLoadDevice;
extern Ymodem_Transmit_Process extUpLoadDevice;
extern Ymodem_Transmit_Process exUpLoadDeviceOpposite;
/* Private function prototypes -----------------------------------------------*/
void extFlashLoader(void);
void extSerialDownload(uint32_t Addr); 
void extSerialUpload(uint32_t Addr); 
void extSerialUploadOpposite(uint32_t Addr);
int8_t AppendFile (uint8_t* Buf, uint32_t Addr, uint8_t *FileName, uint16_t NumByteToWrite);
int8_t AppendFileOpposite (uint8_t *Buf, uint32_t Addr, uint8_t *FileName, uint16_t NumByteToWrite);
uint8_t WriteFile_GPS_GSM(uint8_t *Data, int16_t NumByteToWrite, uint8_t SelectFile);
int8_t AppendFileGPSOpposite (uint8_t *Buf, uint16_t NumByteToWrite);
int8_t AppendFileGSM (uint8_t *Buf, uint16_t NumByteToWrite);
void ClearFile_GSM(void);
void ClearFile_GPS(void);

void FixData(uint8_t *FileName, uint32_t Addr_File, uint32_t Offset,uint8_t *NewData, uint16_t NumByteToFix);
void ReadLogFile(uint8_t *Buf, uint8_t *FileName, uint32_t Addr,uint32_t First_Addr, uint32_t Last_Addr);