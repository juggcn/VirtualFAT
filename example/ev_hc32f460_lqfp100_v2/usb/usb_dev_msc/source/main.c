/*******************************************************************************
 * Copyright (C) 2020, Huada Semiconductor Co., Ltd. All rights reserved.
 *
 * This software component is licensed by HDSC under BSD 3-Clause license
 * (the "License"); You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                    opensource.org/licenses/BSD-3-Clause
 */
/******************************************************************************/
/** \file main.c
 **
 ** \brief USB MSC device example.
 **
 **   - 2019-05-15  1.0  CDT First version for USB MSC device demo.
 **
 ******************************************************************************/

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "hc32_ddl.h"
#include "usb_dev_user.h"
#include "usb_dev_desc.h"
#include "usb_bsp.h"
#include "usb_dev_msc_class.h"

#include "fat32.h"
#include "includes.h"

#define USB_RENUMN_PORT (PortC)
#define USB_RENUMN_PIN (Pin08)

usb_core_instance usb_dev;

static void JumpToBootloader(uint32_t ulAppAdd)
{
    uint32_t i = 0;
    void (*SysMemBootJump)(void);      /* 声明一个函数指针 */
    __IO uint32_t BootAddr = ulAppAdd; /* APP 地址 */

    /* 关闭全局中断 */
    __set_PRIMASK(1);

    /* 关闭滴答定时器，复位到默认值 */
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;

    /* 设置所有时钟到默认状态，使用HSI时钟 */
    // HAL_RCC_DeInit();

    for (uint8_t i = 0; i < Int143_IRQn; i++)
    {
        enIrqResign(i);
    }

    /* 关闭所有中断，清除所有中断挂起标志 */
    for (i = 0; i < 8; i++)
    {
        NVIC->ICER[i] = 0xFFFFFFFF;
        NVIC->ICPR[i] = 0xFFFFFFFF;
    }

    /* 使能全局中断 */
    __set_PRIMASK(0);

    /* 跳转到系统BootLoader，首地址是MSP，地址+4是复位中断服务程序地址 */
    SysMemBootJump = (void (*)(void))(*((uint32_t *)(BootAddr + 4)));

    /* 设置主堆栈指针 */
    __set_MSP(*(uint32_t *)BootAddr);

    /* 在RTOS工程，这条语句很重要，设置为特权级模式，使用MSP指针 */
    __set_CONTROL(0);

    /* 跳转到系统BootLoader */
    SysMemBootJump();

    /* 跳转成功的话，不会执行到这里，用户可以在这里添加代码 */
    while (1)
    {
    }
}

static int lKeyVal(int lKeyIndex)
{
    int lVal = 1;
    static int32_t lKeyValPrev[3] = {1, 1, 1};
    switch (lKeyIndex)
    {
    case 1:
        if (lKeyValPrev[0] != KEY1_VAL())
        {
            lKeyValPrev[0] = KEY1_VAL();
            if (KEY1_VAL() == 0)
            {
                lVal = 0;
            }
        }
        break;
    case 2:
        if (lKeyValPrev[1] != KEY2_VAL())
        {
            lKeyValPrev[1] = KEY2_VAL();
            if (KEY2_VAL() == 0)
            {
                lVal = 0;
            }
        }
        break;
    case 3:
        if (lKeyValPrev[2] != KEY3_VAL())
        {
            lKeyValPrev[2] = KEY3_VAL();
            if (KEY3_VAL() == 0)
            {
                lVal = 0;
            }
        }
        break;
    default:
        break;
    }
    return lVal;
}

extern usb_dev_user_Sta_t usb_dev_user_Sta;
static usb_dev_user_Sta_t usb_dev_user_Sta_Prev;

extern void LCD_SetXY(uint16_t usX, uint16_t usY);
extern void LCD_GetXY(uint16_t *pusX, uint16_t *pusY);

static void BSP_GPIO_Init(void);

