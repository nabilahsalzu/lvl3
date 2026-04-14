/* ================================================================
   menu.c  —  Main Menu Screen and Help Screen

   MAIN MENU LAYOUT
   ─────────────────
   Two minions on left and right at road level
   Centred panel with:
     - "WORD SCRAMBLE" title
     - Divider line
     - "Hello, [name]!" greeting
     - 4 buttons: New Game / How to Play / Score Board / Exit
   "Change Player" outline button below the panel

   HELP SCREEN LAYOUT
   ───────────────────
   Centred panel with:
     - "HOW TO PLAY" title
     - Divider line
     - 5 instruction lines
   BACK button below the panel

   HOW TO EDIT POSITIONS
   ──────────────────────
   All sizes and gaps are #define constants in Section 1.
   ================================================================ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "menu.h"
#include "gfx.h"
#include "theme.h"
#include "player.h"
#include "scoreboard.h"

/* ================================================================
   SECTION 1  —  LAYOUT CONSTANTS
   ================================================================ */

/* Panel width shared by both screens */
#define PANEL_WIDTH              420

/* Main menu panel */
#define MENU_PANEL_HEIGHT        360
#define MENU_PANEL_ABOVE_CENTRE  180   /* panel_y = screen_h/2 - this */

/* Inside the main menu panel */
#define MENU_TITLE_INSIDE_Y       34
#define MENU_DIVIDER_INSIDE_Y     65
#define MENU_GREETING_INSIDE_Y    78
#define MENU_FIRST_BTN_INSIDE_Y   100
#define MENU_BTN_GAP              18   /* gap between buttons          */

/* "Change Player" button below the main menu panel */
#define CHANGE_PLAYER_BTN_WIDTH  180
#define CHANGE_PLAYER_BTN_GAP     18   /* gap below panel bottom       */

/* Minions */
#define MINION_LEFT_X             60
#define MINION_RIGHT_OFFSET       60   /* right minion = screen_w - this */
#define MINION_ABOVE_ROAD         42

/* Help panel */
#define HELP_PANEL_HEIGHT        290
#define HELP_PANEL_ABOVE_CENTRE  160

/* Inside the help panel */
#define HELP_TITLE_INSIDE_Y       34
#define HELP_DIVIDER_INSIDE_Y     52
#define HELP_FIRST_LINE_INSIDE_Y  80
#define HELP_LINE_SPACING         36   /* vertical gap between lines   */

/* BACK button gap below panels */
#define BACK_BTN_GAP_BELOW        16

/* Button size — fills panel minus 20px each side */
#define BTN_WIDTH    (PANEL_WIDTH - 40)
#define BTN_HEIGHT   THEME_BTN_H

/* ================================================================
   SECTION 2  —  SOUND HELPER
   ================================================================ */
static void play_sound(const char *sound_filename)
{
    char command[128];
    snprintf(command, sizeof(command), "aplay -q sounds/%s &", sound_filename);
    system(command);
}

/* ================================================================
   SECTION 3  —  MAIN MENU DRAW
   ================================================================ */
