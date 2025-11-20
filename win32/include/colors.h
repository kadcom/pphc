/*
 * colors.h - Color scheme matching openpajak.hamardikan.com
 * Windows NT 4.0+ compatible color definitions
 */

#ifndef PPHCALC_COLORS_H
#define PPHCALC_COLORS_H

#include <windows.h>

/* Header colors (Dark navy theme) */
#define COLOR_HEADER_BG         RGB(30, 41, 59)      /* #1E293B */
#define COLOR_HEADER_TEXT       RGB(255, 255, 255)   /* White */
#define COLOR_ACTIVE_TAB        RGB(234, 179, 8)     /* Gold/Yellow #EAB308 */
#define COLOR_INACTIVE_TAB      RGB(55, 65, 81)      /* Darker gray */
#define COLOR_TAB_TEXT          RGB(209, 213, 219)   /* Light gray */

/* Background colors */
#define COLOR_WINDOW_BG         RGB(250, 248, 243)   /* Light cream #FAF8F3 */
#define COLOR_PANEL_BG          RGB(255, 255, 255)   /* White */
#define COLOR_SECTION_HEADER    RGB(254, 243, 199)   /* Light yellow #FEF3C7 */
#define COLOR_SUMMARY_BG        RGB(254, 243, 199)   /* Light yellow */
#define COLOR_SUMMARY_BORDER    RGB(234, 179, 8)     /* Gold border */

/* Text colors */
#define COLOR_TEXT_PRIMARY      RGB(17, 24, 39)      /* Dark gray #111827 */
#define COLOR_TEXT_SECONDARY    RGB(107, 114, 128)   /* Medium gray #6B7280 */
#define COLOR_TEXT_LABEL        RGB(75, 85, 99)      /* Gray #4B5563 */
#define COLOR_TEXT_AMOUNT       RGB(17, 24, 39)      /* Dark for amounts */

/* Border and input colors */
#define COLOR_BORDER            RGB(229, 231, 235)   /* Light gray #E5E7EB */
#define COLOR_INPUT_BORDER      RGB(209, 213, 219)   /* Gray #D1D5DB */
#define COLOR_INPUT_BG          RGB(255, 255, 255)   /* White */
#define COLOR_INPUT_FOCUS       RGB(234, 179, 8)     /* Gold focus */

/* Button colors */
#define COLOR_BTN_PRIMARY_BG    RGB(30, 41, 59)      /* Dark navy */
#define COLOR_BTN_PRIMARY_TEXT  RGB(255, 255, 255)   /* White */
#define COLOR_BTN_SECONDARY_BG  RGB(243, 244, 246)   /* Light gray */
#define COLOR_BTN_SECONDARY_TEXT RGB(75, 85, 99)     /* Gray */

/* Helper functions for creating brushes and pens */
HBRUSH CreateColorBrush(COLORREF color);
HPEN CreateColorPen(COLORREF color, int width);

#endif /* PPHCALC_COLORS_H */
