#include "langconfig.h"

sentence_idx_t sentence_idx ;
uint32_t AlternateLanguage= EXT_FLASH_FILE_ADDR;

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/
uint8_t  GetSentence(int8_t * str, uint32_t lang_addr,  sentence_idx_t idx)
{
	int32_t i;
	uint16_t j,k;
	uint8_t ReadChar[2],NoIndex[8],iIndex=0;
	int32_t Index=0;
	j=0;		
	lang_addr = lang_addr + FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2 + 30;
	for(i=0;j<180;i++)
	{
		SPI_FLASH_BufferRead(ReadChar, lang_addr +i , 1);
		if ((ReadChar[0]>= 0x30)&(ReadChar[0]<=0x39))
		{
			NoIndex[iIndex++] = ReadChar[0];
		}
		if (((ReadChar[0] == 0x20)|(ReadChar[0] == '\n'))&(iIndex != 0))
		{
			NoIndex[iIndex++] = '\0';
			iIndex = 0;
			Str2Int(NoIndex,&Index);
		}
		if(ReadChar[0] == '|')
		{
			j++;
			if ((uint8_t)Index == idx)
			{
				/* ---------------------Get String ----------------------*/
				i++;
				SPI_FLASH_BufferRead(ReadChar, lang_addr + i , 1);
				for (k=0;ReadChar[0] != '\r'; )
				{
					str[k] = ReadChar[0];
					SPI_FLASH_BufferRead(ReadChar, lang_addr + i + (++k), 1);
				}	
				str[k] = '\0';
				break;
			}
		}
	}
return k;
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : None
* Return         : 0 --> invalid command
*******************************************************************************/
#if 0 // Search and count index to find position of command
uint8_t GetIndex(int8_t *buffer,uint32_t lang_addr)
{
	int8_t str[100],ReadChar[2];
	uint32_t i;
	uint8_t j,k;
	j=0;

	lang_addr = lang_addr + FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2 +100;
	for(i=0;j<90;i++)
	{
		SPI_FLASH_BufferRead(ReadChar, lang_addr +i , 1);
		if(ReadChar[0] == '#')
		{
			i++;
			for(;j<90;i++)
			{
				SPI_FLASH_BufferRead(ReadChar, lang_addr +i , 1);
				if(ReadChar[0] == ':')
				{
					j++;
					if(j%2)
					{
						i = i + 2;
						SPI_FLASH_BufferRead(ReadChar, lang_addr + i , 1);
						for (k=0;ReadChar[0]!= '"' ; )
						{
							str[k] = ReadChar[0];
							SPI_FLASH_BufferRead(ReadChar, lang_addr + i + (++k), 1);
						}
						str[k] = '\0';
						i = i + k;
						
						if(strcmp(buffer,str)==0)
						{
							return (j/2 + 1);
						}
					}
				}
			}
		}
		
	}
return 0;
}
#else
uint8_t GetIndex(int8_t *buffer,uint32_t lang_addr)
{
	int8_t str[100],ReadChar[2],NoIndex[8];
	uint32_t i;
	int32_t Index;
	uint8_t j,k,iIdex=0;
	j=0;

	lang_addr = lang_addr + FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2 +100;
	for(i=0;j<90;i++)
	{
		SPI_FLASH_BufferRead(ReadChar, lang_addr +i , 1);
		if(ReadChar[0] == '#')
		{
			i++;
			for(;j<90;i++)
			{
				SPI_FLASH_BufferRead(ReadChar, lang_addr +i , 1);
				if ((ReadChar[0]>= 0x30)&(ReadChar[0]<=0x39))
				{
					NoIndex[iIdex++] = ReadChar[0];
				}
				if ((ReadChar[0] == 0x20|ReadChar[0]=='\n')&(iIdex != 0))
				{
					NoIndex[iIdex++] = '\0';
					iIdex = 0;
				}
				if(ReadChar[0] == ':')
				{
					j++;
					if(j%2)
					{
						i = i + 2;
						SPI_FLASH_BufferRead(ReadChar, lang_addr + i , 1);
						for (k=0;ReadChar[0]!= '"' ; )
						{
							str[k] = ReadChar[0];
							SPI_FLASH_BufferRead(ReadChar, lang_addr + i + (++k), 1);
						}
						str[k] = '\0';
						i = i + k;
						
						if(strcmp(buffer,str)==0)
						{
							Str2Int(NoIndex, &Index);
							return Index;
						}
					}
				}
			}
		}
		
	}
return 0;
}

#endif
/*******************************************************************************
* Function Name  : 
* Description    : get position of option command, example 1-->ON 2-->OFF
* Input          : 
* Output         : None
* Return         : 0---> Invalid command
*******************************************************************************/
#if 0
uint8_t GetIndex_Option(int8_t *buffer,uint32_t lang_addr,uint8_t Pos_Command)
{
	int8_t str[100],ReadChar[2],Buf_Option[30],lex[20];
	uint32_t i;
	uint8_t j,k,Position=0;
	j=0;

	lang_addr = lang_addr + FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2 +100;
	for(i=0;j<90;i++)
	{
		SPI_FLASH_BufferRead(ReadChar, lang_addr +i , 1);
		if(ReadChar[0] == '#')
		{
			i++;
			for(;j<90;i++)
			{
				SPI_FLASH_BufferRead(ReadChar, lang_addr +i , 1);
				if(ReadChar[0] == ':')
				{
					j++;
					if(j==Pos_Command*2)
					{
						i = i + 1;
						do
						{
							i++;
							SPI_FLASH_BufferRead(ReadChar, lang_addr + i , 1);
							for (k=0;(ReadChar[0]!= '"')&(ReadChar[0]!= '/') ; )
							{
								str[k] = ReadChar[0];
								SPI_FLASH_BufferRead(ReadChar, lang_addr + i + (++k), 1);
							}
							str[k] = '\0';
							i = i + k;
							Position++;
						}
						while(strcmp(buffer,str)!=0);
						
						
						return Position;
					}
				}
			}
		}
		
	}
return 0;
}
#else
uint8_t GetIndex_Option(int8_t *buffer,uint32_t lang_addr,uint8_t Pos_Command)
{
	int8_t str[100],ReadChar[2],Buf_Option[30],lex[20],NoIndex[8];
	uint32_t i;
	int32_t Index;
	uint8_t j,k,Position=0,iIdex=0,Flag_Even;
	j=0;

	lang_addr = lang_addr + FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2 +100;
	for(i=0;j<50;i++)
	{
		SPI_FLASH_BufferRead(ReadChar, lang_addr +i , 1);
		if(ReadChar[0] == '#')
		{
			i++;
			for(;j<50;i++)
			{
				SPI_FLASH_BufferRead(ReadChar, lang_addr +i , 1);
				if ((ReadChar[0]>= 0x30)&(ReadChar[0]<=0x39))
				{
					NoIndex[iIdex++] = ReadChar[0];
				}
				if ((ReadChar[0] == 0x20|ReadChar[0]=='\n')&(iIdex != 0))
				{
					NoIndex[iIdex++] = '\0';
					iIdex = 0;
					Str2Int(NoIndex,&Index);
					j++;
				}
				if(ReadChar[0] == ':')
				{
					Flag_Even++;
					if ((Pos_Command == (uint8_t)Index)&(Flag_Even%2 == 0))
					{
						i = i + 1;
						do
						{
							i++;
							SPI_FLASH_BufferRead(ReadChar, lang_addr + i , 1);
							for (k=0;(ReadChar[0]!= '"')&(ReadChar[0]!= '/') ; )
							{
								str[k] = ReadChar[0];
								SPI_FLASH_BufferRead(ReadChar, lang_addr + i + (++k), 1);
							}
							str[k] = '\0';
							i = i + k;
							Position++;
						}
						while((strcmp(buffer,str)!=0)&(Position <=5));
						return Position;
					}
				}
			}
		}
		
	}
return 0;
}
#endif	
/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/
void Language_OnOff(u8 AlternateOption,int8_t *str)
{
	uint8_t abc,def;
	
#if 0
	Len = strlen(Command);
	Int2Str(lex, Len);
	/*----------Following command can use to select command via Index command---*/
	for(i=0;i<Len;i++)
	{
		for (;j==0;i++)
		{
			if(Command[i] == ',')
			{
				Syntax[i] = '\0';
				i++;
				break;
			}
			Syntax[i] = Command[i];
		}
		Optional[j++] = Command[i];
	}
	Optional[j] = '\0';
	/* Get position of the command in language file  */
	Position = GetIndex(Syntax,AlternateLanguage);
	/* Get index of option of command -----------*/
	AlternateOption = GetIndex_Option(Optional, AlternateLanguage,Position);
#endif	
	if (AlternateOption == 1)
	{
		AlternateLanguage = EXT_FLASH_FILE_ADDR2;
	}
	else if (AlternateOption== 2)
	{
		AlternateLanguage = EXT_FLASH_FILE_ADDR;
	}
	/*---- Respon note to user ----------------------*/
	GetCommand(str, AlternateLanguage,  12);
	def =strlen(str);
	*(str + def)= ',';
	if(AlternateOption == 1)
	{
		GetSentence(str + def + 1, AlternateLanguage, turned_on_idx);
	}
	else
	{
		GetSentence(str + def + 1, AlternateLanguage, turned_off_idx);
	}
	abc = strlen(str);
	*(str +abc)= ',';
	GetSentence(str+abc+1, AlternateLanguage, Jan_idx);	
}

int8_t StringCompare(int8_t *String1, int8_t *String2)
{
	uint16_t  Len1,Len2,i;
	Len1 = strlen(String1);
	Len2 = strlen(String2);
	int8_t String3[10],String4[10];
	if (Len1 != Len2)
	{
		//SerialPutString("\r\n===============buffer=================\r\n",0);
		//SerialPutString(String1,0);
		//SerialPutString("\r\n",0);
		//SerialPutString(String2,0);
		//SerialPutString("\r\n=====================================\r\n",0);
		
		return -1;//String not equal
	}
	
	for(i=0; i<Len1; i++)
	{
		if ((String1[i]!=String2[i]))
		{
			return -1;
		}
	}

	return 1;//match string
		
}

/*******************************************************************************
* Function Name  : 
* Description    : 
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/
#if 0
uint8_t  GetCommand(int8_t * str, uint32_t lang_addr,  command_idx_t idx)
{
	int8_t ReadChar[2];
	uint32_t i;
	uint8_t j,k;
	j=0;

	lang_addr = lang_addr + FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2 +100;
	for(i=0;j<90;i++)
	{
		SPI_FLASH_BufferRead(ReadChar, lang_addr +i , 1);
		if(ReadChar[0] == '#')
		{
			i++;
			for(;j<90;i++)
			{
				SPI_FLASH_BufferRead(ReadChar, lang_addr +i , 1);
				if(ReadChar[0] == ':')
				{
					j++;
					if((j+1)/2== idx)
					{
						i = i + 2;
						SPI_FLASH_BufferRead(ReadChar, lang_addr + i , 1);
						for (k=0;ReadChar[0]!= '"' ; )
						{
							str[k] = ReadChar[0];
							SPI_FLASH_BufferRead(ReadChar, lang_addr + i + (++k), 1);
						}
						str[k] = '\0';
			                   break;
					}
				}
			}
			break;
		}
		
	}
return 0;
}
#else
uint8_t  GetCommand(int8_t * str, uint32_t lang_addr,  command_idx_t idx)
{
	int8_t ReadChar[2],NoIndex[8];
	uint8_t Flag_Odd=0;
	uint32_t i;
	uint8_t j,k,iIdex=0;
	int32_t Index;
	j=0;

	lang_addr = lang_addr + FILE_NAME_LENGTH + FILE_SIZE_LENGTH + 2 +100;
	for(i=0;j<90;i++)
	{
		SPI_FLASH_BufferRead(ReadChar, lang_addr +i , 1);
		if(ReadChar[0] == '#')
		{
			i++;
			for(;j<90;i++)
			{
				SPI_FLASH_BufferRead(ReadChar, lang_addr +i , 1);
				if ((ReadChar[0]>= 0x30)&&(ReadChar[0]<=0x39))
				{
					NoIndex[iIdex++] = ReadChar[0];
				}
				if (((ReadChar[0] == 0x20)||(ReadChar[0] =='\n'))&&(iIdex != 0))
				{
					NoIndex[iIdex++] = '\0';
					iIdex = 0;
					Str2Int(NoIndex,&Index);
					//j++;
				}
				if(ReadChar[0] == ':')
				{
	   				j++;
	   				Flag_Odd++;
	   				if((idx == Index)&&(Flag_Odd%2 == 1))
	   				{
	   					i = i + 2;
	   					SPI_FLASH_BufferRead(ReadChar, lang_addr + i , 1);
	   					for (k=0;ReadChar[0]!= '"' ; )
	   					{
	   						str[k] = ReadChar[0];
	   						SPI_FLASH_BufferRead(ReadChar, lang_addr + i + (++k), 1);
	   					}
	   					str[k] = '\0';
	   		                   break;
	   				}
				}
			}
			break;
		}
		
	}
return 0;
}
#endif

u8 GetIndexCommand(u8 *Command,u8 *AlternateOption)
{
	uint8_t Len,i,j=0,Position,abc,def;
	int8_t Syntax[50],Optional[20],lex[20];

	Len = strlen(Command);
	Int2Str(lex, Len);
	/*----------Following command can use to select command via Index command---*/
	for(i=0;i<Len;i++)
	{
		for (;((j==0)&(i<Len));i++)
		{
			if(Command[i] == ',')
			{
				Syntax[i] = '\0';
				i++;
				break;
			}
			Syntax[i] = Command[i];
		}
		Optional[j++] = Command[i];
	}
	Optional[j] = '\0';
	
	if (strlen(Syntax)> Len)
	{
		Position = 0;
		*AlternateOption = 0;
	}
	else
	{
		/* Get position of the command in language file  */
		Position = GetIndex(Syntax,AlternateLanguage);
		/* Get index of option of command -----------*/
		*AlternateOption = GetIndex_Option(Optional, AlternateLanguage,Position);
	}
	return Position;
}