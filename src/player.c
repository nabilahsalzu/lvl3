/* ================================================================
   player.c  —  Player Select Screen

   This screen appears at the start of the game.
   The player can choose to:
     - Create a new profile (type a name)
     - Continue as an existing player (pick from saved profiles)

   SCREEN MODES
   ─────────────
   MODE_CHOICE  — shows two buttons: "New Player" and "Existing Player"
   MODE_NEW     — shows a text input box to type a name
   MODE_OLD     — shows a list of saved player names to pick from

   HOW TO EDIT POSITIONS
   ──────────────────────
   All sizes and positions are #define constants in Section 1.
   ================================================================ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "player.h"
#include "gfx.h"
#include "theme.h"

/* ================================================================
   SECTION 1  —  LAYOUT CONSTANTS
   ================================================================ */

/* Panel shared by all three modes */
#define PANEL_WIDTH               420

/* MODE_CHOICE panel */
#define CHOICE_PANEL_HEIGHT       230
#define CHOICE_PANEL_ABOVE_CENTRE 130   /* panel_y = screen_h/2 - this */

/* Inside MODE_CHOICE panel */
#define CHOICE_TITLE_INSIDE_Y      28
#define CHOICE_DIVIDER_INSIDE_Y    52
#define CHOICE_SUBTITLE_INSIDE_Y   68
#define CHOICE_BTN1_INSIDE_Y      100   /* "NEW PLAYER" button Y      */
#define CHOICE_BTN_GAP             14   /* gap between the two buttons */

/* MODE_NEW panel */
#define NEW_PANEL_HEIGHT           240
#define NEW_PANEL_ABOVE_CENTRE     120

/* Inside MODE_NEW panel */
#define NEW_TITLE_INSIDE_Y          28
#define NEW_DIVIDER_INSIDE_Y        52
#define NEW_INPUT_INSIDE_Y          68   /* Y of the text input box    */
#define NEW_INPUT_HEIGHT             42  /* height of the text input   */
#define NEW_CONFIRM_BTN_GAP         16   /* gap below input to button  */

/* BACK button (top-left corner, same for MODE_NEW and MODE_OLD) */
#define BACK_BTN_X        THEME_MARGIN
#define BACK_BTN_Y        (THEME_MARGIN + 8)
#define BACK_BTN_WIDTH    110
#define BACK_BTN_HEIGHT   THEME_BTN_H_SMALL

/* MODE_OLD panel — height depends on how many saved players exist */
#define OLD_HEADER_HEIGHT   80   /* space for title + divider          */
#define OLD_FOOTER_HEIGHT   20   /* padding below last button          */
#define OLD_ROW_HEIGHT      (THEME_BTN_H + 14)   /* height per player row */
#define OLD_FIRST_ROW_Y     68   /* Y of first player button inside panel */
#define OLD_MIN_PANEL_TOP   20   /* panel top never goes above this Y  */

/* Minions (shown on MODE_CHOICE) */
#define MINION_LEFT_X       60
#define MINION_RIGHT_OFFSET 60
#define MINION_ABOVE_ROAD   42

/* Button size — fills panel minus 20px each side */
#define BTN_WIDTH    (PANEL_WIDTH - 40)
#define BTN_HEIGHT   THEME_BTN_H

/* Maximum saved players to show */
#define MAX_SAVED_PLAYERS  10

/* ================================================================
   SECTION 2  —  CURRENT PLAYER (accessible from other files)
   ================================================================ */
Player current_player = { "", 0 };

/* ================================================================
   SECTION 3  —  INTERNAL STATE
   ================================================================ */

/* Which sub-screen we are currently showing */
typedef enum {
    MODE_CHOICE,   /* initial screen — New Player or Existing Player */
    MODE_NEW,      /* type a new name                                */
    MODE_OLD       /* pick from saved player list                    */
} SelectionMode;

