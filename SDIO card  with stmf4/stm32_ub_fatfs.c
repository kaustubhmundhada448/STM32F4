// ------------------------------------------------ --------------
// File: stm32_ub_fatfs.c
// Date: 01.05.2014
// Version : 1.4
// Author : UB
// EMail: mc- 4u ( @ ) t-online.de
// Web : www.mikrocontroller - 4u.de
// CPU: STM32F4
// IDE : CooCox Coide 1.7.4
// GCC 4.7 2012q4
// Module : FATFS ( GPIO , MISC , SDIO DMA)
// Function: File functions by FatFS library
// To MMC devices
//
// Note : Number of devices: 1
// Type of Device: MMC
// (For USB or MMC + USB there are other LIBs )
//
// Document all FATFS functions " doc/00index_e.html "
//
// Speed  MMC : Read block = 2.38 MB / sec ( buffer = 512 bytes)
// Write Block = 499kByte/sec (buffer = 512 bytes)
// ------------------------------------------------ --------------
// Warning : The STM32F4 gene rates in revision - A a
// " SDIO_IT_DCRCFAIL " in function " SD_ProcessIRQSrc "
// To avoid this set the Define " SDIO_STM32F4_REV " to 0
// ( Check the STM Errata Sheet for more information )
// [ Thanks to Major for posting this BUG ... UB ]
// ------------------------------------------------ --------------

// ------------------------------------------------ --------------
// Pin assignment: [ MMC ]
//
// 1-bit mode:
//
// PC8 -> SDIO_D0 = SD card DAT0
// PC12 -> SDIO_CK = SD Card Clock
// PD2 -> SDIO_CMD = SD card CMD
//
// 4bit mode:
//
// PC8 -> SDIO_D0 = SD card DAT0
// PC9 -> SDIO_D1 = SD card DAT1
// PC10 -> SDIO_D2 = SD card DAT2
// PC11 -> SDIO_D3 = SD card DAT3/CD
// PC12 -> SDIO_CK = SD Card Clock
// PD2 -> SDIO_CMD = SD card CMD
//
// With Detect pin:
//
// PC0 -> SD_Detect - Pin (Hi = without SD card)
// ------------------------------------------------ --------------


// ------------------------------------------------ --------------
// Includes
// ------------------------------------------------ --------------

#include "stm32_ub_fatfs.h"



// ------------------------------------------------ --------------
// Global variables
// ------------------------------------------------ --------------
static FATFS FileSystemObject ;


// ------------------------------------------------ --------------
// Init function
// (Init all systems )
// ------------------------------------------------ --------------
void UB_Fatfs_Init  (void)
{
  // Init the hardware for the SDCard functions
  UB_SDCard_Init ();
  // Init the hardware for USB functions
  UB_USBDisk_Init ();
  // Init the hardware for the ATA features
  UB_ATADrive_Init ();
}

// ------------------------------------------------ --------------
// Request status from the medium
// Dev: [ MMC_0 ]
// Return value:
// FATFS_OK => media is inserted
// FATFS_NO_MEDIA => no media inserted
// ------------------------------------------------ --------------


FATFS_t UB_Fatfs_CheckMedia(MEDIA_t dev)
{
  FATFS_t ret_wert=FATFS_NO_MEDIA;
  uint8_t check=SD_NOT_PRESENT;

  // Check if media is inserted
  if(dev==MMC_0) check=UB_SDCard_CheckMedia();
  if(check==SD_PRESENT) {
    ret_wert=FATFS_OK;
  }
  else {
    ret_wert=FATFS_NO_MEDIA;
  }

  return(ret_wert);
}

// ------------------------------------------------ --------------
// Media mount
// Dev: [MMC_0]
// Return value:
// FATFS_OK => no error
// FATFS_MOUNT_ERR => error
// FATFS_GETFREE_ERR => error
// ------------------------------------------------ --------------
FATFS_t UB_Fatfs_Mount(MEDIA_t dev)
{
  FATFS_t ret_wert=FATFS_MOUNT_ERR;
  FRESULT check=FR_INVALID_PARAMETER;
  DWORD fre_clust;
  FATFS	*fs;

  if(dev==MMC_0) check=f_mount(0, &FileSystemObject);
  if(check == FR_OK) {
    if(dev==MMC_0) check=f_getfree("0:", &fre_clust, &fs);
    if(check == FR_OK) {
      ret_wert=FATFS_OK;
    }
    else {
      ret_wert=FATFS_GETFREE_ERR;
    }
  }
  else {
    ret_wert=FATFS_MOUNT_ERR;
  }

  return(ret_wert);
}