void draw_menu(void)
{
    int screen_width  = gfx_xsize();
    int screen_height = gfx_ysize();

    theme_draw_bg(screen_width, screen_height);
    theme_draw_header_bar(screen_width);

    /* Minions at road level on both sides */
    int road_y = screen_height * 72 / 100;
    theme_draw_minion(MINION_LEFT_X,
                      road_y - MINION_ABOVE_ROAD);
    theme_draw_minion(screen_width - MINION_RIGHT_OFFSET,
                      road_y - MINION_ABOVE_ROAD);

    /* Panel */
    int panel_x = (screen_width - PANEL_WIDTH) / 2;
    int panel_y = screen_height / 2 - MENU_PANEL_ABOVE_CENTRE;
    int btn_x   = panel_x + 20;

    theme_draw_panel(panel_x, panel_y, PANEL_WIDTH, MENU_PANEL_HEIGHT);

    /* Title — "MINION SCRAMBLE" centred */
    {
        const char *title   = "MINION SCRAMBLE";
        int         title_w = (int)strlen(title) * 6 * 2;
        int         title_x = panel_x + (PANEL_WIDTH - title_w) / 2;
        int         title_y = panel_y + MENU_TITLE_INSIDE_Y;
        theme_draw_title(title, title_x, title_y, 2);
    }

    theme_draw_divider(panel_x + 20,
                       panel_y + MENU_DIVIDER_INSIDE_Y,
                       PANEL_WIDTH - 40);

    /* Greeting — "Bello, [player name]!" centred */
    {
        char greeting[48];
        sprintf(greeting, "Bello minion '%s'", current_player.name);
        int greeting_w = (int)strlen(greeting) * 8;
        int greeting_x = panel_x + (PANEL_WIDTH - greeting_w) / 2;
        int greeting_y = panel_y + MENU_GREETING_INSIDE_Y;
        theme_draw_subtitle(greeting, greeting_x, greeting_y);
    }

    /* 4 buttons evenly spaced */
    int first_btn_y = panel_y + MENU_FIRST_BTN_INSIDE_Y;
    int btn_step    = BTN_HEIGHT + MENU_BTN_GAP;

    theme_draw_button(btn_x, first_btn_y,
                      BTN_WIDTH, BTN_HEIGHT,
                      COL_BTN_GOLD, "New Mission");

    theme_draw_button(btn_x, first_btn_y + btn_step,
                      BTN_WIDTH, BTN_HEIGHT,
                      COL_BTN_DEFAULT, "Minion Training");

    theme_draw_button(btn_x, first_btn_y + btn_step * 2,
                      BTN_WIDTH, BTN_HEIGHT,
                      COL_BTN_DEFAULT, "Top Minions");

    theme_draw_button(btn_x, first_btn_y + btn_step * 3,
                      BTN_WIDTH, BTN_HEIGHT,
                      COL_BTN_RED, "Leave Lab");

    /* "Change Minion" outline button below the panel */
    {
        int change_btn_x = screen_width / 2 - CHANGE_PLAYER_BTN_WIDTH / 2;
        int change_btn_y = panel_y + MENU_PANEL_HEIGHT + CHANGE_PLAYER_BTN_GAP;
        theme_draw_button_outline(change_btn_x, change_btn_y,
                                  CHANGE_PLAYER_BTN_WIDTH,
                                  THEME_BTN_H_SMALL,
                                  255, 215, 0, "Change minion");
    }
}

/* ================================================================
   SECTION 4  —  MAIN MENU CLICK HANDLER
   ================================================================ */
GameState menu_handle_click(int mouse_x, int mouse_y)
{
    int screen_width  = gfx_xsize();
    int screen_height = gfx_ysize();
    int panel_x       = (screen_width - PANEL_WIDTH) / 2;
    int panel_y       = screen_height / 2 - MENU_PANEL_ABOVE_CENTRE;
    int btn_x         = panel_x + 20;
    int first_btn_y   = panel_y + MENU_FIRST_BTN_INSIDE_Y;
    int btn_step      = BTN_HEIGHT + MENU_BTN_GAP;

    int click_in_column =
        mouse_x >= btn_x &&
        mouse_x <= btn_x + BTN_WIDTH;

    if (click_in_column) {
        if (mouse_y >= first_btn_y &&
            mouse_y <  first_btn_y + BTN_HEIGHT) {
            play_sound("menu_click.wav");
            return STATE_CATEGORY;
        }
        if (mouse_y >= first_btn_y + btn_step &&
            mouse_y <  first_btn_y + btn_step + BTN_HEIGHT) {
            play_sound("menu_click.wav");
            return STATE_HELP;
        }
        if (mouse_y >= first_btn_y + btn_step * 2 &&
            mouse_y <  first_btn_y + btn_step * 2 + BTN_HEIGHT) {
            play_sound("menu_click.wav");
            return STATE_SCOREBOARD;
        }
        if (mouse_y >= first_btn_y + btn_step * 3 &&
            mouse_y <  first_btn_y + btn_step * 3 + BTN_HEIGHT) {
            play_sound("menu_click.wav");
            return STATE_EXIT;
        }
    }

    /* Change Player button */
    {
        int change_btn_x = screen_width / 2 - CHANGE_PLAYER_BTN_WIDTH / 2;
        int change_btn_y = panel_y + MENU_PANEL_HEIGHT + CHANGE_PLAYER_BTN_GAP;
        if (mouse_x >= change_btn_x &&
            mouse_x <= change_btn_x + CHANGE_PLAYER_BTN_WIDTH &&
            mouse_y >= change_btn_y &&
            mouse_y <= change_btn_y + THEME_BTN_H_SMALL) {
            play_sound("menu_click.wav");
            return STATE_SELECT_PLAYER;
        }
    }

    return STATE_MENU;
}

