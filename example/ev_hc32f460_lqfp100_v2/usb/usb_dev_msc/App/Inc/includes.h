#ifndef __INCLUDES_H__
#define __INCLUDES_H__

#include "hc32_ddl.h"

#pragma anon_unions
#pragma pack(1)

#define LCD_DB10_PORT (PortC)
#define LCD_DB10_PIN (Pin00)
#define LCD_DB11_PORT (PortC)
#define LCD_DB11_PIN (Pin01)
#define LCD_DB12_PORT (PortC)
#define LCD_DB12_PIN (Pin02)
#define LCD_DB13_PORT (PortC)
#define LCD_DB13_PIN (Pin03)
#define LCD_DB14_PORT (PortC)
#define LCD_DB14_PIN (Pin04)
#define LCD_DB15_PORT (PortC)
#define LCD_DB15_PIN (Pin05)
#define LCD_DB16_PORT (PortC)
#define LCD_DB16_PIN (Pin06)
#define LCD_DB17_PORT (PortC)
#define LCD_DB17_PIN (Pin07)
#define LCD_REST_PORT (PortB)
#define LCD_REST_PIN (Pin03)
#define LCD_LED_PORT (PortB)
#define LCD_LED_PIN (Pin04)
#define LCD_CS_PORT (PortB)
#define LCD_CS_PIN (Pin05)
#define LCD_RS_PORT (PortB)
#define LCD_RS_PIN (Pin06)
#define LCD_WR_PORT (PortB)
#define LCD_WR_PIN (Pin07)

#define KEY2_PORT (PortB)
#define KEY2_PIN (Pin01)
#define KEY3_PORT (PortB)
#define KEY3_PIN (Pin02)
#define KEY1_PORT (PortB)
#define KEY1_PIN (Pin10)

#define KEY1_VAL() (PORT_GetBit(KEY1_PORT, KEY1_PIN))
#define KEY2_VAL() (PORT_GetBit(KEY2_PORT, KEY2_PIN))
#define KEY3_VAL() (PORT_GetBit(KEY3_PORT, KEY3_PIN))

//#include "ILI9341.h"

#pragma pack()

#endif
