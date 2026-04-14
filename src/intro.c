/* ================================================================
   intro.c  —  Category Intro Screen

   This screen appears after the player picks a category.
   It shows a short story description and an "I'M READY!" button.

   SCREEN LAYOUT
   ─────────────
   Panel at top of screen:
     - Category name as big title
     - Story line 1 (description)
     - Story line 2 (what to do)
   Minion drawn in the lower half of the screen
   "I'M READY!" button just below the panel

   HOW TO EDIT POSITIONS
   ──────────────────────
   All positions and sizes are #define constants in Section 1.
   Just change the number next to the name to move that element.
   ================================================================ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "intro.h"
#include "gfx.h"
#include "theme.h"
#include "game.h"

/* ================================================================
   SECTION 1  —  LAYOUT CONSTANTS
   Change these numbers to move or resize things on screen.
   ================================================================ */

/* Info panel (the box at the top showing the category info) */
#define INFO_PANEL_WIDTH        420   /* width of the panel in pixels          */
#define INFO_PANEL_HEIGHT       220   /* height of the panel in pixels         */
#define INFO_PANEL_TOP_Y         30   /* how far from top of screen            */

/* Inside the info panel */
#define TITLE_INSIDE_Y           22   /* Y of category title inside panel      */
#define DIVIDER_INSIDE_Y         50   /* Y of the divider line inside panel    */
#define LINE1_INSIDE_Y           72   /* Y of story line 1 inside panel        */
#define LINE2_INSIDE_Y          100   /* Y of story line 2 inside panel        */
#define TEXT_LEFT_PADDING        20   /* how far text starts from panel left   */

/* "I'M READY!" button */
#define READY_BTN_WIDTH         200   /* width of the ready button             */
#define READY_BTN_HEIGHT         50   /* height of the ready button            */
#define READY_BTN_GAP_BELOW_PANEL 18  /* gap between panel bottom and button  */

/* Minion position — drawn near the city road area */
#define MINION_ROAD_OFFSET       44   /* how far above the road the minion is  */

/* ================================================================
   SECTION 2  —  INTERNAL STATE
   These variables remember which category was chosen.
   ================================================================ */

/* Which category is currently selected (0=random, 1=home, 2=nature, 3=tech) */
static int category_id = 0;

/* The puzzle file path that was chosen in the category screen */
static char chosen_puzzle_file[64] = "";

/* ================================================================
   SECTION 3  —  SOUND HELPER
   ================================================================ */
static void play_sound(const char *sound_filename)
{
    char command[128];
    snprintf(command, sizeof(command), "aplay -q sounds/%s &", sound_filename);
    system(command);
}

/* ================================================================
   SECTION 4  —  INIT
   Called by category.c when the player picks a category.
   Stores the file path and figures out which category it is.
   ================================================================ */
void intro_init(const char *puzzle_filename)
{
    /* Remember which file to load when the game starts */
    strncpy(chosen_puzzle_file, puzzle_filename,
            sizeof(chosen_puzzle_file) - 1);
    chosen_puzzle_file[sizeof(chosen_puzzle_file) - 1] = '\0';

    /* Figure out which category number this file belongs to */
    category_id = 0;   /* default = random mix */
    if      (strstr(puzzle_filename, "puzzle1.txt")) category_id = 1;
    else if (strstr(puzzle_filename, "puzzle2.txt")) category_id = 2;
    else if (strstr(puzzle_filename, "puzzle3.txt")) category_id = 3;
}

/* ================================================================
   SECTION 5  —  CATEGORY TEXT DATA
   Edit these strings to change the intro messages for each category.
   Index 0 = Random Mix, 1 = Home & Colors, 2 = Nature & School,
         3 = Tech & Science
   ================================================================ */
static const char *category_names[4] = {
    "RANDOM MIX",
    "HOME & COLORS",
    "NATURE & SCHOOL",
    "TECH & SCIENCE"
};

static const char *category_story_line1[4] = {
    "A wild mix of everything!",
    "You are in a magic house!",
    "You are in a forest behind your school!",
    "You are in a futuristic computer lab!"
};

