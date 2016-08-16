
if(SPI_FlashInf_IsReady())
	{		
		if(n_ExtFlashPage >= 4)
		{
			SPI_FlashInf_Write( (uint8_t*)RamSource, extFlashRW_Addr, (EXT_FLASH_PAGE_SIZE*4) );
			extFlashRW_Addr += (EXT_FLASH_PAGE_SIZE*4);
			n_ExtFlashPage -= 4;
		}
		else
		{
			SPI_FlashInf_Write( (uint8_t*)RamSource, extFlashRW_Addr, (EXT_FLASH_PAGE_SIZE*n_ExtFlashPage) + r_ExtFlashPage);	
			extFlashRW_Addr += (EXT_FLASH_PAGE_SIZE*n_ExtFlashPage) + r_ExtFlashPage;	// only for testing
		}
	}

      extFlashRW_Addr = EXT_FLASH_FILE_ADDR;
	if(SPI_FlashInf_IsReady())
	{
		SPI_FlashInf_Read(extfile_header, extFlashRW_Addr, FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2);
		extFlashRW_Addr += (FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2);				// remember the position for next write into flash
	}
#include "FileManegement.h"

#define FIRST_ADDR_TABLE      0x00000000
#define FIRST_ADDR_SECTOR    0x00001000
#define FILE_NAME_LENGTH     40
#define REG_STATUS_LENGTH   256
#define REG_NoFILE_LENGTH    4
#define LENGTH_SECTOR          0x00010000
s8 WriteFile(char *buffer_name, char *buffer_data)
{
	char FileName[FILE_NAME_LENGTH];
	char buffer_status[REG_STATUS_LENGTH];                // register status of Flash memory
	char buffer_status_file[REG_STATUS_LENGTH];         //  register status of file is processing
	s8 Flag_ExistFile;
	char n_ExtFlashPage;
	unsigned int NoSectorFree, NoSectorData, AddressSector, iCount;
	FileName = buffer_name;

	Flag_ExistFile = CheckFileExist(FileName,buffer_status_file);
	if (Flag_ExistFile == 1)                         //File has existed in Flash memory
	{
		NoSectorFree = CalculateSectorFree(buffer_status);
		if( NoSectorFree!=0 )              //Flash memory has free space
		{
			NoSectorData = 1;
			if (NoSectorFree >= NoSectorData)                   // Enough free space 
			{
				AddressSector = GetAddressSector(buffer_status);
				
				SetBit_AddrSector(FileName,AddressSector);
				
				if (iCount != NoSectorData)
				{
					iCount++;
					n_ExtFlashPage = 8;
				}
				else
				{
					n_ExtFlashPage = CalculatePage();
					iCount = 0;

				}
				if(SPI_FlashInf_IsReady())
				{		
					if(n_ExtFlashPage >= 4)
					{

						SPI_FlashInf_Write( (uint8_t*)buffer_data, AddressSector, (EXT_FLASH_PAGE_SIZE*4) );
						AddressSector+= (EXT_FLASH_PAGE_SIZE*4);
						n_ExtFlashPage -= 4;
					}
					else
					{
						SPI_FlashInf_Write( (uint8_t*)buffer_data, AddressSector, (EXT_FLASH_PAGE_SIZE*n_ExtFlashPage) + r_ExtFlashPage);	
						AddressSector += (EXT_FLASH_PAGE_SIZE*n_ExtFlashPage) + r_ExtFlashPage;	// only for testing
					}
				}
			}
			else
			{
				return -1;                       //can't write file because not enough free space
			}
		}
	}
	else                                                 // File hasn't existed in Flash memory
	{
		CreateFile(FileName);
		WriteFile(FileName,buffer_data);
	}
}

/* Check Whether File exist in Flash memory */
s8 CheckFileExist(char *FileName, char *buffer_status)
{
	char Search_Name[FILE_NAME_LENGTH];
	char NoFile, i, Flag_check;
	
	SPI_FLASH_BufferRead(&NoFile, FIRST_ADDR_TABLE  , 1);// Get the number of file in Flash memory	
	for (i = 0; i < NoFile; i++)
	{
		SPI_FLASH_BufferRead(Search_Name, FIRST_ADDR_TABLE + 2 + i*(REG_STATUS_LENGTH + FILE_NAME_LENGTH) , FILE_NAME_LENGTH); //Search name of file in Flash memory
		
		if (strcmp(Search_Name,FileName) == 0)  
			{
				Flag_check = 1;
				SPI_FLASH_BufferRead(buffer_status, FIRST_ADDR_TABLE + 2 + i*(REG_STATUS_LENGTH + FILE_NAME_LENGTH ) + FILE_NAME_LENGTH , REG_STATUS_LENGTH);
				break;
			}
		Flag_check = 0;
	}

	if (Flag_check == 1)
		{
			return 1;
		}
	else
		{
			return 0;
		}
}