int32_t main(void)
{
    /* clock config */
    BSP_CLK_Init();
    /* USER CODE BEGIN Init */
    BSP_GPIO_Init();

//    while (!fat32_init())
//    {
//        // flash addr error
//    }

    if (KEY2_VAL() == 0 &&
        KEY3_VAL() == 0)
    {
        PORT_SetBits(USB_RENUMN_PORT, USB_RENUMN_PIN);
        hd_usb_dev_init(&usb_dev, &user_desc, &usb_dev_msc_cbk, &user_cb);
//        ILI9341_TM280_Initial2();
//        LCD_Clear(0x0000);
        PORT_SetBits(LCD_LED_PORT, LCD_LED_PIN);
//        printf("UC-Nano bootload v1.0\n");
//        printf("Wait for the USB Host to connect...\n");
        Ddl_Delay1ms(2000);
        usb_dev_user_Sta_Prev = usb_dev_user_Sta;
        while (1)
        {
            if (PORT_GetBit(USB_RENUMN_PORT, USB_RENUMN_PIN)) //表示USB已初始化
            {
                if (usb_dev_user_Sta_Prev != usb_dev_user_Sta)
                {
                    usb_dev_user_Sta_Prev = usb_dev_user_Sta;
                    switch (usb_dev_user_Sta)
                    {
                    case USB_DEV_USER_STA_CFG:
                    case USB_DEV_USER_STA_CONNECTED:
//                        printf("The USB Host is connected.\n");
                        break;
                    default:
//                        printf("The USB Host is not connected.\n");
                        break;
                    }
                }
            }
            Ddl_Delay1ms(100);
        }
    }
    else
    {
        JumpToBootloader(0x10000);
    }
    while (1)
    {
        ;
    }
}

static void BSP_GPIO_Init(void)
{
    PORT_DebugPortSetting(TDO_SWO, Disable);
    PORT_DebugPortSetting(TDI, Disable);
    PORT_DebugPortSetting(TRST, Disable);

    stc_port_init_t stcPortInit;

    /* configuration structure initialization */
    MEM_ZERO_STRUCT(stcPortInit);

    stcPortInit.enPinMode = Pin_Mode_Out;
    stcPortInit.enExInt = Enable;
    stcPortInit.enPullUp = Enable;

    PORT_Init(USB_RENUMN_PORT, USB_RENUMN_PIN, &stcPortInit);

    PORT_Init(LCD_DB10_PORT, LCD_DB10_PIN, &stcPortInit);
    PORT_Init(LCD_DB11_PORT, LCD_DB11_PIN, &stcPortInit);
    PORT_Init(LCD_DB12_PORT, LCD_DB12_PIN, &stcPortInit);
    PORT_Init(LCD_DB13_PORT, LCD_DB13_PIN, &stcPortInit);
    PORT_Init(LCD_DB14_PORT, LCD_DB14_PIN, &stcPortInit);
    PORT_Init(LCD_DB15_PORT, LCD_DB15_PIN, &stcPortInit);
    PORT_Init(LCD_DB16_PORT, LCD_DB16_PIN, &stcPortInit);
    PORT_Init(LCD_DB17_PORT, LCD_DB17_PIN, &stcPortInit);
    PORT_Init(LCD_REST_PORT, LCD_REST_PIN, &stcPortInit);
    PORT_Init(LCD_LED_PORT, LCD_LED_PIN, &stcPortInit);
    PORT_Init(LCD_CS_PORT, LCD_CS_PIN, &stcPortInit);
    PORT_Init(LCD_RS_PORT, LCD_RS_PIN, &stcPortInit);
    PORT_Init(LCD_WR_PORT, LCD_WR_PIN, &stcPortInit);

    MEM_ZERO_STRUCT(stcPortInit);

    stcPortInit.enPinMode = Pin_Mode_In;
    stcPortInit.enExInt = Enable;
    stcPortInit.enPullUp = Enable;

    PORT_Init(KEY2_PORT, KEY2_PIN, &stcPortInit);
    PORT_Init(KEY3_PORT, KEY3_PIN, &stcPortInit);
    PORT_Init(KEY1_PORT, KEY1_PIN, &stcPortInit);
}

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
