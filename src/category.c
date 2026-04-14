/* ================================================================
   category.c  —  Category Selection Screen

   SCREEN LAYOUT
   ─────────────
   Centred panel with:
     - "SELECT CATEGORY" title
     - Divider line
     - 4 category buttons
   BACK button below the panel

   HOW TO EDIT POSITIONS
   ──────────────────────
   All sizes and positions are #define constants in Section 1.
   ================================================================ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "category.h"
#include "gfx.h"
#include "theme.h"
#include "intro.h"
#include "game.h"

/* ================================================================
   SECTION 1  —  LAYOUT CONSTANTS
   ================================================================ */

#define PANEL_WIDTH              420
#define PANEL_HEIGHT             340
#define PANEL_ABOVE_CENTRE       170   /* panel_y = screen_h/2 - this */

/* Inside the panel */
#define TITLE_INSIDE_Y            34
#define DIVIDER_INSIDE_Y          52
#define FIRST_BTN_INSIDE_Y        68
#define BTN_GAP                   12   /* gap between category buttons */

/* BACK button gap below panel */
#define BACK_BTN_GAP_BELOW        16

/* Button size — fills panel minus 20px each side */
#define BTN_WIDTH    (PANEL_WIDTH - 40)
#define BTN_HEIGHT   THEME_BTN_H

/* Total number of category buttons */
#define NUM_CATEGORIES  4

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
   SECTION 3  —  CATEGORY DATA
   Each category has: button colour (r,g,b), label text, puzzle file.
   Edit the label or filename here to change what each button does.
   Index 3 (Random Mix) has no fixed file — it picks one at random.
   ================================================================ */
typedef struct {
    int         colour_r;
    int         colour_g;
    int         colour_b;
    const char *button_label;
    const char *puzzle_filename;   /* NULL = random */
} CategoryEntry;

static const CategoryEntry category_list[NUM_CATEGORIES] = {
    {  35, 130,  55, "1.  Home & Colors",   "data/puzzle1.txt" },
    {  35, 130,  55, "2.  Nature & School", "data/puzzle2.txt" },
    {   0,  80, 170, "3.  Tech & Science",  "data/puzzle3.txt" },
    {  85,  60, 155, "4.  Random Mix",      NULL               }
};

/* ================================================================
   SECTION 4  —  DRAW
   ================================================================ */
void draw_category_menu(void)
{
    int screen_width  = gfx_xsize();
    int screen_height = gfx_ysize();

    theme_draw_bg(screen_width, screen_height);
    theme_draw_header_bar(screen_width);

    /* Panel */
    int panel_x = (screen_width - PANEL_WIDTH)  / 2;
    int panel_y = screen_height / 2 - PANEL_ABOVE_CENTRE;
    int btn_x   = panel_x + 20;

    theme_draw_panel(panel_x, panel_y, PANEL_WIDTH, PANEL_HEIGHT);

    /* Title — "SELECT CATEGORY" */
    {
        const char *title   = "SELECT CATEGORY";
        int         title_w = (int)strlen(title) * 8 * 2;
        int         title_x = panel_x + (PANEL_WIDTH - title_w) / 2;
        int         title_y = panel_y + TITLE_INSIDE_Y;
        theme_draw_title(title, title_x, title_y, 2);
    }

    theme_draw_divider(panel_x + 20,
                       panel_y + DIVIDER_INSIDE_Y,
                       PANEL_WIDTH - 40);

    /* 4 category buttons */
    {
        int i;
        int first_btn_y = panel_y + FIRST_BTN_INSIDE_Y;
        int btn_step    = BTN_HEIGHT + BTN_GAP;

        for (i = 0; i < NUM_CATEGORIES; i++) {
            int btn_y = first_btn_y + i * btn_step;
            theme_draw_button(btn_x, btn_y, BTN_WIDTH, BTN_HEIGHT,
                              category_list[i].colour_r,
                              category_list[i].colour_g,
                              category_list[i].colour_b,
                              category_list[i].button_label);
        }
    }

    /* BACK button below panel */
    int back_btn_y = panel_y + PANEL_HEIGHT + BACK_BTN_GAP_BELOW;
    theme_draw_button(btn_x, back_btn_y,
                      BTN_WIDTH, BTN_HEIGHT,
                      COL_BTN_GREY, "BACK");
}

/* ================================================================
   SECTION 5  —  CLICK HANDLER
   ================================================================ */
GameState category_handle_click(int mouse_x, int mouse_y)
{
    int screen_width  = gfx_xsize();
    int screen_height = gfx_ysize();
    int panel_x       = (screen_width - PANEL_WIDTH)  / 2;
    int panel_y       = screen_height / 2 - PANEL_ABOVE_CENTRE;
    int btn_x         = panel_x + 20;
    int first_btn_y   = panel_y + FIRST_BTN_INSIDE_Y;
    int btn_step      = BTN_HEIGHT + BTN_GAP;
    int i;

    /* Check each category button */
    for (i = 0; i < NUM_CATEGORIES; i++) {
        int btn_y = first_btn_y + i * btn_step;

        if (mouse_x >= btn_x         &&
            mouse_x <= btn_x + BTN_WIDTH &&
            mouse_y >= btn_y          &&
            mouse_y <  btn_y + BTN_HEIGHT)
        {
            char chosen_file[32];

            /* Random Mix picks a random puzzle file */
            if (category_list[i].puzzle_filename == NULL)
                sprintf(chosen_file, "data/puzzle%d.txt", rand() % 3 + 1);
            else
                strcpy(chosen_file, category_list[i].puzzle_filename);

            play_sound("menu_click.wav");
            intro_init(chosen_file);
            return STATE_INTRO;
        }
    }

    /* BACK button */
    {
        int back_btn_y = panel_y + PANEL_HEIGHT + BACK_BTN_GAP_BELOW;
        if (mouse_x >= btn_x         &&
            mouse_x <= btn_x + BTN_WIDTH &&
            mouse_y >= back_btn_y     &&
            mouse_y <  back_btn_y + BTN_HEIGHT) {
            play_sound("menu_click.wav");
            return STATE_MENU;
        }
    }

    return STATE_CATEGORY;
}
