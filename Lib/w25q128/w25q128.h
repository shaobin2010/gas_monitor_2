
#ifndef __W25Q128_FLASH_H__
#define __W25Q128_FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "spi.h"
#include "gpio.h"
#include <stdint.h>

/*
 *  256B per Page;                             Count:  256*16*4096/256 = 16*4096 = 65536(0x10000)
 *  4096 (0x1000) Bytes per Sector;            Count:  256*16
 *  4096*16 = 65536(0x10000) Bytes per Block;  Count:  256
 *
 */
extern SPI_HandleTypeDef hspi1;
#define W25QXX_SPI_PTR   &hspi1

#if 0
#define W25QFLASH_CS_SELECT      LL_GPIO_ResetOutputPin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin)
#define W25QFLASH_CS_UNSELECT    LL_GPIO_SetOutputPin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin)
#else
#define W25QFLASH_CS_SELECT      HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, GPIO_PIN_RESET)
#define W25QFLASH_CS_UNSELECT    HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, GPIO_PIN_SET)
#endif

#define W25QFLASH_PAGE_SIZE        256      // 0x100
#define W25QFLASH_PAGE_COUNT       65536    // 0x10000
#define W25QFLASH_SECTOR_SIZE      4096     // 0x1000
#define W25QFLASH_SECTOR_COUNT     4096     // 0x1000
#define W25QFLASH_BLOCK_SIZE       65536    // 0x10000
#define W25QFLASH_BLOCK_COUNT      256      // 0x100

#define Page_To_Sector(page_addr)  		((page_addr * W25QFLASH_PAGE_SIZE)/W25QFLASH_SECTOR_SIZE)
#define Page_To_Block(page_addr)   		((page_addr * W25QFLASH_PAGE_SIZE)/W25QFLASH_BLOCK_SIZE)
#define Sector_To_Block(sector_addr)    ((sector_addr * W25QFLASH_SECTOR_SIZE)/W25QFLASH_BLOCK_SIZE)
#define Sector_To_Page(sector_addr)     ((sector_addr * W25QFLASH_SECTOR_SIZE)/W25QFLASH_PAGE_SIZE)
#define Block_To_Page(block_addr)       ((block_addr * W25QFLASH_BLOCK_SIZE)/W25QFLASH_PAGE_SIZE)

#define W25_WRITE_DISABLE     0x04
#define W25_WRITE_ENABLE      0x06
#define W25_CHIP_ERASE        0xC7 //0x60
#define W25_SECTOR_ERASE      0x20
#define W25_BLOCK_ERASE       0xD8
#define W25_FAST_READ         0x0B
#define W25_PAGE_PROGRAMM     0x02
#define W25_GET_JEDEC_ID      0x9F
#define W25_READ_STATUS_1     0x05
#define W25_READ_STATUS_2     0x35
#define W25_READ_STATUS_3     0x15
#define W25_WRITE_STATUS_1    0x01
#define W25_WRITE_STATUS_2    0x31
#define W25_WRITE_STATUS_3    0x11
#define W25_READ_UNIQUE_ID    0x4B

typedef enum
{
	W25Q10 = 1,
	W25Q20,
	W25Q40,
	W25Q80,
	W25Q16,
	W25Q32,
	W25Q64,
	W25Q128,
	W25Q256,
	W25Q512,

} W25QXX_ID_t;

typedef struct
{
	W25QXX_ID_t	ID;
//	uint8_t		UniqID[8];
//	uint16_t	PageSize;
//	uint32_t	PageCount;
//	uint32_t	SectorSize;
//	uint32_t	SectorCount;
//	uint32_t	BlockSize;
//	uint32_t	BlockCount;
//	uint32_t	CapacityInKiloByte;
//	uint8_t		StatusRegister1;
//	uint8_t		StatusRegister2;
//	uint8_t		StatusRegister3;
	uint8_t		Lock;

}w25qxx_t;

extern w25qxx_t	w25qxx;

extern uint8_t W25qxx_Init(void);
extern void W25qxx_EraseSector(uint32_t SectorAddr);
extern void W25qxx_EraseBlock(uint32_t BlockAddr);
extern void W25qxx_WriteByte(uint8_t byte, uint32_t addr);
extern void W25qxx_WritePage(uint8_t *pBuffer, uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_PageSize);
extern void W25qxx_ReadByte(uint8_t *pBuffer, uint32_t Bytes_Address);
extern void W25qxx_ReadBytes(uint8_t* pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead);
extern void W25qxx_ReadPage(uint8_t *pBuffer, uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_PageSize);

#ifdef __cplusplus
}
#endif

#endif /* __W25Q128_FLASH_H__ */
