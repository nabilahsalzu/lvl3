/* ================================================================
   result.c  —  Game Result Screen

   Shown when the game ends (all words done, out of lives, or quit).
   Displays the player name, final score, result message,
   and two buttons: PLAY AGAIN and MAIN MENU.

   SCREEN LAYOUT
   ─────────────
   Centred panel with:
     - "GAME RESULT" title
     - Divider
     - Result message (2 lines)
     - Player name
     - Gold score badge
     - Second divider
     - PLAY AGAIN button  |  MAIN MENU button

   HOW TO EDIT POSITIONS
   ──────────────────────
   All sizes and gaps are #define constants in Section 1.
   ================================================================ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "result.h"
#include "gfx.h"
#include "theme.h"
#include "player.h"

/* ================================================================
   SECTION 1  —  LAYOUT CONSTANTS
   ================================================================ */

#define PANEL_WIDTH              440
#define PANEL_HEIGHT             340
#define PANEL_ABOVE_CENTRE       170   /* panel_y = screen_h/2 - this */

/* Inside the panel */
#define TITLE_INSIDE_Y            34
#define DIVIDER_INSIDE_Y          52
#define RESULT_LINE1_INSIDE_Y     72   /* first result message line     */
#define RESULT_LINE2_INSIDE_Y     96   /* second result message line    */
#define PLAYER_NAME_INSIDE_Y     130   /* "Player: name" label          */
#define SCORE_BADGE_INSIDE_Y     156   /* gold score badge              */
#define SECOND_DIVIDER_INSIDE_Y  196   /* divider above buttons         */
#define BUTTONS_INSIDE_Y         216   /* both buttons at this Y        */

/* Score badge size */
#define SCORE_BADGE_HEIGHT        28
#define SCORE_BADGE_PADDING       24   /* extra width beyond text       */

/* Buttons (two side by side) */
/* Each button fills half the panel width minus margins and gap */
#define BTN_SIDE_MARGIN           20   /* margin from panel edge        */
#define BTN_MIDDLE_GAP            20   /* gap between the two buttons   */
#define BTN_WIDTH  ((PANEL_WIDTH - BTN_SIDE_MARGIN * 2 - BTN_MIDDLE_GAP) / 2)
#define BTN_HEIGHT  THEME_BTN_H

/* ================================================================
   SECTION 2  —  INTERNAL STATE
   ================================================================ */
static int stored_final_score = 0;
static int stored_end_reason  = 0;
/* end_reason values:
   0 = completed all words successfully
   1 = ran out of lives
   2 = player quit early (clicked MENU during game) */

/* Button positions stored so the click handler can use them */
typedef struct {
    int x;
    int y;
    int width;
    int height;
} ButtonBox;

static ButtonBox play_again_button;
static ButtonBox main_menu_button;

/* ================================================================
   SECTION 3  —  SETTERS
   Called from game.c before switching to STATE_RESULT.
   ================================================================ */
void result_set_score(int final_score)
{
    stored_final_score = final_score;
}

void result_set_end_reason(int end_reason)
{
    stored_end_reason = end_reason;
}

/* ================================================================
   SECTION 4  —  SOUND HELPER
   ================================================================ */
static void play_sound(const char *sound_filename)
{
    char command[128];
    snprintf(command, sizeof(command), "aplay -q sounds/%s &", sound_filename);
    system(command);
}

/* ================================================================
   SECTION 5  —  DRAW RESULT SCREEN
   ================================================================ */
