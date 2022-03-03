#define UF2_VERSION "2.8.2"
#ifndef BOARD_ID
#define BOARD_ID "STM32F4xx-Arcade-Generic"
#endif
#define INDEX_URL "https://arcade.makecode.com"
#define UF2_NUM_BLOCKS 8000
#ifndef VOLUME_LABEL
#define VOLUME_LABEL "ARCADE-F4"
#endif
// where the UF2 files are allowed to write data; we let them write to settings area as well
#define FLASH_START_ADD (0x00000000)
#define USER_FLASH_START (uint32_t)(FLASH_START_ADD + 0x00010000)
#define USER_FLASH_END (uint32_t)(FLASH_START_ADD + BOARD_FLASH_SIZE)

#define VALID_FLASH_ADDR(addr, sz) (USER_FLASH_START <= (addr) && (addr) + (sz) <= USER_FLASH_END)

