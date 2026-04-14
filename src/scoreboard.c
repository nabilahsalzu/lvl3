/* ================================================================
   scoreboard.c  —  Score Board Screen

   Shows the current player's top 10 scores, sorted best first.
   The most recently submitted score is highlighted in blue.
   The all-time best score is highlighted in gold.

   SCREEN LAYOUT
   ─────────────
   Centred panel (height grows to fit score rows):
     - "[Player name]'s Scores" title
     - Divider
     - "TOP 10 BEST SCORES" label
     - Up to 10 score rows
   BACK button below the panel

   HOW TO EDIT POSITIONS
   ──────────────────────
   All sizes and gaps are #define constants in Section 1.
   ================================================================ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "scoreboard.h"
#include "gfx.h"
#include "theme.h"
#include "player.h"

/* ================================================================
   SECTION 1  —  LAYOUT CONSTANTS
   ================================================================ */

/* Panel */
#define SCORE_ROW_WIDTH         380   /* width of each score row              */
#define PANEL_WIDTH             (SCORE_ROW_WIDTH + 40)   /* = 420             */
#define PANEL_MIN_TOP_Y          10   /* panel never goes above this Y        */

/* Panel height is calculated dynamically:
   PANEL_HEADER_HEIGHT + (number_of_rows * SCORE_ROW_HEIGHT) + PANEL_FOOTER_HEIGHT */
#define PANEL_HEADER_HEIGHT      80   /* space for title + divider + sub-label */
#define PANEL_FOOTER_HEIGHT      20   /* padding below last row               */
#define SCORE_ROW_HEIGHT         38   /* height of each score row             */

/* Inside the panel */
#define TITLE_INSIDE_Y           28
#define DIVIDER_INSIDE_Y         52
#define SUB_LABEL_INSIDE_Y       58   /* "TOP 10 BEST SCORES" label           */
#define FIRST_ROW_INSIDE_Y       74   /* Y of the first score row             */

/* BACK button gap below the panel */
#define BACK_BTN_GAP_BELOW       16

/* Button size */
#define BACK_BTN_WIDTH           200
#define BACK_BTN_HEIGHT          THEME_BTN_H

/* Maximum scores to show */
#define MAX_DISPLAY_SCORES        10

/* Maximum scores to read from file */
#define MAX_SCORES_FROM_FILE     100

/* ================================================================
   SECTION 2  —  INTERNAL STATE
   ================================================================ */
static int scores[MAX_SCORES_FROM_FILE];
static int number_of_scores = 0;
static int latest_score     = -1;   /* the most recently submitted score */

/* ================================================================
   SECTION 3  —  LOAD SCORES FROM FILE
   Reads all scores for the current player, finds the latest one,
   then sorts them from highest to lowest and keeps only the top 10.
   ================================================================ */
static void load_scores_from_file(void)
{
    char  filepath[64];
    FILE *file;
    int   score_value;
    int   i, j, temp;

    number_of_scores = 0;
    latest_score     = -1;

    sprintf(filepath, "players/%s.txt", current_player.name);
    file = fopen(filepath, "r");
    if (!file) return;

    /* Read all scores from file */
    while (number_of_scores < MAX_SCORES_FROM_FILE &&
           fscanf(file, "%d", &score_value) == 1) {
        scores[number_of_scores] = score_value;
        number_of_scores++;
    }
    fclose(file);

    /* The last score in the file is the most recent one */
    if (number_of_scores > 0)
        latest_score = scores[number_of_scores - 1];

    /* Sort scores from highest to lowest (bubble sort) */
    for (i = 0; i < number_of_scores - 1; i++) {
        for (j = i + 1; j < number_of_scores; j++) {
            if (scores[j] > scores[i]) {
                temp      = scores[i];
                scores[i] = scores[j];
                scores[j] = temp;
            }
        }
    }

    /* Keep only the top 10 */
    if (number_of_scores > MAX_DISPLAY_SCORES)
        number_of_scores = MAX_DISPLAY_SCORES;
}

/* ================================================================
   SECTION 4  —  HELPER: calculate panel Y position
   ================================================================ */
static int get_panel_y(int screen_height, int panel_height)
{
    int panel_y = screen_height / 2 - panel_height / 2;
    if (panel_y < PANEL_MIN_TOP_Y)
        panel_y = PANEL_MIN_TOP_Y;
    return panel_y;
}

/* ================================================================
   SECTION 5  —  DRAW SCOREBOARD SCREEN
   ================================================================ */
