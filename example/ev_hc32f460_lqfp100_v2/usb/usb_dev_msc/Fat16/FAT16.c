/******************************************************************************
 *
 *  (c) copyright Freescale Semiconductor 2008
 *  ALL RIGHTS RESERVED
 *
 *  File Name: FAT16.c
 *
 *  Purpose: This file is for a USB Mass-Storage Device bootloader.  This file
 *           mimics a FAT16 drive in order to enumerate as a disk drive
 *
 *  Assembler:  Codewarrior for Microcontrollers V6.2
 *
 *  Version:  1.3
 *
 *
 *  Author: Derek Snell
 *
 *  Location: Indianapolis, IN. USA
 *
 * UPDATED HISTORY:
 *
 * REV   YYYY.MM.DD  AUTHOR        DESCRIPTION OF CHANGE
 * ---   ----------  ------        ---------------------
 * 1.3   2009.01.13  Derek Snell   Added linker SEGMENTs for S08 version
 * 1.2   2008.11.24  Derek Snell   Added Volume label "BOOTLOADER" to FAT16 root directory
 * 1.1   2008.09.17  Derek Snell   Updated to give S19 address error in status
 * 1.0   2008.06.10  Derek Snell   Initial version
 *
 *
 ******************************************************************************/
/* Freescale  is  not  obligated  to  provide  any  support, upgrades or new */
/* releases  of  the Software. Freescale may make changes to the Software at */
/* any time, without any obligation to notify or provide updated versions of */
/* the  Software  to you. Freescale expressly disclaims any warranty for the */
/* Software.  The  Software is provided as is, without warranty of any kind, */
/* either  express  or  implied,  including, without limitation, the implied */
/* warranties  of  merchantability,  fitness  for  a  particular purpose, or */
/* non-infringement.  You  assume  the entire risk arising out of the use or */
/* performance of the Software, or any systems you design using the software */
/* (if  any).  Nothing  may  be construed as a warranty or representation by */
/* Freescale  that  the  Software  or  any derivative work developed with or */
/* incorporating  the  Software  will  be  free  from  infringement  of  the */
/* intellectual property rights of third parties. In no event will Freescale */
/* be  liable,  whether in contract, tort, or otherwise, for any incidental, */
/* special,  indirect, consequential or punitive damages, including, but not */
/* limited  to,  damages  for  any loss of use, loss of time, inconvenience, */
/* commercial loss, or lost profits, savings, or revenues to the full extent */
/* such  may be disclaimed by law. The Software is not fault tolerant and is */
/* not  designed,  manufactured  or  intended by Freescale for incorporation */
/* into  products intended for use or resale in on-line control equipment in */
/* hazardous, dangerous to life or potentially life-threatening environments */
/* requiring  fail-safe  performance,  such  as  in the operation of nuclear */
/* facilities,  aircraft  navigation  or  communication systems, air traffic */
/* control,  direct  life  support machines or weapons systems, in which the */
/* failure  of  products  could  lead  directly to death, personal injury or */
/* severe  physical  or  environmental  damage  (High  Risk Activities). You */
/* specifically  represent and warrant that you will not use the Software or */
/* any  derivative  work of the Software for High Risk Activities.           */
/* Freescale  and the Freescale logos are registered trademarks of Freescale */
/* Semiconductor Inc.                                                        */
/*****************************************************************************/

#include "string.h"
#include "stdlib.h"
#include "FAT16.h"
#include "Flash.h"

