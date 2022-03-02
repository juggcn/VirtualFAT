#ifndef __VIRTUALFLASH_H__
#define __VIRTUALFLASH_H__

#include "hc32_ddl.h"

#include <stdint.h>
#include <stdbool.h>

#define FLASH_SECTOR_SIZE (8 * 1024)

#define APP_START_ADDRESS (0x10000)

#define FLASH_SIZE (512 * 1024)

extern void VirtualFlashErase(uint32_t addr);
extern void VirtualFlashProgram(uint32_t addr, uint8_t *buf, uint16_t len);


#endif