static SelectionMode current_mode = MODE_CHOICE;

/* Name typing state (used in MODE_NEW) */
static char name_input_text[32] = "";
static int  name_input_length   = 0;

/* Loaded existing player names (used in MODE_OLD) */
static char saved_player_names[MAX_SAVED_PLAYERS][32];
static int  saved_player_count = 0;

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
   SECTION 5  —  FILE HELPERS
   ================================================================ */

/* Make sure the "players" folder exists, create it if not */
static void ensure_players_folder_exists(void)
{
    struct stat folder_info = { 0 };
    if (stat("players", &folder_info) == -1)
        mkdir("players", 0700);
}

/* Read all saved player names from the players/ folder */
static void load_saved_player_names(void)
{
    DIR           *folder;
    struct dirent *entry;

    ensure_players_folder_exists();
    saved_player_count = 0;

    folder = opendir("players");
    if (!folder) return;

    while ((entry = readdir(folder)) != NULL &&
           saved_player_count < MAX_SAVED_PLAYERS)
    {
        /* Only include .txt files */
        char *dot_txt = strstr(entry->d_name, ".txt");
        if (dot_txt) {
            int name_length = (int)(dot_txt - entry->d_name);
            strncpy(saved_player_names[saved_player_count],
                    entry->d_name, name_length);
            saved_player_names[saved_player_count][name_length] = '\0';
            saved_player_count++;
        }
    }
    closedir(folder);
}

/* Save the current player's score to their file */
void save_player_data(void)
{
    char filepath[64];
    sprintf(filepath, "players/%s.txt", current_player.name);
    FILE *file = fopen(filepath, "a");
    if (file) {
        fprintf(file, "%d\n", current_player.high_score);
        fclose(file);
    }
}

/* Load a player's data from file.
   Sets current_player.name and current_player.high_score. */
void load_player_data(const char *player_name)
{
    char filepath[64];

    strncpy(current_player.name, player_name, 31);
    current_player.name[31] = '\0';

    sprintf(filepath, "players/%s.txt", player_name);
    FILE *file = fopen(filepath, "r");
    if (file) {
        fscanf(file, "%d", &current_player.high_score);
        fclose(file);
    } else {
        current_player.high_score = 0;   /* new player — no score yet */
    }
}

/* ================================================================
   SECTION 6  —  DRAW PLAYER SELECT SCREEN
   ================================================================ */