// ------------------------------------------------ --------------
// Media unmount
// Dev: [MMC_0]
// Return value:
// FATFS_OK => no error
// FATFS_MOUNT_ERR => error
// ------------------------------------------------ --------------

FATFS_t UB_Fatfs_UnMount(MEDIA_t dev)
{
  FATFS_t ret_wert=FATFS_MOUNT_ERR;
  FRESULT check=FR_INVALID_PARAMETER;

  if(dev==MMC_0) check=f_mount(0, NULL);
  if(check == FR_OK) {
    ret_wert=FATFS_OK;
  }
  else {
    ret_wert=FATFS_MOUNT_ERR;
  }

  return(ret_wert);
}


// ------------------------------------------------ --------------
//Delete / File
// File can not be opened
// Name is the complete path, for example, "0 :/ Test.txt"
// Return value:
// FATFS_OK => no error
// FATFS_UNLINK_ERR => error
// ------------------------------------------------ --------------

FATFS_t UB_Fatfs_DelFile(const char* name)
{
  FATFS_t ret_wert=FATFS_UNLINK_ERR;
  FRESULT check=FR_INVALID_PARAMETER;

  check=f_unlink(name);
  if(check==FR_OK) {
    ret_wert=FATFS_OK;
  }
  else {
    ret_wert=FATFS_UNLINK_ERR;
  }

  return(ret_wert);
}


// ------------------------------------------------ --------------
// File open (for reading or writing)
// Transfer file via &-operator
// Name is the complete path, for example, "0 :/ Test.txt"
// Mode: [F_RD, F_WR, F_WR_NEW, F_WR_CLEAR]
// Return value:
// FATFS_OK => no error
// FATFS_OPEN_ERR => error
// FATFS_SEEK_ERR => error at WR and WR_NEW
// ------------------------------------------------ --------------

FATFS_t UB_Fatfs_OpenFile(FIL* fp, const char* name, FMODE_t mode)
{
  FATFS_t ret_wert=FATFS_OPEN_ERR;
  FRESULT check=FR_INVALID_PARAMETER;

  if(mode==F_RD) check = f_open(fp, name, FA_OPEN_EXISTING | FA_READ); 
  if(mode==F_WR) check = f_open(fp, name, FA_OPEN_EXISTING | FA_WRITE);
  if(mode==F_WR_NEW) check = f_open(fp, name, FA_OPEN_ALWAYS | FA_WRITE);
  if(mode==F_WR_CLEAR) check = f_open(fp, name, FA_CREATE_ALWAYS | FA_WRITE);

  if(check==FR_OK) {
    ret_wert=FATFS_OK;
    if((mode==F_WR) || (mode==F_WR_NEW)) {
    	// Ask Pointer to the end of file
      check = f_lseek(fp, f_size(fp));
      if(check!=FR_OK) {
        ret_wert=FATFS_SEEK_ERR;
      }
    }
  }
  else {
    ret_wert=FATFS_OPEN_ERR;
  }   

  return(ret_wert);
}

// ------------------------------------------------ --------------
//Close  File
// Transfer file via &-operator
// Return value:
// FATFS_OK => no error
// FATFS_CLOSE_ERR => error
// ------------------------------------------------ --------------

FATFS_t UB_Fatfs_CloseFile(FIL* fp)
{
  FATFS_t ret_wert=FATFS_CLOSE_ERR;
  FRESULT check=FR_INVALID_PARAMETER;

  check=f_close(fp);

  if(check==FR_OK) {
    ret_wert=FATFS_OK;
  }
  else {
    ret_wert=FATFS_CLOSE_ERR;
  }   

  return(ret_wert);
}

// ------------------------------------------------ --------------
// Write string to file
// File must be open
// Transfer file via &-operator
// Line-ending ('\ n') is automatically appended
// Return value:
// FATFS_OK => no error
// FATFS_PUTS_ERR => error
// ------------------------------------------------ --------------

