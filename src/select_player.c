#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "player_internal.h"
#include "player.h"
#include "gfx.h"
#include "theme.h"
#include "select_player.h"

static char saved_player_names[MAX_SAVED_PLAYERS][32];
static int  saved_player_count = 0;

static void play_sound(const char *sound_filename)
{
    char command[128];
    snprintf(command, sizeof(command), "aplay -q sounds/%s &", sound_filename);
    system(command);
}

static void ensure_players_folder_exists(void)
{
    struct stat folder_info = { 0 };
    if (stat("players", &folder_info) == -1)
        mkdir("players", 0700);
}

void select_player_load_saved_names(void)
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

void draw_player_choice(void)
{
    int screen_width  = gfx_xsize();
    int screen_height = gfx_ysize();
    int panel_x       = (screen_width - PANEL_WIDTH) / 2;
    int btn_x         = panel_x + 20;
    int panel_y       = (screen_height - CHOICE_PANEL_HEIGHT) / 2;

    int road_y = screen_height * 72 / 100;
    theme_draw_minion(MINION_LEFT_X,
                      road_y - MINION_ABOVE_ROAD);
    theme_draw_minion(screen_width - MINION_RIGHT_OFFSET,
                      road_y - MINION_ABOVE_ROAD);

    theme_draw_panel(panel_x, panel_y, PANEL_WIDTH, CHOICE_PANEL_HEIGHT);

    {
        const char *title1   = "BELLO!";
        int         title1_w = (int)strlen(title1) * 12;
        int         title1_x = panel_x + (PANEL_WIDTH - title1_w) / 2;
        int         title1_y = panel_y + CHOICE_TITLE_INSIDE_Y;
        theme_draw_title(title1, title1_x, title1_y, 3);
    }

    {
        const char *title2   = "Scramble Blast";
        int         title2_w = (int)strlen(title2) * 10;
        int         title2_x = panel_x + (PANEL_WIDTH - title2_w) / 2;
        int         title2_y = panel_y + CHOICE_TITLE2_INSIDE_Y;
        theme_draw_title(title2, title2_x, title2_y, 2);
    }

    {
        const char *friendly = "A silly word puzzle for smart Minions!";
        int         friendly_w = (int)strlen(friendly) * 9;
        if (friendly_w > PANEL_WIDTH - 40)
            friendly_w = PANEL_WIDTH - 40;
        int         friendly_x = panel_x + (PANEL_WIDTH - friendly_w) / 2;
        int         friendly_y = panel_y + CHOICE_FRIENDLY_INSIDE_Y;
        theme_draw_label(friendly, friendly_x, friendly_y);
    }

    theme_draw_divider(panel_x + 20,
                       panel_y + CHOICE_DIVIDER_INSIDE_Y,
                       PANEL_WIDTH - 40);

    {
        const char *subtitle   = "Let's get started!";
        int         subtitle_w = (int)strlen(subtitle) * 9;
        int         subtitle_x = panel_x + (PANEL_WIDTH - subtitle_w) / 2;
        int         subtitle_y = panel_y + CHOICE_SUBTITLE_INSIDE_Y;
        theme_draw_subtitle(subtitle, subtitle_x, subtitle_y);
    }

    int btn1_y = panel_y + CHOICE_BTN1_INSIDE_Y;
    int btn2_y = btn1_y + BTN_HEIGHT + CHOICE_BTN_GAP;
    int btn3_y = btn2_y + BTN_HEIGHT + CHOICE_BTN_GAP;

    theme_draw_button(btn_x, btn1_y,
                      BTN_WIDTH, BTN_HEIGHT,
                      COL_BTN_GREEN, "CREATE MINION");

    theme_draw_button(btn_x, btn2_y,
                      BTN_WIDTH, BTN_HEIGHT,
                      COL_BTN_DEFAULT, "SELECT MINION");

    theme_draw_button(btn_x, btn3_y,
                      BTN_WIDTH, BTN_HEIGHT,
                      COL_BTN_RED, "EXIT");
}

