#include <stdio.h>
#include "w25q128.h"

#define W25Q_USE_FREERTOS
#ifdef W25Q_USE_FREERTOS
#define W25qxx_Delay(delay) osDelay(delay)
#include "cmsis_os.h"
#else
#define W25qxx_Delay(delay) HAL_Delay(delay)
#endif

#define W25QXX_DUMMY_BYTE        0xA5

w25qxx_t w25qxx;

static uint8_t	W25qxx_Spi(uint8_t Data)
{
	uint8_t	ret;

	HAL_SPI_TransmitReceive(W25QXX_SPI_PTR, &Data, &ret, 1, 100); // spi1

	return ret;
}

static uint32_t W25qxx_ReadID(void)
{
	uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

	W25QFLASH_CS_SELECT;

	W25qxx_Spi(W25_GET_JEDEC_ID);

	Temp0 = W25qxx_Spi(W25QXX_DUMMY_BYTE);
	Temp1 = W25qxx_Spi(W25QXX_DUMMY_BYTE);
	Temp2 = W25qxx_Spi(W25QXX_DUMMY_BYTE);

	W25QFLASH_CS_UNSELECT;

	Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

	return Temp;
}

/* void W25qxx_ReadUniqID(void)
{
	W25QFLASH_CS_SELECT;
	W25qxx_Spi(W25_READ_UNIQUE_ID);
	for(uint8_t	i = 0; i < 4; i++)
		W25qxx_Spi(W25QXX_DUMMY_BYTE);
	for(uint8_t	i = 0; i < 8; i++)
		w25qxx.UniqID[i] = W25qxx_Spi(W25QXX_DUMMY_BYTE);
	W25QFLASH_CS_UNSELECT;
} */

static void W25qxx_WriteEnable(void)
{
	W25QFLASH_CS_SELECT;
	W25qxx_Spi(W25_WRITE_ENABLE);
	W25QFLASH_CS_UNSELECT;
	W25qxx_Delay(1);
}

static void W25qxx_WriteDisable(void)
{
	W25QFLASH_CS_SELECT;
	W25qxx_Spi(W25_WRITE_DISABLE);
	W25QFLASH_CS_UNSELECT;
	W25qxx_Delay(1);
}

