#!/bin/sh
#
# Copyright (C) 2009 Arcadyan
# All Rights Reserved.
#

LCD_PROC_FILE=/proc/driver/lcd904

echo shutdown > $LCD_PROC_FILE
echo init     > $LCD_PROC_FILE

echo rect   0 0  39 239 0xffff > $LCD_PROC_FILE
echo rect  40 0  79 239 0x0000 > $LCD_PROC_FILE
echo rect  80 0 119 239 0xf800 > $LCD_PROC_FILE
echo rect 120 0 159 239 0x07e0 > $LCD_PROC_FILE
echo rect 160 0 199 239 0x001f > $LCD_PROC_FILE
echo rect 200 0 239 239 0xffe0 > $LCD_PROC_FILE
echo rect 240 0 279 239 0x07ff > $LCD_PROC_FILE
echo rect 280 0 319 239 0xf81f > $LCD_PROC_FILE
echo flush                     > $LCD_PROC_FILE

sleep 2

echo rect 0   0 319  29 0xffff > $LCD_PROC_FILE
echo rect 0  30 319  59 0x0000 > $LCD_PROC_FILE
echo rect 0  60 319  89 0xf800 > $LCD_PROC_FILE
echo rect 0  90 319 119 0x07e0 > $LCD_PROC_FILE
echo rect 0 120 319 149 0x001f > $LCD_PROC_FILE
echo rect 0 150 319 179 0xffe0 > $LCD_PROC_FILE
echo rect 0 180 319 209 0x07ff > $LCD_PROC_FILE
echo rect 0 210 319 239 0xf81f > $LCD_PROC_FILE
echo flush                     > $LCD_PROC_FILE

