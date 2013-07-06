/*
 * Driver for the LCD display on the Tensilica LX60 Board.
 *
 * Copyright (C) 2001 - 2013 Tensilica Inc.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

void lcd_init(void);
void lcd_disp_at_pos(char *, unsigned char);
void lcd_shiftleft(void);
void lcd_shiftright(void);