/*uint8_t W25qxx_ReadStatusRegister(uint8_t SelectStatusRegister_1_2_3)
{
	uint8_t	status=0;
	W25QFLASH_CS_SELECT;
	if(SelectStatusRegister_1_2_3 == 1)
	{
		W25qxx_Spi(W25_READ_STATUS_1);
		status = W25qxx_Spi(W25QXX_DUMMY_BYTE);
		w25qxx.StatusRegister1 = status;
	}
	else if(SelectStatusRegister_1_2_3 == 2)
	{
		W25qxx_Spi(W25_READ_STATUS_2);
		status = W25qxx_Spi(W25QXX_DUMMY_BYTE);
		w25qxx.StatusRegister2 = status;
	}
	else
	{
		W25qxx_Spi(W25_READ_STATUS_3);
		status = W25qxx_Spi(W25QXX_DUMMY_BYTE);
		w25qxx.StatusRegister3 = status;
	}
	W25QFLASH_CS_UNSELECT;
	return status;
}

void W25qxx_WriteStatusRegister(uint8_t	SelectStatusRegister_1_2_3, uint8_t Data)
{
	W25QFLASH_CS_SELECT;
	if(SelectStatusRegister_1_2_3 == 1)
	{
		W25qxx_Spi(W25_WRITE_STATUS_1);
		w25qxx.StatusRegister1 = Data;
	}
	else if(SelectStatusRegister_1_2_3 == 2)
	{
		W25qxx_Spi(W25_WRITE_STATUS_2);
		w25qxx.StatusRegister2 = Data;
	}
	else
	{
		W25qxx_Spi(W25_WRITE_STATUS_3);
		w25qxx.StatusRegister3 = Data;
	}
	W25qxx_Spi(Data);
	W25QFLASH_CS_UNSELECT;
}

uint8_t W25qxx_ReadStatusRegister(uint8_t SelectStatusRegister_1_2_3)
{
	uint8_t	status=0;
	W25QFLASH_CS_SELECT;
	if(SelectStatusRegister_1_2_3 == 1)
	{
		W25qxx_Spi(W25_READ_STATUS_1);
		status = W25qxx_Spi(W25QXX_DUMMY_BYTE);
		w25qxx.StatusRegister1 = status;
	}
	else if(SelectStatusRegister_1_2_3 == 2)
	{
		W25qxx_Spi(W25_READ_STATUS_2);
		status = W25qxx_Spi(W25QXX_DUMMY_BYTE);
		w25qxx.StatusRegister2 = status;
	}
	else
	{
		W25qxx_Spi(W25_READ_STATUS_3);
		status = W25qxx_Spi(W25QXX_DUMMY_BYTE);
		w25qxx.StatusRegister3 = status;
	}
	W25QFLASH_CS_UNSELECT;
	return status;
}

void W25qxx_WriteStatusRegister(uint8_t	SelectStatusRegister_1_2_3, uint8_t Data)
{
	W25QFLASH_CS_SELECT;
	if(SelectStatusRegister_1_2_3 == 1)
	{
		W25qxx_Spi(W25_WRITE_STATUS_1);
		w25qxx.StatusRegister1 = Data;
	}
	else if(SelectStatusRegister_1_2_3 == 2)
	{
		W25qxx_Spi(W25_WRITE_STATUS_2);
		w25qxx.StatusRegister2 = Data;
	}
	else
	{
		W25qxx_Spi(W25_WRITE_STATUS_3);
		w25qxx.StatusRegister3 = Data;
	}
	W25qxx_Spi(Data);
	W25QFLASH_CS_UNSELECT;
}
*/

static void W25qxx_WaitForWriteEnd(void)
{
	uint8_t StatusRegister;

	W25qxx_Delay(1);
	W25QFLASH_CS_SELECT;
	W25qxx_Spi(W25_READ_STATUS_1);

	do{
		StatusRegister = W25qxx_Spi(W25QXX_DUMMY_BYTE);
		W25qxx_Delay(5);
	}
	while((StatusRegister & 0x01) == 0x01);

	W25QFLASH_CS_UNSELECT;
}


uint8_t W25qxx_Init(void)
{
	w25qxx.Lock = 1;
	W25qxx_Delay(1);

	W25QFLASH_CS_UNSELECT;
	W25qxx_Delay(100);

	uint32_t id;

	id = W25qxx_ReadID();
	if ((id & 0x0000FFFF) != 0x4018) {
		printf("Wrong external flash ID: 0x%08x\r\n", id);
		w25qxx.Lock = 0;
		return 0;
	}

	w25qxx.ID = W25Q128;
	w25qxx.Lock = 0;
	printf("external flash ID: 0x%08x\r\n", id);
	return 1;
}

/*
void W25qxx_EraseChip(void)
{
	while(w25qxx.Lock == 1)
		board_DelayUs(1000);

	w25qxx.Lock = 1;

	W25qxx_WriteEnable();

	W25QFLASH_CS_SELECT;
	W25qxx_Spi(W25_CHIP_ERASE);
	W25QFLASH_CS_UNSELECT;

	W25qxx_WaitForWriteEnd();

	board_DelayUs(10000);

	w25qxx.Lock = 0;
} */

void W25qxx_EraseSector(uint32_t SectorAddr)
{
	while(w25qxx.Lock == 1)
		W25qxx_Delay(1);

	w25qxx.Lock = 1;

	W25qxx_WaitForWriteEnd();
	SectorAddr = SectorAddr * W25QFLASH_SECTOR_SIZE;

	W25qxx_WriteEnable();

	W25QFLASH_CS_SELECT;

	W25qxx_Spi(W25_SECTOR_ERASE);

	W25qxx_Spi((SectorAddr & 0xFF0000) >> 16);
	W25qxx_Spi((SectorAddr & 0xFF00) >> 8);
	W25qxx_Spi(SectorAddr & 0xFF);

	W25QFLASH_CS_UNSELECT;

	W25qxx_WriteDisable();
	W25qxx_WaitForWriteEnd();

	W25qxx_Delay(1);
	w25qxx.Lock = 0;
}

