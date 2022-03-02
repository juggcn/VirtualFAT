#include "ILI9341.h"

#define LCD_Width LCD_WIDTH
#define LCD_Height LCD_HEIGHT

extern const char cFont8X16_Ascii[][16];

/**
 * @brief		显示一个ASCII码字符
 * @param   x,y		显示起始坐标
 * @param   ch		需要显示的字符
 * @param   size	字体大小(支持16/24/32号字体)
 * @return  none
 * @note		需要font.h字库文件的支持
 */
void LCD_ShowChar(uint16_t x, uint16_t y, char ch, uint16_t back_color, uint16_t font_color, uint8_t font_size, char *pFont)
{
	int i = 0, j = 0;
	uint8_t temp = 0;
	uint8_t size = 0;
	uint8_t t = 0;

	/* 检测显示是否会越界 */
	if ((x > (LCD_Width - font_size / 2)) || (y > (LCD_Height - font_size)))
		return;

	/* 根据字符大小设置显存操作区域 */
	LCD_SetArea(x, y, x + font_size / 2 - 1, y + font_size - 1);

	/* 计算字符在字库中的偏移值*/
	ch = ch - ' ';

	/* 显示16号/32号字体 */
	if ((font_size == 16) || (font_size == 32))
	{
		/* 计算字体一个字符对应点阵集所占的字节数 */
		size = (font_size / 8 + ((font_size % 8) ? 1 : 0)) * (font_size / 2);

		for (i = 0; i < size; i++)
		{
			// if (font_size == 16)
			// 	temp = asc2_1608[ch][i]; //调用1608字体
			// else if (font_size == 32)
			// 	temp = asc2_3216[ch][i]; //调用3216字体
			// else
			// 	return; //没有的字库
			// temp = pFont[]

			temp = cFont8X16_Ascii[ch][i];

			for (j = 0; j < 8; j++)
			{
				if (temp & 0x80)
				{
					LCD_WR_DATA16(font_color);
				}
				else
				{
					LCD_WR_DATA16(back_color);
				}

				temp <<= 1;
			}
		}
	}
	/* 显示12号字体 */
	else if (font_size == 12)
	{
		/* 计算字体一个字符对应点阵集所占的字节数 */
		size = (font_size / 8 + ((font_size % 8) ? 1 : 0)) * (font_size / 2);

		for (i = 0; i < size; i++)
		{
			// temp = asc2_1206[ch][i];

			for (j = 0; j < 6; j++)
			{
				if (temp & 0x80)
				{
					LCD_WR_DATA16(font_color);
				}
				else
				{
					LCD_WR_DATA16(back_color);
				}

				temp <<= 1;
			}
		}
	}
	/* 显示24号字体 */
	else if (font_size == 24)
	{
		/* 计算字体一个字符对应点阵集所占的字节数 */
		size = (font_size * 16) / 8;

		for (i = 0; i < size; i++)
		{
			//			temp = cFont12X24_Ascii[ch][i];
			if (i % 2 == 0)
				t = 8;
			else
				t = 4;
			for (j = 0; j < t; j++)
			{
				if (temp & 0x80)
				{
					LCD_WR_DATA16(font_color);
				}
				else
				{
					LCD_WR_DATA16(back_color);
				}

				temp <<= 1;
			}
		}
	}
	/* 其余字体 */
	else
		return;
}

/**
 * @brief		显示一个ASCII码字符串
 * @param   x,y		显示起始坐标
 * @param   str		需要显示的字符串
 * @param   size	字体大小(支持16/24/32号字体)
 * @return  none
 * @note		1. 需要font.h字库文件的支持
 * 					2. 超过指定width不显示超过的字符
 */
void LCD_ShowStr(uint16_t x, uint16_t y, uint8_t max_width, char *str, uint16_t back_color, uint16_t font_color, uint8_t font_size)
{

	max_width += x;

	while ((*str <= '~') && (*str >= ' ')) //判断是否非法字符
	{
		if (x >= max_width)
		{
			// x方向越界，结束
			break;
		}

		LCD_ShowChar(x, y, *str, back_color, font_color, font_size, NULL);
		x += font_size / 2;
		str++;
	}
}

#include "stdio.h"

static uint16_t x = 0;
static uint16_t y = 0;

void LCD_SetXY(uint16_t usX, uint16_t usY)
{
	x = usX;
	y = usY;
}

void LCD_GetXY(uint16_t *pusX, uint16_t *pusY)
{
	*pusX = x;
	*pusY = y;
}

int fputc(int ch, FILE *f)
{

	if (ch == '\n') //换行
	{
		x = 0;
		y += 16;
		return ch;
	}

	if (x > LCD_Width - 8)
	{
		x = 0;	 // x置零
		y += 16; // y移至下一行
	}
	if (y > LCD_Height - 16)
	{
		x = 0;
		y = 0;
		LCD_Clear(0x0000);
	}

	// LCD_Ascii_One(x, y, string, color); //打印字符ch
	LCD_ShowChar(x, y, ch, 0x0000, 0xffff, 16, NULL);
	x += 8; //跳转到下一个位置, 是否越界有上面函数判断

	return ch;
}
