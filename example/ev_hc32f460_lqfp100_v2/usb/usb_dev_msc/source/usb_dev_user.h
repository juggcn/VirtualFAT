/*******************************************************************************
 * Copyright (C) 2020, Huada Semiconductor Co., Ltd. All rights reserved.
 *
 * This software component is licensed by HDSC under BSD 3-Clause license
 * (the "License"); You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                    opensource.org/licenses/BSD-3-Clause
 */
/******************************************************************************/
/** \file usb_dev_user.h
 **
 ** A detailed description is available at
 ** @link Header file for usbd_usr.c @endlink
 **
 **   - 2021-04-13  1.0  CDT First version for USB demo.
 **
 ******************************************************************************/
#ifndef __USB_DEV_USER_H__
#define __USB_DEV_USER_H__

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "usb_dev_core.h"

/*******************************************************************************
 * Global type definitions ('typedef')
 ******************************************************************************/
extern  usb_dev_user_func user_cb;

/*******************************************************************************
  Global function prototypes (definition in C source)
 ******************************************************************************/
typedef enum 
{
  USB_DEV_USER_STA_RST = 0,
  USB_DEV_USER_STA_CFG,
  USB_DEV_USER_STA_CONNECTED,
  USB_DEV_USER_STA_DISCONNECTED,
  USB_DEV_USER_STA_SUSPEND,
  USB_DEV_USER_STA_RESUMES,
} usb_dev_user_Sta_t;

#endif /*__USB_DEV_USER_H__*/

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