void W25qxx_EraseBlock(uint32_t BlockAddr)
{
	while(w25qxx.Lock == 1)
		W25qxx_Delay(1);

	w25qxx.Lock = 1;

	W25qxx_WaitForWriteEnd();

	BlockAddr = BlockAddr * W25QFLASH_BLOCK_SIZE;

	W25qxx_WriteEnable();

	W25QFLASH_CS_SELECT;

	W25qxx_Spi(W25_BLOCK_ERASE);

	W25qxx_Spi((BlockAddr & 0xFF0000) >> 16);
	W25qxx_Spi((BlockAddr & 0xFF00) >> 8);
	W25qxx_Spi(BlockAddr & 0xFF);

	W25QFLASH_CS_UNSELECT;

	W25qxx_WriteDisable();

	W25qxx_WaitForWriteEnd();

	W25qxx_Delay(1);
	w25qxx.Lock = 0;
}

/*
uint8_t W25qxx_IsEmptyPage(uint32_t Page_Address, uint32_t OffsetInByte)
{
	while(w25qxx.Lock == 1)
		W25qxx_Delay(1);

	w25qxx.Lock = 1;

	uint8_t	pBuffer[256] = {0,};
	uint32_t WorkAddress = 0;
	uint16_t size = 0;

	size = W25QFLASH_PAGE_SIZE - OffsetInByte;
	WorkAddress = (OffsetInByte + Page_Address * W25QFLASH_PAGE_SIZE);

	W25QFLASH_CS_SELECT;

	W25qxx_Spi(W25_FAST_READ);
	W25qxx_Spi((WorkAddress & 0xFF0000) >> 16);
	W25qxx_Spi((WorkAddress & 0xFF00) >> 8);
	W25qxx_Spi(WorkAddress & 0xFF);

	W25qxx_Spi(0);

	HAL_SPI_Receive(W25QXX_SPI_PTR, pBuffer, size, 100);

	W25QFLASH_CS_UNSELECT;

	for(uint16_t i = 0; i < size; i++)
	{
		if(pBuffer[i] != 0xFF)
		{
			w25qxx.Lock = 0;
			return 0;
		}
	}

	w25qxx.Lock = 0;
	return 1;
}
*/

uint8_t W25qxx_IsEmptySector(uint32_t Sector_Address, uint32_t OffsetInByte)
{
	while(w25qxx.Lock == 1)
		W25qxx_Delay(1);

	w25qxx.Lock = 1;

	uint8_t	pBuffer[256] = {0,};
	uint32_t WorkAddress = 0;
	uint16_t s_buf = 256;
	uint16_t size = 0;

	size = W25QFLASH_SECTOR_SIZE - OffsetInByte;
	WorkAddress = (OffsetInByte + W25QFLASH_SECTOR_SIZE);

	uint16_t cikl = size / 256;
	uint16_t cikl2 = size % 256;
	uint16_t count_cikle = 0;

	if(size <= 256)
	{
		count_cikle = 1;
		//HAL_UART_Transmit(&huart1, (uint8_t*)"1\n", 2, 100);
	}
	else if(cikl2 == 0)
	{
		count_cikle = cikl;
		//HAL_UART_Transmit(&huart1, (uint8_t*)"2\n", 2, 100);
	}
	else
	{
		count_cikle = cikl + 1;
		//HAL_UART_Transmit(&huart1, (uint8_t*)"3\n", 2, 100);
	}


	for(uint16_t i = 0; i < count_cikle; i++)
	{
		W25QFLASH_CS_SELECT;
		W25qxx_Spi(W25_FAST_READ);
		W25qxx_Spi((WorkAddress & 0xFF0000) >> 16);
		W25qxx_Spi((WorkAddress & 0xFF00) >> 8);
		W25qxx_Spi(WorkAddress & 0xFF);

		W25qxx_Spi(0);

		if(size < 256) s_buf = size;

		HAL_SPI_Receive(W25QXX_SPI_PTR, pBuffer, s_buf, 100);

		W25QFLASH_CS_UNSELECT;

		for(uint16_t i = 0; i < s_buf; i++)
		{
			if(pBuffer[i] != 0xFF)
			{
				w25qxx.Lock = 0;
				return 0;
			}
		}

		size = size - 256;
		WorkAddress = WorkAddress + 256;
	}

	w25qxx.Lock = 0;
	return 1;
}