void draw_player_old(void)
{
    int screen_width  = gfx_xsize();
    int screen_height = gfx_ysize();
    int panel_x       = (screen_width - PANEL_WIDTH) / 2;
    int btn_x         = panel_x + 20;

    int visible_rows = saved_player_count > 0 ? saved_player_count : 1;
    int panel_height = OLD_HEADER_HEIGHT
                     + visible_rows * OLD_ROW_HEIGHT
                     + OLD_FOOTER_HEIGHT;

    int panel_y = screen_height / 2 - panel_height / 2;
    if (panel_y < OLD_MIN_PANEL_TOP)
        panel_y = OLD_MIN_PANEL_TOP;

    theme_draw_panel(panel_x, panel_y, PANEL_WIDTH, panel_height);

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

    if (saved_player_count == 0) {
        const char *no_players_msg = "No existing players found.";
        int         msg_w          = (int)strlen(no_players_msg) * 8;
        int         msg_x          = panel_x + (PANEL_WIDTH - msg_w) / 2;
        theme_draw_label(no_players_msg, msg_x,
                         panel_y + OLD_FIRST_ROW_Y);
    } else {
        for (int i = 0; i < saved_player_count; i++) {
            int row_btn_y = panel_y + OLD_FIRST_ROW_Y
                          + i * OLD_ROW_HEIGHT;
            theme_draw_button(btn_x, row_btn_y,
                              BTN_WIDTH, BTN_HEIGHT,
                              COL_BTN_DEFAULT,
                              saved_player_names[i]);
        }
    }

    theme_draw_button(BACK_BTN_X, BACK_BTN_Y,
                      BACK_BTN_WIDTH, BACK_BTN_HEIGHT,
                      COL_BTN_GREY, "BACK");
}

int select_player_choice_handle_click(int mouse_x, int mouse_y)
{
    int screen_width  = gfx_xsize();
    int screen_height = gfx_ysize();
    int panel_x       = (screen_width - PANEL_WIDTH) / 2;
    int btn_x         = panel_x + 20;
    int panel_y       = (screen_height - CHOICE_PANEL_HEIGHT) / 2;
    int btn1_y        = panel_y + CHOICE_BTN1_INSIDE_Y;
    int btn2_y        = btn1_y + BTN_HEIGHT + CHOICE_BTN_GAP;
    int btn3_y        = btn2_y + BTN_HEIGHT + CHOICE_BTN_GAP;

    int click_in_column =
        mouse_x >= btn_x &&
        mouse_x <= btn_x + BTN_WIDTH;

    if (click_in_column) {
        if (mouse_y >= btn1_y && mouse_y < btn1_y + BTN_HEIGHT) {
            play_sound("menu_click.wav");
            return 1;
        }
        else if (mouse_y >= btn2_y && mouse_y < btn2_y + BTN_HEIGHT) {
            play_sound("menu_click.wav");
            return 2;
        }
        else if (mouse_y >= btn3_y && mouse_y < btn3_y + BTN_HEIGHT) {
            play_sound("menu_click.wav");
            return 3;
        }
    }
    return 0;
}

int select_player_handle_click(int mouse_x, int mouse_y)
{
    int screen_width  = gfx_xsize();
    int screen_height = gfx_ysize();
    int visible_rows  = saved_player_count > 0 ? saved_player_count : 1;
    int panel_height  = OLD_HEADER_HEIGHT
                      + visible_rows * OLD_ROW_HEIGHT
                      + OLD_FOOTER_HEIGHT;
    int panel_y       = screen_height / 2 - panel_height / 2;

    if (panel_y < OLD_MIN_PANEL_TOP)
        panel_y = OLD_MIN_PANEL_TOP;

    int btn_x = (screen_width - PANEL_WIDTH) / 2 + 20;

    for (int i = 0; i < saved_player_count; i++) {
        int row_btn_y = panel_y + OLD_FIRST_ROW_Y
                      + i * OLD_ROW_HEIGHT;

        if (mouse_x >= btn_x &&
            mouse_x <= btn_x + BTN_WIDTH &&
            mouse_y >= row_btn_y &&
            mouse_y <  row_btn_y + BTN_HEIGHT)
        {
            play_sound("menu_click.wav");
            load_player_data(saved_player_names[i]);
            return 2;
        }
    }

    if (mouse_x >= BACK_BTN_X &&
        mouse_x <= BACK_BTN_X + BACK_BTN_WIDTH &&
        mouse_y >= BACK_BTN_Y &&
        mouse_y <= BACK_BTN_Y + BACK_BTN_HEIGHT)
    {
        return 1;
    }

    return 0;
}