FATFS_t UB_Fatfs_WriteString(FIL* fp, const char* text)
{
  FATFS_t ret_wert=FATFS_PUTS_ERR;
  int check=0;

  check=f_puts(text, fp);

  if(check>=0) {
    ret_wert=FATFS_OK;
    // Add line-end
    f_putc('\n', fp);
  }
  else {
    ret_wert=FATFS_PUTS_ERR;
  }   

  return(ret_wert);
}


// ------------------------------------------------ --------------
// Read a string from a file
// File must be open
// Transfer file via &operator
// Text: String
// Len: Size of the string buffer
// There are sign read
// until end of file or end of string reached
// Return value:
// FATFS_OK => no error
// FATFS_EOF => end of file reached
// FATFS_RD_STRING_ERR => error
// ------------------------------------------------ --------------
FATFS_t UB_Fatfs_ReadString(FIL* fp, char* text, uint32_t len)
{
  FATFS_t ret_wert=FATFS_RD_STRING_ERR;
  int check;

  f_gets(text, len, fp);
  check=f_eof(fp);
  if(check!=0) return(FATFS_EOF);
  check=f_error(fp);
  if(check!=0) return(FATFS_RD_STRING_ERR);
  ret_wert=FATFS_OK;

  return(ret_wert);
}


// ------------------------------------------------ --------------
//Read File Size
// File must be open
// Transfer file via &-operator
// Return value:
//> 0 => File size in bytes
// 0 => error
//--------------------------------------------------------------

uint32_t UB_Fatfs_FileSize(FIL* fp)
{
  uint32_t ret_wert=0;
  int filesize;

  filesize=f_size(fp);
  if(filesize>=0) ret_wert=(uint32_t)(filesize);

  return(ret_wert);
}


// ------------------------------------------------ --------------
// Read data block from a file
// File must be open
// Transfer file via &-operator
// Buf: a buffer for the data
// Len: Size of the data buffer (max 512 bytes)
// There are (len) sign read
// Or until end of file is reached
// Read: number of characters that have been read (if err => 0)
// Return value:
// FATFS_OK => no error
// FATFS_EOF => end of file reached
// FATFS_RD_BLOCK_ERR => error
// ------------------------------------------------ --------------

FATFS_t UB_Fatfs_ReadBlock(FIL* fp, unsigned char* buf, uint32_t len, uint32_t* read)
{
  FATFS_t ret_wert=FATFS_RD_BLOCK_ERR;
  FRESULT check=FR_INVALID_PARAMETER;
  UINT ulen,uread;

  ulen=(UINT)(len);
  if(ulen>_MAX_SS) {
    ret_wert=FATFS_RD_BLOCK_ERR;
    *read=0;
  }
  else {
    check=f_read(fp, buf, ulen, &uread);
    if(check==FR_OK) {
      *read=(uint32_t)(uread);
      if(ulen==uread) {
        ret_wert=FATFS_OK;
      }
      else {
        ret_wert=FATFS_EOF;
      }
    }
    else {
      ret_wert=FATFS_RD_BLOCK_ERR;
      *read=0;
    }
  }

  return(ret_wert);
}


// ------------------------------------------------ --------------
// Write data block in a file
// File must be open
// Transfer file via &-operator
// Buf: data (in a buffer)
// Len: Size of the data buffer (max 512 bytes)
// There are (len) characters written
// Write: Number of characters were written (with err => 0)
// Return value:
// FATFS_OK => no error
// FATFS_DISK_FULL => no more space
// FATFS_WR_BLOCK_ERR => error
// ------------------------------------------------ --------------

FATFS_t UB_Fatfs_WriteBlock(FIL* fp, unsigned char* buf, uint32_t len, uint32_t* write)
{
  FATFS_t ret_wert=FATFS_WR_BLOCK_ERR;
  FRESULT check=FR_INVALID_PARAMETER;
  UINT ulen,uwrite;

  ulen=(UINT)(len);
  if(ulen>_MAX_SS) {
    ret_wert=FATFS_WR_BLOCK_ERR;
    *write=0;
  }
  else {
    check=f_write(fp, buf, ulen, &uwrite);
    if(check==FR_OK) {
      *write=(uint32_t)(uwrite);
      if(ulen==uwrite) {
        ret_wert=FATFS_OK;
      }
      else {
        ret_wert=FATFS_DISK_FULL;
      }
    }
    else {
      ret_wert=FATFS_WR_BLOCK_ERR;
      *write=0;
    }
  }

  return(ret_wert);
}

