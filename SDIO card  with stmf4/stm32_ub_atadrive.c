// ------------------------------------------------ --------------
// File: stm32_ub_atadrive.c
// Date: 26/02/2013
// Version: 1.0
// Author: UB
// EMail: mc-4u (@) t-online.de
// Web: www.mikrocontroller-4u.de
// CPU: STM32F4
// IDE: CooCox Coide 1.7.0
// Module: No
// Function: FATFS file system for ATA media
// LoLevel IO module (only dummy functions)
// ------------------------------------------------ --------------


// ------------------------------------------------ --------------
// Includes
// ------------------------------------------------ --------------
#include "stm32_ub_atadrive.h"


// ------------------------------------------------ --------------
// Init the hardware for the ATA features
// Must be done before using again
//--------------------------------------------------------------
void UB_ATADrive_Init(void)
{

}

// ------------------------------------------------ --------------
// Check if media is inserted
// Return value:
//> 0 = if media is inserted
// 0 = if no media is inserted
// ------------------------------------------------ --------------

uint8_t UB_ATADrive_CheckMedia(void)
{
  uint8_t ret_wert=0;

  return(ret_wert);
}

// ------------------------------------------------ --------------
// Init the disk
// Return value:
// 0 = all ok
// <0 = error
// ------------------------------------------------ --------------
int ATA_disk_initialize(void)
{
  int ret_wert=-1;

  return(ret_wert);
}


//--------------------------------------------------------------
// Disk Status abfragen
// Return Wert :
//    0 = alles ok
//  < 0 = Fehler
//--------------------------------------------------------------
int ATA_disk_status(void)
{
  int ret_wert=-1;

  return(ret_wert);
}


// ------------------------------------------------ --------------
// READ function
// Return value:
// 0 = all ok
// <0 = error
// ------------------------------------------------ --------------
int ATA_disk_read(BYTE *buff, DWORD sector, BYTE count)
{
  int ret_wert=-1;

  return(ret_wert);
}


// ------------------------------------------------ --------------
// WRITE function
// Return value:
// 0 = all ok
// <0 = error
// ------------------------------------------------ --------------
int ATA_disk_write(const BYTE *buff, DWORD sector, BYTE count)
{
  int ret_wert=-1;

  return(ret_wert);
}


// ------------------------------------------------ --------------
// IOCTL function
// Return value:
// 0 = all ok
// <0 = error
// ------------------------------------------------ --------------
int ATA_disk_ioctl(BYTE cmd, void *buff)
{
  int ret_wert=-1;

  return(ret_wert);
}


