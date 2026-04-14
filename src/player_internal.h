#ifndef PLAYER_INTERNAL_H
#define PLAYER_INTERNAL_H

#include "theme.h"

#define PANEL_WIDTH               420

#define CHOICE_PANEL_HEIGHT       230
#define CHOICE_PANEL_ABOVE_CENTRE 130
#define CHOICE_TITLE_INSIDE_Y      28
#define CHOICE_DIVIDER_INSIDE_Y    52
#define CHOICE_SUBTITLE_INSIDE_Y   68
#define CHOICE_BTN1_INSIDE_Y      100
#define CHOICE_BTN_GAP             14

#define NEW_PANEL_HEIGHT           240
#define NEW_PANEL_ABOVE_CENTRE     120
#define NEW_TITLE_INSIDE_Y          28
#define NEW_DIVIDER_INSIDE_Y        52
#define NEW_INPUT_INSIDE_Y          68
#define NEW_INPUT_HEIGHT             42
#define NEW_CONFIRM_BTN_GAP         16

#define BACK_BTN_X        THEME_MARGIN
#define BACK_BTN_Y        (THEME_MARGIN + 8)
#define BACK_BTN_WIDTH    110
#define BACK_BTN_HEIGHT   THEME_BTN_H_SMALL

#define OLD_HEADER_HEIGHT   80
#define OLD_FOOTER_HEIGHT   20
#define OLD_ROW_HEIGHT      (THEME_BTN_H + 14)
#define OLD_FIRST_ROW_Y     68
#define OLD_MIN_PANEL_TOP   20

#define MINION_LEFT_X       60
#define MINION_RIGHT_OFFSET 60
#define MINION_ABOVE_ROAD   42

#define BTN_WIDTH    (PANEL_WIDTH - 40)
#define BTN_HEIGHT   THEME_BTN_H

#define MAX_SAVED_PLAYERS  10

#endif
