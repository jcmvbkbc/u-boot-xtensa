/*
 * Driver for the LCD display on the Tensilica XT-AV60 Board.
 *
 * Copyright (C) 2001 - 2013 Tensilica Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*
 * The LCD functions busy wait due to limitations of the LCD controller.
 * This is a single-threaded boot-loader, so we don't care about that.
 */


#include <config.h>
#include <common.h>

#define LCD_INSTR_ADDR		(char *)(CONFIG_SYS_ASCDISP_ADDR + 0)
#define LCD_DATA_ADDR		(char *)(CONFIG_SYS_ASCDISP_ADDR + 4)

#define LCD_PAUSE_ITERATIONS	4000
#define LCD_CLEAR		0x1
#define LCD_DISPLAY_ON		0xc

/* The KC704 requires 4-bit mode */
#ifdef CONFIG_XTFPGA_KC705
# define LCD_DISPLAY_FUNC	0x28	/* 4 bits and 2 lines display */
# define LCD_DISPLAY_MODE	0x06	/* increase, not-shifted */
#else
# define LCD_DISPLAY_FUNC	0x38	/* 8 bits and 2 lines display */
# define LCD_DISPLAY_MODE	0x06	/* increase, not-shifted */
#endif

#define LCD_SHIFT_LEFT		0x18
#define LCD_SHIFT_RIGHT		0x1c
#define LCD_DISPLAY_POS		0x80

#ifdef CONFIG_XTFPGA_KC705
static inline void lcd_write(char *addr, char value)
{
	*addr = value;
	udelay(500);
	*addr = value << 4;
	udelay(500);
}
#else
static inline void lcd_write(char *addr, char value)
{
	*addr = value;
	udelay(500);
}
#endif

void lcd_init(void)
{
	/* switching back to 8-bit mode needs at least 3 writes */
	*LCD_INSTR_ADDR = 0x33;
	udelay(500);
	*LCD_INSTR_ADDR = 0x33;
	udelay(500);
	*LCD_INSTR_ADDR = 0x33;
	udelay(500);
	*LCD_INSTR_ADDR = LCD_DISPLAY_FUNC;
	udelay(500);
	lcd_write(LCD_INSTR_ADDR, LCD_DISPLAY_FUNC);
	lcd_write(LCD_INSTR_ADDR, LCD_DISPLAY_ON);
	lcd_write(LCD_INSTR_ADDR, LCD_DISPLAY_MODE);
	lcd_write(LCD_INSTR_ADDR, LCD_CLEAR);
	udelay(2000);
}

void lcd_disp_at_pos(char *str, char pos)
{
	lcd_write(LCD_INSTR_ADDR, LCD_DISPLAY_POS | pos);
	udelay(100);
	while (*str != 0)
		lcd_write(LCD_DATA_ADDR, *str++);
}

void lcd_shiftleft(void)
{
	lcd_write(LCD_INSTR_ADDR, LCD_SHIFT_LEFT);
}

void lcd_shiftright(void)
{
	lcd_write(LCD_INSTR_ADDR, LCD_SHIFT_RIGHT);
}