void draw_player_select(void)
{
    int screen_width  = gfx_xsize();
    int screen_height = gfx_ysize();

    theme_draw_bg(screen_width, screen_height);
    theme_draw_header_bar(screen_width);

    /* Panel is always centred horizontally */
    int panel_x = (screen_width - PANEL_WIDTH) / 2;
    int btn_x   = panel_x + 20;   /* buttons start 20px inside panel left */

    /* ── MODE_CHOICE ─────────────────────────────────────── */
    if (current_mode == MODE_CHOICE)
    {
        /* Minions at road level on both sides */
        int road_y = screen_height * 72 / 100;
        theme_draw_minion(MINION_LEFT_X,
                          road_y - MINION_ABOVE_ROAD);
        theme_draw_minion(screen_width - MINION_RIGHT_OFFSET,
                          road_y - MINION_ABOVE_ROAD);

        int panel_y = screen_height / 2 - CHOICE_PANEL_ABOVE_CENTRE;
        theme_draw_panel(panel_x, panel_y, PANEL_WIDTH, CHOICE_PANEL_HEIGHT);

        /* Title */
        {
            const char *title   = "WORD SCRAMBLE";
            int         title_w = (int)strlen(title) * 8 * 2;
            int         title_x = panel_x + (PANEL_WIDTH - title_w) / 2;
            int         title_y = panel_y + CHOICE_TITLE_INSIDE_Y;
            theme_draw_title(title, title_x, title_y, 2);
        }

        theme_draw_divider(panel_x + 20,
                           panel_y + CHOICE_DIVIDER_INSIDE_Y,
                           PANEL_WIDTH - 40);

        /* "Choose how to continue:" subtitle centred */
        {
            const char *subtitle   = "Choose how to continue:";
            int         subtitle_w = (int)strlen(subtitle) * 8;
            int         subtitle_x = panel_x + (PANEL_WIDTH - subtitle_w) / 2;
            int         subtitle_y = panel_y + CHOICE_SUBTITLE_INSIDE_Y;
            theme_draw_subtitle(subtitle, subtitle_x, subtitle_y);
        }

        /* Two buttons */
        int btn1_y = panel_y + CHOICE_BTN1_INSIDE_Y;
        int btn2_y = btn1_y + BTN_HEIGHT + CHOICE_BTN_GAP;

        theme_draw_button(btn_x, btn1_y,
                          BTN_WIDTH, BTN_HEIGHT,
                          COL_BTN_GREEN, "NEW PLAYER");

        theme_draw_button(btn_x, btn2_y,
                          BTN_WIDTH, BTN_HEIGHT,
                          COL_BTN_DEFAULT, "EXISTING PLAYER");
    }

    /* ── MODE_NEW ─────────────────────────────────────────── */
    else if (current_mode == MODE_NEW)
    {
        int panel_y = screen_height / 2 - NEW_PANEL_ABOVE_CENTRE;
        theme_draw_panel(panel_x, panel_y, PANEL_WIDTH, NEW_PANEL_HEIGHT);

        /* Title */
        {
            const char *title   = "ENTER NAME";
            int         title_w = (int)strlen(title) * 8 * 2;
            int         title_x = panel_x + (PANEL_WIDTH - title_w) / 2;
            int         title_y = panel_y + NEW_TITLE_INSIDE_Y;
            theme_draw_title(title, title_x, title_y, 2);
        }

        theme_draw_divider(panel_x + 20,
                           panel_y + NEW_DIVIDER_INSIDE_Y,
                           PANEL_WIDTH - 40);

        /* Text input box */
        int input_box_x = btn_x;
        int input_box_y = panel_y + NEW_INPUT_INSIDE_Y;
        int input_box_w = BTN_WIDTH;
        int input_box_h = NEW_INPUT_HEIGHT;

        /* Input box background — dark blue */
        gfx_color(0, 20, 60);
        gfx_fillrectangle(input_box_x, input_box_y,
                          input_box_w, input_box_h);

        /* Teal highlight line at the bottom */
        gfx_color(0, 175, 255);
        gfx_fillrectangle(input_box_x,
                          input_box_y + input_box_h - 3,
                          input_box_w, 3);

        /* Black border around input box */
        gfx_color(0, 0, 0);
        gfx_fillrectangle(input_box_x, input_box_y,
                          input_box_w, 2);
        gfx_fillrectangle(input_box_x,
                          input_box_y + input_box_h - 2,
                          input_box_w, 2);
        gfx_fillrectangle(input_box_x, input_box_y,
                          2, input_box_h);
        gfx_fillrectangle(input_box_x + input_box_w - 2, input_box_y,
                          2, input_box_h);

        /* Typed text + blinking cursor, centred in the box */
        {
            char display_text[36];
            sprintf(display_text, "%s_", name_input_text);
            int text_w = (int)strlen(display_text) * 8;
            int text_x = input_box_x + (input_box_w - text_w) / 2;
            int text_y = input_box_y + (input_box_h + 8) / 2 - 10;
            gfx_color(255, 255, 255);
            gfx_text(display_text, text_x, text_y, 1);
        }

        /* "CONFIRM & START" button below the input box */
        int confirm_btn_y = input_box_y + input_box_h + NEW_CONFIRM_BTN_GAP;
        theme_draw_button(btn_x, confirm_btn_y,
                          BTN_WIDTH, BTN_HEIGHT,
                          COL_BTN_GREEN, "CONFIRM & START");

        /* BACK button at top-left */
        theme_draw_button(BACK_BTN_X, BACK_BTN_Y,
                          BACK_BTN_WIDTH, BACK_BTN_HEIGHT,
                          COL_BTN_GREY, "BACK");
    }

    /* ── MODE_OLD ─────────────────────────────────────────── */
    else if (current_mode == MODE_OLD)
    {
        /* Panel height grows to fit the number of saved players */
        int visible_rows = saved_player_count > 0 ? saved_player_count : 1;
        int panel_height = OLD_HEADER_HEIGHT
                         + visible_rows * OLD_ROW_HEIGHT
                         + OLD_FOOTER_HEIGHT;

        int panel_y = screen_height / 2 - panel_height / 2;
        if (panel_y < OLD_MIN_PANEL_TOP)
            panel_y = OLD_MIN_PANEL_TOP;

        theme_draw_panel(panel_x, panel_y, PANEL_WIDTH, panel_height);

        /* Title */
        {
            const char *title   = "SELECT PROFILE";
            int         title_w = (int)strlen(title) * 8 * 2;
            int         title_x = panel_x + (PANEL_WIDTH - title_w) / 2;
            int         title_y = panel_y + NEW_TITLE_INSIDE_Y;
            theme_draw_title(title, title_x, title_y, 2);
        }

        theme_draw_divider(panel_x + 20,
                           panel_y + NEW_DIVIDER_INSIDE_Y,
                           PANEL_WIDTH - 40);

        /* Show message or list of player buttons */
        if (saved_player_count == 0) {
            const char *no_players_msg = "No existing players found.";
            int         msg_w          = (int)strlen(no_players_msg) * 8;
            int         msg_x          = panel_x + (PANEL_WIDTH - msg_w) / 2;
            theme_draw_label(no_players_msg, msg_x,
                             panel_y + OLD_FIRST_ROW_Y);
        } else {
            int i;
            for (i = 0; i < saved_player_count; i++) {
                int row_btn_y = panel_y + OLD_FIRST_ROW_Y
                              + i * OLD_ROW_HEIGHT;
                theme_draw_button(btn_x, row_btn_y,
                                  BTN_WIDTH, BTN_HEIGHT,
                                  COL_BTN_DEFAULT,
                                  saved_player_names[i]);
            }
        }

        /* BACK button at top-left */
        theme_draw_button(BACK_BTN_X, BACK_BTN_Y,
                          BACK_BTN_WIDTH, BACK_BTN_HEIGHT,
                          COL_BTN_GREY, "BACK");
    }
}