void draw_result(void)
{
    int screen_width  = gfx_xsize();
    int screen_height = gfx_ysize();

    theme_draw_bg(screen_width, screen_height);
    theme_draw_header_bar(screen_width);

    int panel_x = (screen_width - PANEL_WIDTH) / 2;
    int panel_y = screen_height / 2 - PANEL_ABOVE_CENTRE;

    theme_draw_panel(panel_x, panel_y, PANEL_WIDTH, PANEL_HEIGHT);

    /* ── "GAME RESULT" title centred ──────────────────────── */
    {
        const char *title   = "GAME RESULT";
        int         title_w = (int)strlen(title) * 8 * 2;
        int         title_x = panel_x + (PANEL_WIDTH - title_w) / 2;
        int         title_y = panel_y + TITLE_INSIDE_Y;
        theme_draw_title(title, title_x, title_y, 2);
    }

    theme_draw_divider(panel_x + 20,
                       panel_y + DIVIDER_INSIDE_Y,
                       PANEL_WIDTH - 40);

    /* ── Result message — 2 lines centred ─────────────────── */
    {
        const char *line1;
        const char *line2;

        if (stored_end_reason == 0) {
            line1 = "Excellent! You cracked every clue!";
            line2 = "Well done, word master!";
        } else if (stored_end_reason == 1) {
            line1 = "Oh no — you ran out of lives!";
            line2 = "Better luck next time!";
        } else {
            line1 = "Mission ended early.";
            line2 = "Come back and finish the job!";
        }

        int line1_w = (int)strlen(line1) * 8;
        int line2_w = (int)strlen(line2) * 8;
        int line1_x = panel_x + (PANEL_WIDTH - line1_w) / 2;
        int line2_x = panel_x + (PANEL_WIDTH - line2_w) / 2;

        theme_draw_subtitle(line1, line1_x, panel_y + RESULT_LINE1_INSIDE_Y);
        theme_draw_subtitle(line2, line2_x, panel_y + RESULT_LINE2_INSIDE_Y);
    }

    /* ── Player name centred ───────────────────────────────── */
    {
        char player_label[64];
        sprintf(player_label, "Player: %s", current_player.name);
        int label_w = (int)strlen(player_label) * 8;
        int label_x = panel_x + (PANEL_WIDTH - label_w) / 2;
        theme_draw_label(player_label, label_x,
                         panel_y + PLAYER_NAME_INSIDE_Y);
    }

    /* ── Gold score badge centred ──────────────────────────── */
    {
        char score_text[24];
        sprintf(score_text, "SCORE  %d", stored_final_score);

        int badge_w = (int)strlen(score_text) * 8 + SCORE_BADGE_PADDING;
        int badge_h = SCORE_BADGE_HEIGHT;
        int badge_x = panel_x + (PANEL_WIDTH - badge_w) / 2;
        int badge_y = panel_y + SCORE_BADGE_INSIDE_Y;

        /* Shadow */
        gfx_color(0, 0, 0);
        gfx_fillrectangle(badge_x + 2, badge_y + 2, badge_w, badge_h);

        /* Gold background */
        gfx_color(185, 140, 0);
        gfx_fillrectangle(badge_x, badge_y, badge_w, badge_h);

        /* Top shine stripe */
        gfx_color(255, 218, 55);
        gfx_fillrectangle(badge_x + 2, badge_y + 2, badge_w - 4, 4);

        /* Black border */
        gfx_color(0, 0, 0);
        gfx_fillrectangle(badge_x,              badge_y,              badge_w, 2);
        gfx_fillrectangle(badge_x,              badge_y + badge_h - 2, badge_w, 2);
        gfx_fillrectangle(badge_x,              badge_y,              2, badge_h);
        gfx_fillrectangle(badge_x + badge_w - 2, badge_y,            2, badge_h);

        /* Score text centred inside badge */
        int text_x = badge_x + (badge_w - (int)strlen(score_text) * 8) / 2;
        int text_y = badge_y + (badge_h + 8) / 2 - 8;
        gfx_color(0, 0, 0);
        gfx_text(score_text, text_x + 1, text_y + 1, 1);
        gfx_color(25, 8, 0);
        gfx_text(score_text, text_x, text_y, 1);
    }

    theme_draw_divider(panel_x + 20,
                       panel_y + SECOND_DIVIDER_INSIDE_Y,
                       PANEL_WIDTH - 40);

    /* ── Two buttons side by side ──────────────────────────── */
    {
        int btn_y           = panel_y + BUTTONS_INSIDE_Y;
        int left_btn_x      = panel_x + BTN_SIDE_MARGIN;
        int right_btn_x     = left_btn_x + BTN_WIDTH + BTN_MIDDLE_GAP;

        /* Store button positions for click detection */
        play_again_button.x      = left_btn_x;
        play_again_button.y      = btn_y;
        play_again_button.width  = BTN_WIDTH;
        play_again_button.height = BTN_HEIGHT;

        main_menu_button.x      = right_btn_x;
        main_menu_button.y      = btn_y;
        main_menu_button.width  = BTN_WIDTH;
        main_menu_button.height = BTN_HEIGHT;

        theme_draw_button(left_btn_x,  btn_y, BTN_WIDTH, BTN_HEIGHT,
                          35, 140, 60, "PLAY AGAIN");
        theme_draw_button(right_btn_x, btn_y, BTN_WIDTH, BTN_HEIGHT,
                          160, 45, 55, "MAIN MENU");
    }
}

/* ================================================================
   SECTION 6  —  CLICK HANDLER
   ================================================================ */
GameState result_handle_click(int mouse_x, int mouse_y)
{
    /* PLAY AGAIN button */
    if (mouse_x >= play_again_button.x &&
        mouse_x <= play_again_button.x + play_again_button.width &&
        mouse_y >= play_again_button.y &&
        mouse_y <= play_again_button.y + play_again_button.height) {
        play_sound("menu_click.wav");
        return STATE_CATEGORY;
    }

    /* MAIN MENU button */
    if (mouse_x >= main_menu_button.x &&
        mouse_x <= main_menu_button.x + main_menu_button.width &&
        mouse_y >= main_menu_button.y &&
        mouse_y <= main_menu_button.y + main_menu_button.height) {
        play_sound("menu_click.wav");
        return STATE_MENU;
    }

    return STATE_RESULT;
}