/* ================================================================
   SECTION 5  —  HELP SCREEN DRAW
   ================================================================ */
void draw_help(void)
{
    int screen_width  = gfx_xsize();
    int screen_height = gfx_ysize();

    theme_draw_bg(screen_width, screen_height);
    theme_draw_header_bar(screen_width);

    int panel_x = (screen_width - PANEL_WIDTH) / 2;
    int panel_y = screen_height / 2 - HELP_PANEL_ABOVE_CENTRE;
    int text_x  = panel_x + 20;

    theme_draw_panel(panel_x, panel_y, PANEL_WIDTH, HELP_PANEL_HEIGHT);

    /* Title */
    {
        const char *title   = "MINION TRAINING";
        int         title_w = (int)strlen(title) * 6 * 2;
        int         title_x = panel_x + (PANEL_WIDTH - title_w) / 2;
        int         title_y = panel_y + HELP_TITLE_INSIDE_Y;
        theme_draw_title(title, title_x, title_y, 2);
    }

    theme_draw_divider(panel_x + 20,
                       panel_y + HELP_DIVIDER_INSIDE_Y,
                       PANEL_WIDTH - 40);

    /* Instruction lines centered */
    int line_y = panel_y + HELP_FIRST_LINE_INSIDE_Y;
    const char *lines[] = {
        "Click scrambled letters to find the word.",
        "Each letter drops into the next slot.",
        "Click a filled slot to kick it out.",
        "Correct answer = +10 bananas!",
        "3 lives and 25 seconds per word."
    };
    for (int i = 0; i < 5; i++) {
        int line_w = (int)strlen(lines[i]) * 6;
        int center_x = panel_x + (PANEL_WIDTH - line_w) / 2;
        theme_draw_label(lines[i], center_x, line_y + i * HELP_LINE_SPACING);
    }

    /* BACK button */
    int back_btn_y = panel_y + HELP_PANEL_HEIGHT + BACK_BTN_GAP_BELOW;
    theme_draw_button(panel_x + 20, back_btn_y,
                      BTN_WIDTH, BTN_HEIGHT,
                      COL_BTN_GOLD, "BACK TO LAB");
}

/* ================================================================
   SECTION 6  —  HELP SCREEN CLICK HANDLER
   ================================================================ */
GameState help_handle_click(int mouse_x, int mouse_y)
{
    int screen_width  = gfx_xsize();
    int screen_height = gfx_ysize();
    int panel_x       = (screen_width - PANEL_WIDTH) / 2;
    int panel_y       = screen_height / 2 - HELP_PANEL_ABOVE_CENTRE;
    int back_btn_x    = panel_x + 20;
    int back_btn_y    = panel_y + HELP_PANEL_HEIGHT + BACK_BTN_GAP_BELOW;

    int clicked_back =
        mouse_x >= back_btn_x &&
        mouse_x <= back_btn_x + BTN_WIDTH &&
        mouse_y >= back_btn_y &&
        mouse_y <= back_btn_y + BTN_HEIGHT;

    if (clicked_back) {
        play_sound("menu_click.wav");
        return STATE_MENU;
    }

    return STATE_HELP;
}

GameState game_update(void);
