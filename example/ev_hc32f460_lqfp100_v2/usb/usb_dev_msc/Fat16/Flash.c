#include "Flash.h"

void vFlashErase(uint32_t addr)
{
    if (addr >= APP_START_ADDRESS &&
        addr < FLASH_SIZE)
    {
        if ((addr % FLASH_SECTOR_SIZE) == 0)
        {
            /* Unlock EFM. */
            EFM_Unlock();
            /* Enable flash. */
            EFM_FlashCmd(Enable);
            /* Wait flash ready. */
            EFM_SectorErase(addr);
            EFM_Lock();
        }
    }
}

void vFlashProgram(uint32_t addr, uint8_t *buf, uint16_t len)
{
    if (addr >= APP_START_ADDRESS &&
        addr < FLASH_SIZE)
    {
        /* Unlock EFM. */
        EFM_Unlock();
        /* Enable flash. */
        EFM_FlashCmd(Enable);
        /* Wait flash ready. */

        for (uint16_t i = 0; i < len; i += 4)
        {
            EFM_SingleProgram(addr + i, *((uint32_t *)(buf + i)));
        }

        EFM_Lock();
    }
}