uint8_t W25qxx_IsEmptyBlock(uint32_t Block_Address, uint32_t OffsetInByte)
{
	while(w25qxx.Lock == 1)
		W25qxx_Delay(1);

	w25qxx.Lock = 1;

	uint8_t	pBuffer[256] = {0,};
	uint32_t WorkAddress = 0;
	uint16_t s_buf = 256;
	uint32_t size = 0;

	size = W25QFLASH_BLOCK_SIZE - OffsetInByte;
	WorkAddress = (OffsetInByte + Block_Address * W25QFLASH_BLOCK_SIZE);


	uint16_t cikl = size / 256;
	uint16_t cikl2 = size % 256;
	uint16_t count_cikle = 0;

	if(size <= 256)
	{
		count_cikle = 1;
		//HAL_UART_Transmit(&huart1, (uint8_t*)"1\n", 2, 100);
	}
	else if(cikl2 == 0)
	{
		count_cikle = cikl;
		//HAL_UART_Transmit(&huart1, (uint8_t*)"2\n", 2, 100);
	}
	else
	{
		count_cikle = cikl + 1;
		//HAL_UART_Transmit(&huart1, (uint8_t*)"3\n", 2, 100);
	}


	for(uint16_t i = 0; i < count_cikle; i++)
	{
		W25QFLASH_CS_SELECT;
		W25qxx_Spi(W25_FAST_READ);
		W25qxx_Spi((WorkAddress & 0xFF0000) >> 16);
		W25qxx_Spi((WorkAddress & 0xFF00) >> 8);
		W25qxx_Spi(WorkAddress & 0xFF);

		W25qxx_Spi(0);

		if(size < 256) s_buf = size;

		HAL_SPI_Receive(W25QXX_SPI_PTR, pBuffer, s_buf, 100);

		W25QFLASH_CS_UNSELECT;

		for(uint16_t i = 0; i < s_buf; i++)
		{
			if(pBuffer[i] != 0xFF)
			{
				w25qxx.Lock = 0;
				return 0;
			}
		}

		size = size - 256;
		WorkAddress = WorkAddress + 256;
	}

	w25qxx.Lock = 0;
	return 1;
}


void W25qxx_WriteByte(uint8_t byte, uint32_t addr)
{
	while(w25qxx.Lock == 1)
		W25qxx_Delay(1);

	w25qxx.Lock = 1;

	W25qxx_WaitForWriteEnd();
	W25qxx_WriteEnable();

	W25QFLASH_CS_SELECT;

	W25qxx_Spi(W25_PAGE_PROGRAMM);
	W25qxx_Spi((addr & 0xFF0000) >> 16);
	W25qxx_Spi((addr & 0xFF00) >> 8);
	W25qxx_Spi(addr & 0xFF);

	W25qxx_Spi(byte);

	W25QFLASH_CS_UNSELECT;

	W25qxx_WriteDisable();

	W25qxx_WaitForWriteEnd();

	w25qxx.Lock = 0;
}