/********************************************************************
*********************************************************************
*       FAT16 Boot Sector
*********************************************************************
********************************************************************/
static const uint8_t FAT16_BootSector[FATBootSize] =
    {
        0xEB,                                                  /*00 - BS_jmpBoot */
        0x3C,                                                  /*01 - BS_jmpBoot */
        0x90,                                                  /*02 - BS_jmpBoot */
        'M', 'S', 'D', 'O', 'S', '5', '.', '0',                /* 03-10 - BS_OEMName */
        0x00,                                                  /*11 - BPB_BytesPerSec = 512 */
        0x02,                                                  /*11 - BPB_BytesPerSec = 512 */
        32,                                                    /*13 - BPB_Sec_PerClus = 32 */
        2,                                                     /*14 - BPB_RsvdSecCnt = 2 */
        0,                                                     /*15 - BPB_RsvdSecCnt = 2 */
        2,                                                     /*16 - BPB_NumFATs = 2 */
        0x0,                                                   /*17 - BPB_RootEntCnt = 512 */
        0x2,                                                   /*18 - BPB_RootEntCnt = 512 */
        0,                                                     /*19 - BPB_TotSec16 = 0 */
        0,                                                     /*20 - BPB_TotSec16 = 0 */
        0xF8,                                                  /*21 - BPB_Media = 0xF8 */
        243,                                                   /*22 - BPBFATSz16 = 243 */
        0,                                                     /*23 - BPBFATSz16 = 243 */
        63,                                                    /*24 - BPB_SecPerTrk = 63 */
        0,                                                     /*25 - BPB_SecPerTrk = 63 */
        255,                                                   /*26 - BPB_NumHeads = 255 */
        0,                                                     /*27 - BPB_NumHeads = 255 */
        249,                                                   /*28 - BPB_HiddSec = 249 */
        0,                                                     /*29 - BPB_HiddSec = 249 */
        0,                                                     /*30 - BPB_HiddSec = 249 */
        0,                                                     /*31 - BPB_HiddSec = 249 */
        0x07,                                                  /*32 - BPB_TotSec32 = 1,983,495 */
        0x44,                                                  /*33 - BPB_TotSec32 = 1,983,495 */
        0x1E,                                                  /*34 - BPB_TotSec32 = 1,983,495 */
        0x00,                                                  /*35 - BPB_TotSec32 = 1,983,495 */
        0,                                                     /*36 - BS_DrvNum = 1 */
        0, /*37 - BS_Reserved1 = 0 , dirty bit = 0*/           /* Updated from FSL*/
        0x29,                                                  /*38 - BS_BootSig = 0x29 */
        0xBD,                                                  /*39 - BS_VolID = 0x02DDA5BD */
        0xA5,                                                  /*40 - BS_VolID = 0x02DDA5BD */
        0xDD,                                                  /*41 - BS_VolID = 0x02DDA5BD */
        0x02,                                                  /*42 - BS_VolID = 0x02DDA5BD */
        'N', 'O', ' ', 'N', 'A', 'M', 'E', ' ', ' ', ' ', ' ', /*43-53 - BS_VolLab */
        'F', 'A', 'T', '1', '6', ' ', ' ', ' '                 /*54-61 - BS_FilSysType */
};

/********************************************************************
*********************************************************************
*       First Sector of FAT Table
*********************************************************************
********************************************************************/
static const uint8_t FAT16_TableSector0[FATTableSize] =
    {
        0xF8, 0xFF, 0xFF, 0x7F};

/********************************************************************
*********************************************************************
*       FAT Root Directory Sector
*********************************************************************
********************************************************************/

static uint8_t FAT16_ReadyFileName[FATFileNameSize] =
    {
        'R', 'E', 'A', 'D', 'Y', ' ', ' ', ' ', 'T', 'X', 'T' /*00-10 - Short File Name */
};

// TODO
// Change Finish to Success

static const uint8_t FAT16_RootDirSector[FATDirSize] =
    {
        0x20,                                                  /*11 - Archive Attribute set */
        0x00,                                                  /*12 - Reserved */
        0x4B,                                                  /*13 - Create Time Tenth */
        0x9C,                                                  /*14 - Create Time */
        0x42,                                                  /*15 - Create Time */
        0x92,                                                  /*16 - Create Date */
        0x38,                                                  /*17 - Create Date */
        0x92,                                                  /*18 - Last Access Date */
        0x38,                                                  /*19 - Last Access Date */
        0x00,                                                  /*20 - Not used in FAT16 */
        0x00,                                                  /*21 - Not used in FAT16 */
        0x9D,                                                  /*22 - Write Time */
        0x42,                                                  /*23 - Write Time */
        0x92,                                                  /*24 - Write Date */
        0x38,                                                  /*25 - Write Date */
        0x00,                                                  /*26 - First Cluster (none, because file is empty) */
        0x00,                                                  /*27 - First Cluster (none, because file is empty) */
        0x00,                                                  /*28 - File Size */
        0x00,                                                  /*29 - File Size */
        0x00,                                                  /*30 - File Size */
        0x00,                                                  /*31 - File Size */
        'B', 'O', 'O', 'T', 'L', 'O', 'A', 'D', 'E', 'R', ' ', /*32-42 - Volume label */
        0x08,                                                  /*43 - File attribute = Volume label */
        0x00,                                                  /*44 - Reserved */
        0x00,                                                  /*45 - Create Time Tenth */
        0x00,                                                  /*46 - Create Time */
        0x00,                                                  /*47 - Create Time */
        0x00,                                                  /*48 - Create Date */
        0x00,                                                  /*49 - Create Date */
        0x00,                                                  /*50 - Last Access Date */
        0x00,                                                  /*51 - Last Access Date */
        0x00,                                                  /*52 - Not used in FAT16 */
        0x00,                                                  /*53 - Not used in FAT16 */
        0x9D,                                                  /*54 - Write Time */
        0x42,                                                  /*55 - Write Time */
        0x92,                                                  /*56 - Write Date */
        0x38,                                                  /*57 - Write Date */
};

FAT_DIR_t FileAttr;

/*********************************************************
 * Name: FATReadLBA
 *
 * Desc: Read a Logical Block Address
 *
 * Parameter: FAT_LBA - Logical Block Address to Read
 *            pu8DataPointer - Pointer to array to store data read
 *
 * Return: None
 *
 **********************************************************/