/* ================================================================
   SECTION 7  —  CLICK HANDLER
   ================================================================ */
GameState player_handle_click(int mouse_x, int mouse_y)
{
    int screen_width  = gfx_xsize();
    int screen_height = gfx_ysize();
    int panel_x       = (screen_width - PANEL_WIDTH) / 2;
    int btn_x         = panel_x + 20;

    /* ── MODE_CHOICE clicks ───────────────────────────────── */
    if (current_mode == MODE_CHOICE)
    {
        int panel_y = screen_height / 2 - CHOICE_PANEL_ABOVE_CENTRE;
        int btn1_y  = panel_y + CHOICE_BTN1_INSIDE_Y;
        int btn2_y  = btn1_y + BTN_HEIGHT + CHOICE_BTN_GAP;

        int click_in_column =
            mouse_x >= btn_x &&
            mouse_x <= btn_x + BTN_WIDTH;

        if (click_in_column) {
            /* "NEW PLAYER" clicked */
            if (mouse_y >= btn1_y && mouse_y < btn1_y + BTN_HEIGHT) {
                play_sound("menu_click.wav");
                name_input_length  = 0;
                name_input_text[0] = '\0';
                current_mode       = MODE_NEW;
            }
            /* "EXISTING PLAYER" clicked */
            else if (mouse_y >= btn2_y && mouse_y < btn2_y + BTN_HEIGHT) {
                play_sound("menu_click.wav");
                load_saved_player_names();
                current_mode = MODE_OLD;
            }
        }
    }

    /* ── MODE_NEW clicks ──────────────────────────────────── */
    else if (current_mode == MODE_NEW)
    {
        int panel_y         = screen_height / 2 - NEW_PANEL_ABOVE_CENTRE;
        int input_box_y     = panel_y + NEW_INPUT_INSIDE_Y;
        int confirm_btn_y   = input_box_y + NEW_INPUT_HEIGHT + NEW_CONFIRM_BTN_GAP;

        int click_in_column =
            mouse_x >= btn_x &&
            mouse_x <= btn_x + BTN_WIDTH;

        /* "CONFIRM & START" clicked */
        if (click_in_column &&
            mouse_y >= confirm_btn_y &&
            mouse_y <  confirm_btn_y + BTN_HEIGHT)
        {
            if (strlen(name_input_text) > 0) {
                play_sound("menu_click.wav");
                load_player_data(name_input_text);
                save_player_data();
                return STATE_MENU;
            }
        }

        /* "BACK" button clicked */
        if (mouse_x >= BACK_BTN_X &&
            mouse_x <= BACK_BTN_X + BACK_BTN_WIDTH &&
            mouse_y >= BACK_BTN_Y &&
            mouse_y <= BACK_BTN_Y + BACK_BTN_HEIGHT) {
            current_mode = MODE_CHOICE;
        }
    }

    /* ── MODE_OLD clicks ──────────────────────────────────── */
    else if (current_mode == MODE_OLD)
    {
        int visible_rows = saved_player_count > 0 ? saved_player_count : 1;
        int panel_height = OLD_HEADER_HEIGHT
                         + visible_rows * OLD_ROW_HEIGHT
                         + OLD_FOOTER_HEIGHT;
        int panel_y      = screen_height / 2 - panel_height / 2;
        if (panel_y < OLD_MIN_PANEL_TOP)
            panel_y = OLD_MIN_PANEL_TOP;

        /* Check each player name button */
        int i;
        for (i = 0; i < saved_player_count; i++) {
            int row_btn_y = panel_y + OLD_FIRST_ROW_Y
                          + i * OLD_ROW_HEIGHT;

            if (mouse_x >= btn_x             &&
                mouse_x <= btn_x + BTN_WIDTH &&
                mouse_y >= row_btn_y          &&
                mouse_y <  row_btn_y + BTN_HEIGHT)
            {
                load_player_data(saved_player_names[i]);
                return STATE_MENU;
            }
        }

        /* "BACK" button clicked */
        if (mouse_x >= BACK_BTN_X &&
            mouse_x <= BACK_BTN_X + BACK_BTN_WIDTH &&
            mouse_y >= BACK_BTN_Y &&
            mouse_y <= BACK_BTN_Y + BACK_BTN_HEIGHT) {
            current_mode = MODE_CHOICE;
        }
    }

    return STATE_SELECT_PLAYER;
}

/* ================================================================
   SECTION 8  —  KEYBOARD HANDLER
   Only active in MODE_NEW so the player can type their name.
   ================================================================ */
void player_handle_key(char key_pressed)
{
    if (current_mode != MODE_NEW) return;

    if (key_pressed == 8 || key_pressed == 127) {
        /* Backspace — delete last character */
        if (name_input_length > 0) {
            name_input_length--;
            name_input_text[name_input_length] = '\0';
        }
    }
    else if (name_input_length < 31 &&
             key_pressed >= 32 &&
             key_pressed <= 126)
    {
        /* Normal printable character — add to the name */
        name_input_text[name_input_length] = key_pressed;
        name_input_length++;
        name_input_text[name_input_length] = '\0';
    }
}
