#include <stdio.h>
#include "usart.h"

#include "crc.h"
#include "w25q128.h"
#include "app_ota.h"



#if 0

#include "ota_test_app_hdr.h"

static ota_img_header_s ota_img_header;
uint32_t ota_crc32;
uint8_t buff[256];

void app_ota_get_header(void)
{
	memcpy(&ota_img_header, (void *)OTA_TEST_APP, sizeof(ota_img_header_s));
	printf("OTA IMAGE HEADER: magic: 0x%08x, size: 0x%08x, crc32: 0x%08x\r\n",
			ota_img_header.magic,
			ota_img_header.img_size,
			ota_img_header.crc32);


	// Erase first block of the external flash
	W25qxx_Init();
	W25qxx_EraseBlock(0);
}

void app_ota_write_raw_img(void)
{
	uint32_t crc32;
	uint32_t read_len, img_offset, ota_page_idx;
	hw_crc32_init();

	// jump header
	img_offset = sizeof(ota_img_header_s);
	ota_page_idx = OTA_IMG_FIRST_PAGE_IDX;
	printf("Copying rx data into external flash\r\n");

	while(img_offset < OTA_TEST_APP_LEN) {
		if(ota_page_idx == OTA_IMG_FIRST_PAGE_IDX) {
			// first pkg just write 256 - 16 bytes
			read_len = W25QFLASH_PAGE_SIZE - sizeof(ota_img_header_s);
			W25qxx_WritePage(&OTA_TEST_APP[img_offset], ota_page_idx, RAW_IMG_FIRST_OFFSET, read_len);
		} else {
			read_len = ((OTA_TEST_APP_LEN - img_offset) > W25QFLASH_PAGE_SIZE) ? W25QFLASH_PAGE_SIZE : OTA_TEST_APP_LEN - img_offset;
			W25qxx_WritePage(&OTA_TEST_APP[img_offset], ota_page_idx, 0, read_len);
		}

		crc32 = hw_crc32_feeddata((uint32_t *)&OTA_TEST_APP[img_offset], read_len/4);
		img_offset += read_len;
		ota_page_idx++;
		Debug_USART_SendByte('.');
	}

	printf("\r\nDone\r\nReceived CRC32 : 0x%08x\r\n", crc32);

	// check raw image crc32 in the external flash
	hw_crc32_init();
	img_offset = sizeof(ota_img_header_s);
	ota_page_idx = OTA_IMG_FIRST_PAGE_IDX;

	while(img_offset < OTA_TEST_APP_LEN) {
		if(ota_page_idx == OTA_IMG_FIRST_PAGE_IDX) {
			// first pkg just write 256 - 16 bytes
			read_len = W25QFLASH_PAGE_SIZE - sizeof(ota_img_header_s);
			W25qxx_ReadPage(buff, ota_page_idx, RAW_IMG_FIRST_OFFSET, read_len);
		} else {
			read_len = ((OTA_TEST_APP_LEN - img_offset) > W25QFLASH_PAGE_SIZE) ? W25QFLASH_PAGE_SIZE : OTA_TEST_APP_LEN - img_offset;
			W25qxx_ReadPage(buff, ota_page_idx, 0, read_len);
		}

		crc32 = hw_crc32_feeddata((uint32_t *)buff, read_len/4);
		img_offset += read_len;
		ota_page_idx++;
//		printf("img_offset : %d\r\n", img_offset);
	}

	printf("External flash CRC32 : 0x%08x\r\n", crc32);

	if(crc32 == ota_img_header.crc32) {
		// write header
		W25qxx_WritePage(&OTA_TEST_APP[0], 0, 0, sizeof(ota_img_header_s));
		printf("App OTA Done\r\n");
	} else {
		printf("CRC32 ERROR\r\n");
	}
}

void app_ota_test(void)
{
	app_ota_get_header();
	app_ota_write_raw_img();
}

#endif