void draw_scoreboard(void)
{
    int screen_width  = gfx_xsize();
    int screen_height = gfx_ysize();

    theme_draw_bg(screen_width, screen_height);
    theme_draw_header_bar(screen_width);

    /* Load the current player's scores fresh each time we draw */
    load_scores_from_file();

    /* Calculate panel height — at least 1 row even if no scores */
    int visible_rows = number_of_scores > 0 ? number_of_scores : 1;
    int panel_height = PANEL_HEADER_HEIGHT
                     + visible_rows * SCORE_ROW_HEIGHT
                     + PANEL_FOOTER_HEIGHT;

    int panel_x = (screen_width - PANEL_WIDTH) / 2;
    int panel_y = get_panel_y(screen_height, panel_height);

    theme_draw_panel(panel_x, panel_y, PANEL_WIDTH, panel_height);

    /* ── Title: "[Name]'s Scores" centred ──────────────────── */
    {
        char title_text[64];
        sprintf(title_text, "%s's Scores", current_player.name);
        int title_w = (int)strlen(title_text) * 8 * 2;
        int title_x = panel_x + (PANEL_WIDTH - title_w) / 2;
        int title_y = panel_y + TITLE_INSIDE_Y;
        theme_draw_title(title_text, title_x, title_y, 2);
    }

    theme_draw_divider(panel_x + 20,
                       panel_y + DIVIDER_INSIDE_Y,
                       PANEL_WIDTH - 40);

    /* ── "TOP 10 BEST SCORES" sub-label centred ─────────────── */
    {
        const char *sub_label   = "TOP 10 BEST SCORES";
        int         sub_label_w = (int)strlen(sub_label) * 8;
        int         sub_label_x = panel_x + (PANEL_WIDTH - sub_label_w) / 2;
        int         sub_label_y = panel_y + SUB_LABEL_INSIDE_Y;
        theme_draw_label(sub_label, sub_label_x, sub_label_y);
    }

    /* ── Score rows or "no scores" message ──────────────────── */
    int rows_x = panel_x + 20;
    int rows_y = panel_y + FIRST_ROW_INSIDE_Y;

    if (number_of_scores == 0) {
        /* No scores saved yet */
        const char *empty_msg   = "No scores yet — play a game first!";
        int         empty_msg_w = (int)strlen(empty_msg) * 8;
        int         empty_msg_x = panel_x + (PANEL_WIDTH - empty_msg_w) / 2;
        theme_draw_subtitle(empty_msg, empty_msg_x, rows_y + 10);
    } else {
        int i;
        int already_marked_latest = 0;   /* make sure we only mark latest once */

        for (i = 0; i < number_of_scores; i++) {
            int rank        = i + 1;                  /* 1st, 2nd, 3rd ... */
            int score_value = scores[i];
            int is_best     = (i == 0);               /* first = highest score */
            int is_latest   = (score_value == latest_score &&
                               !already_marked_latest);
            if (is_latest) already_marked_latest = 1;

            int row_y = rows_y + i * SCORE_ROW_HEIGHT;

            theme_draw_score_row(rank, score_value,
                                 is_best, is_latest,
                                 rows_x, row_y,
                                 SCORE_ROW_WIDTH);
        }
    }

    /* ── BACK button centred below the panel ─────────────────── */
    int back_btn_x = (screen_width - BACK_BTN_WIDTH) / 2;
    int back_btn_y = panel_y + panel_height + BACK_BTN_GAP_BELOW;
    theme_draw_button(back_btn_x, back_btn_y,
                      BACK_BTN_WIDTH, BACK_BTN_HEIGHT,
                      COL_BTN_GREY, "BACK");
}

/* ================================================================
   SECTION 6  —  CLICK HANDLER
   ================================================================ */
GameState scoreboard_handle_click(int mouse_x, int mouse_y)
{
    int screen_width  = gfx_xsize();
    int screen_height = gfx_ysize();

    /* Recalculate panel position to find where BACK button is */
    int visible_rows = number_of_scores > 0 ? number_of_scores : 1;
    int panel_height = PANEL_HEADER_HEIGHT
                     + visible_rows * SCORE_ROW_HEIGHT
                     + PANEL_FOOTER_HEIGHT;
    int panel_y      = get_panel_y(screen_height, panel_height);

    int back_btn_x = (screen_width - BACK_BTN_WIDTH) / 2;
    int back_btn_y = panel_y + panel_height + BACK_BTN_GAP_BELOW;

    int clicked_back =
        mouse_x >= back_btn_x &&
        mouse_x <= back_btn_x + BACK_BTN_WIDTH &&
        mouse_y >= back_btn_y &&
        mouse_y <= back_btn_y + BACK_BTN_HEIGHT;

    if (clicked_back)
        return STATE_MENU;

    return STATE_SCOREBOARD;
}