uint32_t FATReadLBA(uint32_t FAT_LBA, uint8_t *data, uint32_t len)
{
    /* Body */
    int32_t i;
    switch (FAT_LBA)
    {
    /* Boot Sector */
    case (FATBootSec * FATBytesPerSec):
        /* Write Boot Sector info */
        for (i = 0; i < FATBootSize; i++)
        {
            *data++ = FAT16_BootSector[i];
        } /* EndFor */
        /* Rest of sector empty except last two bytes */
        i += 2;
        while (i++ < FATBytesPerSec)
        {
            *data++ = 0;
        } /* EndWhile */

        /* Boot Sector requires these 2 bytes at end */
        *data++ = 0x55;
        *data++ = 0xAA;

        break;
    /* FAT Table Sector */
    case (FATTable0Sec0 * FATBytesPerSec): // -FAT0
    case (FATTable1Sec0 * FATBytesPerSec): // -FAT1
        /* Write FAT Table Sector */
        for (i = 0; i < FATTableSize; i++)
        {
            *data++ = FAT16_TableSector0[i];
        } /* EndFor */
        /* Rest of sector empty */
        while (i++ < FATBytesPerSec)
        {
            *data++ = 0;
        } /*ENdWhile */
        break;

    /* Root Directory Sector */
    case (FATRootDirSec0 * FATBytesPerSec): //

        for (i = 0; i < FATFileNameSize; i++)
        {
            *data++ = FAT16_ReadyFileName[i];
        } /* EndFor */

        /* Write rest of file FAT structure */
        for (i = 0; i < FATDirSize; i++)
        {
            *data++ = FAT16_RootDirSector[i];
        } /* EndFor */

        /* Rest of sector empty to signify no more files */
        i += FATFileNameSize;
        while (i++ < FATBytesPerSec)
        {
            *data++ = 0;
        } /* EndWhile */
        break;

        /* All other sectors empty */
    default:
        i = 0;
        while (i++ < FATBytesPerSec)
        {
            *data++ = 0;
        } /* EndWhile */
        break;
    } /* EndSwitch */

    return FATBytesPerSec;
} /* EndBody */

static uint32_t FAT_RootDirWriteRequest(uint32_t FAT_LBA, uint8_t *data, uint32_t len)
{
    FAT_DIR_t *pFile = (FAT_DIR_t *)data;
    uint32_t index = 2;

    pFile++; // Skip Root Dir
    pFile++; // Skip Status File

    while ((pFile->DIR_Attr != 0x20) && (index++ < 512))
    {
        pFile++;
    }

    // Find it
    if (index <= 512)
    {
        memcpy(&FileAttr, pFile, 32);
        FileAttr.DIR_WriteTime = 0;
        FileAttr.DIR_WriteDate = 0;
    }
    else
    {
        memset(&FileAttr, (int)0, 32);
    }

    return len;
}

static uint32_t FAT_DataSectorWriteRequest(uint32_t FAT_LBA, uint8_t *data, uint32_t len)
{
    static uint32_t ulBytesWriten = 0;
    if (FAT_LBA == (FATDataSec0 * FATBytesPerSec))
    {
        ulBytesWriten = 0;
    }
    if (!memcmp(&(FileAttr.DIR_Name[8]), "BIN", 3))
    {
        const uint32_t ulFreeFlash = 100 * 1024 * 1024;

        if (ulFreeFlash >= FileAttr.DIR_FileSize)
        {
            // Flash MCU
            vFlashErase(APP_START_ADDRESS + ulBytesWriten);
            vFlashProgram(APP_START_ADDRESS + ulBytesWriten, data, len);

            ulBytesWriten += len;
            if (ulBytesWriten >= FileAttr.DIR_FileSize)
            {
                FATSetStatusFileName("SUCCESS");
            }
        }
        else
        {
            FATSetStatusFileName("LARGE");
        }
    }
    else
    {
        // Can't Recognize it
        // Cancel and Reset USB
        FATSetStatusFileName("UNKOWN");
    }

    return len;
}

uint32_t FATWriteLBA(uint32_t FAT_LBA, uint8_t *data, uint32_t len)
{
    switch (FAT_LBA)
    {
    case (FATBootSec * FATBytesPerSec):    //    Boot Sector
    case (FATTable0Sec0 * FATBytesPerSec): //   FAT0
    case (FATTable1Sec0 * FATBytesPerSec): //   FAT1
        break;
    case (FATRootDirSec0 * FATBytesPerSec): //   Root Directory
        FAT_RootDirWriteRequest(FAT_LBA, data, len);
        break;
    default:
        if (FAT_LBA >= (FATDataSec0 * FATBytesPerSec))
        {
            FAT_DataSectorWriteRequest(FAT_LBA, data, len);
        }
        break;
    }

    return FATBytesPerSec;
}

uint32_t FATSetStatusFileName(const char *name)
{
    uint8_t i;
    uint8_t len = (uint8_t)strlen(name);

    for (i = 0; i < 8 && i < len; i++)
    {
        FAT16_ReadyFileName[i] = name[i];
    }

    for (; i < 8; i++)
    {
        FAT16_ReadyFileName[i] = ' ';
    }

    return i;
}