void W25qxx_WritePage(uint8_t *pBuffer, uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_PageSize)
{
	while(w25qxx.Lock == 1)
		W25qxx_Delay(1);

	w25qxx.Lock = 1;

	if(((NumByteToWrite_up_to_PageSize + OffsetInByte) > W25QFLASH_PAGE_SIZE) || (NumByteToWrite_up_to_PageSize == 0))
		NumByteToWrite_up_to_PageSize = W25QFLASH_PAGE_SIZE - OffsetInByte;

	if((OffsetInByte + NumByteToWrite_up_to_PageSize) > W25QFLASH_PAGE_SIZE)
		NumByteToWrite_up_to_PageSize = W25QFLASH_PAGE_SIZE - OffsetInByte;


	W25qxx_WaitForWriteEnd();

	W25qxx_WriteEnable();

	W25QFLASH_CS_SELECT;

	W25qxx_Spi(W25_PAGE_PROGRAMM);

	Page_Address = (Page_Address * W25QFLASH_PAGE_SIZE) + OffsetInByte;
	W25qxx_Spi((Page_Address & 0xFF0000) >> 16);
	W25qxx_Spi((Page_Address & 0xFF00) >> 8);
	W25qxx_Spi(Page_Address & 0xFF);

	HAL_SPI_Transmit(W25QXX_SPI_PTR, pBuffer, NumByteToWrite_up_to_PageSize, 100);

	W25QFLASH_CS_UNSELECT;

	W25qxx_WriteDisable();

	W25qxx_WaitForWriteEnd();

	W25qxx_Delay(1);
	w25qxx.Lock = 0;
}

/*
void W25qxx_WriteSector(uint8_t *pBuffer, uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_SectorSize)
{
	if((NumByteToWrite_up_to_SectorSize > W25QFLASH_SECTOR_SIZE) || (NumByteToWrite_up_to_SectorSize == 0))
		NumByteToWrite_up_to_SectorSize = W25QFLASH_SECTOR_SIZE;

	uint32_t StartPage;
	int32_t	BytesToWrite;
	uint32_t LocalOffset;

	if((OffsetInByte + NumByteToWrite_up_to_SectorSize) > W25QFLASH_SECTOR_SIZE)
		BytesToWrite = W25QFLASH_SECTOR_SIZE - OffsetInByte;
	else
		BytesToWrite = NumByteToWrite_up_to_SectorSize;

	StartPage = Sector_To_Page(Sector_Address) + (OffsetInByte / W25QFLASH_PAGE_SIZE);
	LocalOffset = OffsetInByte % W25QFLASH_PAGE_SIZE;

	do
	{
		W25qxx_WritePage(pBuffer, StartPage, LocalOffset, BytesToWrite);
		StartPage++;

		BytesToWrite -= W25QFLASH_PAGE_SIZE - LocalOffset;
		//pBuffer += W25QFLASH_PAGE_SIZE;
		pBuffer += W25QFLASH_PAGE_SIZE - LocalOffset;
		LocalOffset = 0;
	}
	while(BytesToWrite > 0);
}


void W25qxx_WriteBlock(uint8_t* pBuffer, uint32_t Block_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_BlockSize)
{
	if((NumByteToWrite_up_to_BlockSize>W25QFLASH_BLOCK_SIZE)||(NumByteToWrite_up_to_BlockSize == 0))
		NumByteToWrite_up_to_BlockSize=W25QFLASH_BLOCK_SIZE;

	uint32_t	StartPage;
	int32_t		BytesToWrite;
	uint32_t	LocalOffset;

	if((OffsetInByte+NumByteToWrite_up_to_BlockSize) > W25QFLASH_BLOCK_SIZE)
		BytesToWrite = W25QFLASH_BLOCK_SIZE - OffsetInByte;
	else
		BytesToWrite = NumByteToWrite_up_to_BlockSize;

	StartPage = W25qxx_BlockToPage(Block_Address)+(OffsetInByte/W25QFLASH_PAGE_SIZE);

	LocalOffset = OffsetInByte%W25QFLASH_PAGE_SIZE;

	do
	{
		W25qxx_WritePage(pBuffer,StartPage,LocalOffset,BytesToWrite);
		StartPage++;
		BytesToWrite -= W25QFLASH_PAGE_SIZE - LocalOffset;
		//pBuffer += W25QFLASH_PAGE_SIZE;
		pBuffer += W25QFLASH_PAGE_SIZE - LocalOffset;
		LocalOffset = 0;
	}
	while(BytesToWrite > 0);
}
*/