unsigned int CalculateSectorFree()
{
	char NoFile, i,j ;
	char buf[REG_STATUS_LENGTH],buf_temp[REG_STATUS_LENGTH];
	unsigned int Count;

	for (i=0; i < REG_STATUS_LENGTH; i++)
		{
			buf[i] = 0;
			buf_temp[i] = 0;
		}
	
	SPI_FLASH_BufferRead(&NoFile, FIRST_ADDR_TABLE  , 1);// Get the number of file in Flash memory

	for (i =0; i < NoFile; i++)
		{
			SPI_FLASH_BufferRead(buf, FIRST_ADDR_TABLE + 2 + i*(REG_STATUS_LENGTH + FILE_NAME_LENGTH) + FILE_NAME_LENGTH, REG_STATUS_LENGTH);
			for (j=0; j < REG_STATUS_LENGTH; j++)
				{
					buf_temp[j] |= buf[j];
				}
		}

	for(i = 0; i < REG_STATUS_LENGTH; i++)
		{
			 while (buf_temp[i]) 
			 {
	                     Count += buf_temp[i] | 0x01;
	                     buf_temp[i] >>= 1;
                     }

		}
	Count = REG_STATUS_LENGTH*8 - Count;
	return Count;
}

//unsigned int CalculateSectorData(char *buffer_data)
//{

//}

unsigned int GetAddressSector(char *buffer_status)
{
	char buf[REG_STATUS_LENGTH];
	char i;
      unsigned int Count;

      buf = buffer_status;
	  
	for (i=0; i <REG_STATUS_LENGTH; i++)
		{
			if ((buf[i] | 0xEF) == 0xFF)
				{
					Count++;
				}
			else
				{
					Count = i*8 + Count;
				}
		}
	Count = FIRST_ADDR_SECTOR + Count*LENGTH_SECTOR;
	return Count;
}
void SetBit_AddrSector(char *FileName,char *AddressSector)
{
	char buf[REG_STATUS_LENGTH];
	char i;
      unsigned int Count;

	buf =  

	

	Count = AddressSector;
	Count = (Count - FIRST_ADDR_SECTOR)/(LENGTH_SECTOR);

	SPI_FLASH_BufferRead(buf, FIRST_ADDR_TABLE + REG_NoFILE_LENGTH + i*(REG_STATUS_LENGTH + FILE_NAME_LENGTH) + FILE_NAME_LENGTH, REG_STATUS_LENGTH);

	
	return Count;
}

void CreateFile(char *FileName)
{
	unsigned char NoFile;
	char Name[FILE_NAME_LENGTH];

	Name = FileName;
	Name = FileName;
	NoFile = ReadNoFile();

	WriteNoFile(NoFile + 1);

	WriteFileName(Name,NoFile + 1);

	ResetREG_Status(Name, NoFile + 1);
		
}

void WriteNoFile(unsigned char NoFile)
{
	unsigned char No[1];
	No[0] = NoFile;
	SPI_FlashInf_Write( No, FIRST_ADDR_TABLE, REG_NoFILE_LENGTH);
}

void WriteFileName(char *FileName, char Index)
{

}

void ResetREG_Status(char *FileName, char Index)
{

}

/* Get index of file in Flash memory */
char GetIndexFile(char *FileName)
{
	char NoFile, Count, i;
	char Search_Name[FILE_NAME_LENGTH];
	
	SPI_FLASH_BufferRead(&NoFile, FIRST_ADDR_TABLE  , 1);// Get the number of file in Flash memory	
	for (i = 0; i < NoFile; i++)
	{
		SPI_FLASH_BufferRead(Search_Name, FIRST_ADDR_TABLE + 2 + i*(REG_STATUS_LENGTH + FILE_NAME_LENGTH) , FILE_NAME_LENGTH); //Search name of file in Flash memory
		
		if (strcmp(Search_Name,FileName) == 0)  
			{
				Count = i;
				break;
			}
	}
	
return Count;
}

/* Get status register of file in memory */
char *GetREG_FileStatus(char *FileName)
{
	char Name[FILE_NAME_LENGTH];
	char IndexFile;
	char REG_STATUS[REG_STATUS_LENGTH];
	unsigned int NoFile;
	Name = FileName;

	NoFile = ReadNoFile();

	IndexFile =  GetIndexFile(Name);

	SPI_FLASH_BufferRead(REG_STATUS, FIRST_ADDR_TABLE + 2 + (FILE_NAME_LENGTH+REG_STATUS_LENGTH)*(IndexFile-1) + FILE_NAME_LENGTH, REG_STATUS_LENGTH);

	return REG_STATUS;
}

/* Read number of file in memory flash */
unsigned char ReadNoFile()
{
	unsigned char NoFile;
	SPI_FLASH_BufferRead(&NoFile, FIRST_ADDR_TABLE  , 1);// Get the number of file in Flash memory	

	return NoFile;
}




s8 ReadFile(char *buffer_name, char *buffer_data)
{

}

