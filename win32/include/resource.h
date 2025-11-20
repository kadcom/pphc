/*
 * resource.h - Resource IDs and control identifiers
 * Windows NT 4.0+ compatible
 */

#ifndef PPHCALC_RESOURCE_H
#define PPHCALC_RESOURCE_H

/* Application info */
#define APP_NAME            "PPh Kalkulator"
#define APP_VERSION         "1.0"
#define APP_TITLE           "Kalkulator PPh 21/26 - Pajak Indonesia"

/* Control IDs - Input Panel */
#define IDC_CMB_SUBJECT         1001
#define IDC_EDIT_BRUTO          1002
#define IDC_EDIT_MONTHS         1003
#define IDC_SPIN_MONTHS         1004
#define IDC_EDIT_PENSION        1005
#define IDC_EDIT_BONUS          1006
#define IDC_CMB_PTKP            1007
#define IDC_EDIT_ZAKAT          1008
#define IDC_CMB_SCHEME          1009
#define IDC_CMB_TER_CATEGORY    1010

/* Buttons - Input Panel */
#define IDC_BTN_RESET           1020
#define IDC_BTN_EXAMPLE         1021

/* Control IDs - Results Panel */
#define IDC_LISTVIEW_BREAKDOWN  2001
#define IDC_STATIC_TOTAL_TAX    2002
#define IDC_STATIC_TER_MONTHLY  2003
#define IDC_STATIC_DEC_BALANCE  2004
#define IDC_STATIC_THP_ANNUAL   2005
#define IDC_STATIC_THP_MONTHLY  2006

/* Buttons - Results Panel */
#define IDC_BTN_COPY            2020
#define IDC_BTN_PRINT           2021
#define IDC_BTN_EXPORT          2022

/* Header tabs */
#define IDC_TAB_PPH21           3001
#define IDC_TAB_PPH22           3002
#define IDC_TAB_PPH23           3003
#define IDC_TAB_PPH42           3004
#define IDC_BTN_ABOUT           3005

/* Menus */
#define IDM_FILE_EXIT           4001
#define IDM_EDIT_COPY           4002
#define IDM_HELP_ABOUT          4003

/* String IDs */
#define IDS_APP_TITLE           5001
#define IDS_ERROR_TITLE         5002
#define IDS_CONFIRM_RESET       5003

/* Layout constants */
#define HEADER_HEIGHT           56
#define MARGIN                  16
#define CONTROL_HEIGHT          24
#define LABEL_HEIGHT            16
#define SPACING                 8
#define PANEL_WIDTH             360
#define BUTTON_WIDTH            100
#define BUTTON_HEIGHT           32

/* Font sizes */
#define FONT_SIZE_HEADER        14
#define FONT_SIZE_LABEL         9
#define FONT_SIZE_AMOUNT        16
#define FONT_SIZE_NORMAL        9

#endif /* PPHCALC_RESOURCE_H */