void W25qxx_ReadByte(uint8_t *pBuffer, uint32_t Bytes_Address)
{
	while(w25qxx.Lock==1)
		W25qxx_Delay(1);

	w25qxx.Lock=1;

	W25QFLASH_CS_SELECT;
	W25qxx_Spi(W25_FAST_READ);
	W25qxx_Spi((Bytes_Address & 0xFF0000) >> 16);
	W25qxx_Spi((Bytes_Address& 0xFF00) >> 8);
	W25qxx_Spi(Bytes_Address & 0xFF);
	W25qxx_Spi(0);

	*pBuffer = W25qxx_Spi(W25QXX_DUMMY_BYTE);

	W25QFLASH_CS_UNSELECT;

	w25qxx.Lock = 0;
}

void W25qxx_ReadBytes(uint8_t* pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead)
{
	while(w25qxx.Lock == 1)
		W25qxx_Delay(1);

	w25qxx.Lock = 1;

	W25QFLASH_CS_SELECT;

	W25qxx_Spi(W25_FAST_READ);
	W25qxx_Spi((ReadAddr & 0xFF0000) >> 16);
	W25qxx_Spi((ReadAddr& 0xFF00) >> 8);
	W25qxx_Spi(ReadAddr & 0xFF);
	W25qxx_Spi(0);

	HAL_SPI_Receive(W25QXX_SPI_PTR, pBuffer, NumByteToRead, 2000);
	W25QFLASH_CS_UNSELECT;

	W25qxx_Delay(1);
	w25qxx.Lock = 0;
}


void W25qxx_ReadPage(uint8_t *pBuffer, uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_PageSize)
{
	while(w25qxx.Lock==1)
		W25qxx_Delay(1);

	w25qxx.Lock = 1;

	if((NumByteToRead_up_to_PageSize>W25QFLASH_PAGE_SIZE) || (NumByteToRead_up_to_PageSize==0))
		NumByteToRead_up_to_PageSize=W25QFLASH_PAGE_SIZE;

	if((OffsetInByte+NumByteToRead_up_to_PageSize) > W25QFLASH_PAGE_SIZE)
		NumByteToRead_up_to_PageSize = W25QFLASH_PAGE_SIZE - OffsetInByte;

	Page_Address = Page_Address * W25QFLASH_PAGE_SIZE + OffsetInByte;
	W25QFLASH_CS_SELECT;

	W25qxx_Spi(W25_FAST_READ);
	W25qxx_Spi((Page_Address & 0xFF0000) >> 16);
	W25qxx_Spi((Page_Address& 0xFF00) >> 8);
	W25qxx_Spi(Page_Address & 0xFF);

	W25qxx_Spi(0);

	HAL_SPI_Receive(W25QXX_SPI_PTR, pBuffer, NumByteToRead_up_to_PageSize, 100);

	W25QFLASH_CS_UNSELECT;

	W25qxx_Delay(1);
	w25qxx.Lock=0;
}