static const char *category_story_line2[4] = {
    "Are you ready for the challenge?",
    "Unscramble things and colors!",
    "Find nature and school words!",
    "Help fix the robot — unscramble tech words!"
};

/* ================================================================
   SECTION 6  —  DRAW
   Draws the full intro screen.
   ================================================================ */
void draw_intro(void)
{
    int screen_width  = gfx_xsize();
    int screen_height = gfx_ysize();

    /* ── Background and header bar ───────────────────────── */
    theme_draw_bg(screen_width, screen_height);
    theme_draw_header_bar(screen_width);

    /* ── Minion near the road at the bottom of the scene ─── */
    int road_y       = screen_height * 72 / 100;   /* road is at 72% down */
    int minion_x     = screen_width / 2;
    int minion_y     = road_y - MINION_ROAD_OFFSET;
    theme_draw_minion(minion_x, minion_y);

    /* ── Info panel ────────────────────────────────────────── */
    int panel_x = (screen_width - INFO_PANEL_WIDTH) / 2;
    int panel_y = INFO_PANEL_TOP_Y;

    theme_draw_panel(panel_x, panel_y, INFO_PANEL_WIDTH, INFO_PANEL_HEIGHT);

    /* ── Category name as big title ────────────────────────── */
    {
        const char *category_name  = category_names[category_id];
        int         name_length    = (int)strlen(category_name);
        /* Each character at scale 2 is 16px wide (8px * 2) */
        int         title_x        = screen_width / 2 - name_length * 8;
        int         title_y        = panel_y + TITLE_INSIDE_Y;
        theme_draw_title(category_name, title_x, title_y, 2);
    }

    /* ── Divider line below title ───────────────────────────── */
    theme_draw_divider(panel_x + TEXT_LEFT_PADDING,
                       panel_y + DIVIDER_INSIDE_Y,
                       INFO_PANEL_WIDTH - TEXT_LEFT_PADDING * 2);

    /* ── Story description lines ────────────────────────────── */
    int text_x = panel_x + TEXT_LEFT_PADDING;
    theme_draw_label(category_story_line1[category_id],
                     text_x,
                     panel_y + LINE1_INSIDE_Y);
    theme_draw_label(category_story_line2[category_id],
                     text_x,
                     panel_y + LINE2_INSIDE_Y);

    /* ── "I'M READY!" button centred below the panel ────────── */
    int ready_btn_x = screen_width / 2 - READY_BTN_WIDTH / 2;
    int ready_btn_y = panel_y + INFO_PANEL_HEIGHT + READY_BTN_GAP_BELOW_PANEL;

    theme_draw_button(ready_btn_x, ready_btn_y,
                      READY_BTN_WIDTH, READY_BTN_HEIGHT,
                      COL_BTN_GREEN, "I'M READY!");
}

/* ================================================================
   SECTION 7  —  CLICK HANDLER
   Checks if the player clicked the "I'M READY!" button.
   If yes → start the game with the chosen puzzle file.
   ================================================================ */
GameState intro_handle_click(int mouse_x, int mouse_y)
{
    int screen_width = gfx_xsize();

    /* Calculate where the ready button is (same as in draw_intro) */
    int ready_btn_x = screen_width / 2 - READY_BTN_WIDTH / 2;
    int ready_btn_y = INFO_PANEL_TOP_Y + INFO_PANEL_HEIGHT
                      + READY_BTN_GAP_BELOW_PANEL;

    /* Check if the click is inside the ready button */
    int clicked_inside_btn =
        mouse_x >= ready_btn_x &&
        mouse_x <= ready_btn_x + READY_BTN_WIDTH &&
        mouse_y >= ready_btn_y &&
        mouse_y <= ready_btn_y + READY_BTN_HEIGHT;

    if (clicked_inside_btn) {
        play_sound("menu_click.wav");
        game_init_with_file(chosen_puzzle_file);
        return STATE_PLAYING;
    }

    return STATE_INTRO;   /* nothing clicked — stay on intro screen */
}