/*
void W25qxx_ReadSector(uint8_t *pBuffer,uint32_t Sector_Address,uint32_t OffsetInByte,uint32_t NumByteToRead_up_to_SectorSize)
{
	if((NumByteToRead_up_to_SectorSize>W25QFLASH_SECTOR_SIZE) || (NumByteToRead_up_to_SectorSize==0))
		NumByteToRead_up_to_SectorSize=W25QFLASH_SECTOR_SIZE;

	uint32_t StartPage;
	int32_t	BytesToRead;
	uint32_t LocalOffset;

	if((OffsetInByte + NumByteToRead_up_to_SectorSize) > W25QFLASH_SECTOR_SIZE)
		BytesToRead = W25QFLASH_SECTOR_SIZE - OffsetInByte;
	else
		BytesToRead = NumByteToRead_up_to_SectorSize;

	StartPage = Sector_To_Page(Sector_Address) + (OffsetInByte / W25QFLASH_PAGE_SIZE);

	LocalOffset = OffsetInByte % W25QFLASH_PAGE_SIZE;

	do
	{
		W25qxx_ReadPage(pBuffer, StartPage, LocalOffset, BytesToRead);
		StartPage++;
		BytesToRead -= W25QFLASH_PAGE_SIZE-LocalOffset;
		//pBuffer+=W25QFLASH_PAGE_SIZE;
		pBuffer += W25QFLASH_PAGE_SIZE - LocalOffset;
		LocalOffset = 0;
	}
	while(BytesToRead > 0);
}



void W25qxx_ReadBlock(uint8_t *pBuffer, uint32_t Block_Address, uint32_t OffsetInByte, uint32_t	NumByteToRead_up_to_BlockSize)
{
	if((NumByteToRead_up_to_BlockSize > W25QFLASH_BLOCK_SIZE) || (NumByteToRead_up_to_BlockSize == 0))
		NumByteToRead_up_to_BlockSize = W25QFLASH_BLOCK_SIZE;

	uint32_t StartPage;
	int32_t	BytesToRead;
	uint32_t LocalOffset;

	if((OffsetInByte+NumByteToRead_up_to_BlockSize) > W25QFLASH_BLOCK_SIZE)
		BytesToRead = W25QFLASH_BLOCK_SIZE-OffsetInByte;
	else
		BytesToRead = NumByteToRead_up_to_BlockSize;

	StartPage = W25qxx_BlockToPage(Block_Address) + (OffsetInByte / W25QFLASH_PAGE_SIZE);

	LocalOffset = OffsetInByte%W25QFLASH_PAGE_SIZE;

	do
	{
		W25qxx_ReadPage(pBuffer,StartPage,LocalOffset,BytesToRead);
		StartPage++;
		BytesToRead-=W25QFLASH_PAGE_SIZE-LocalOffset;
		//pBuffer+=W25QFLASH_PAGE_SIZE;
		pBuffer += W25QFLASH_PAGE_SIZE - LocalOffset;
		LocalOffset=0;
	}
	while(BytesToRead > 0);
}
*/


#if 0
void W25qxx_test(void)
{
    uint8_t b0 = 's';
    uint8_t b1 = 't';
    uint8_t b2 = 'D';

    if(!W25qxx_Init()) {
    	Debug_Printf("ERROR:  W25Q128 initialize FAILED \r\n");
    	return;
    }

    Debug_Printf("W25Q128 initialize done \r\n");
    W25qxx_EraseBlock(0);
    Debug_Printf("W25Q128 erase block done \r\n");

    W25qxx_WriteByte(b0, 25);
    W25qxx_WriteByte(b1, 26);
    W25qxx_WriteByte(b2, 27);
    uint8_t buf[64] = {0,};

    W25qxx_ReadByte(&buf[0], 25);
    W25qxx_ReadByte(&buf[1], 26);
    W25qxx_ReadByte(&buf[2], 27);

    buf[3] = 0;
    Debug_Printf(buf);       // Should be stD
    Debug_Printf("\r\n");



    uint8_t w_buf[] = "istarik.ru";
    W25qxx_WritePage(w_buf, 0, 28, 10);
    memset(buf, 0, 64);
    W25qxx_ReadPage(buf, 0, 28, 10);
    Debug_Printf(buf);       // Should be istarik.ru
    Debug_Printf("\r\n");

	W25qxx_WriteSector(w_buf, 0, 1350, 10);
    memset(buf, 0, 64);
    W25qxx_ReadSector(buf, 0, 1350, 10);
    Debug_Printf(buf);      // Should be istarik.ru
    Debug_Printf("\r\n");

    memset(buf, 0, 64);
    W25qxx_ReadBytes(buf, 1350, 10);
    Debug_Printf(buf);     // Should be istarik.ru
    Debug_Printf("\r\n");

    Debug_Printf("W25Q128 test done \r\n");
}
#endif
