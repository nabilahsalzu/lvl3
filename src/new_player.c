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
#include "new_player.h"

static char name_input_text[32] = "";
static int  name_input_length   = 0;
static int  name_duplicate_error = 0;

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

static int is_name_taken(void)
{
    if (name_input_length == 0)
        return 0;

    ensure_players_folder_exists();

    DIR *folder = opendir("players");
    if (!folder)
        return 0;

    struct dirent *entry;
    while ((entry = readdir(folder)) != NULL)
    {
        char *dot_txt = strstr(entry->d_name, ".txt");
        if (!dot_txt)
            continue;

        int entry_name_len = (int)(dot_txt - entry->d_name);
        if (entry_name_len != name_input_length)
            continue;

        if (strncasecmp(entry->d_name, name_input_text, name_input_length) == 0)
        {
            closedir(folder);
            return 1;
        }
    }

    closedir(folder);
    return 0;
}

void new_player_reset(void)
{
    name_input_length = 0;
    name_input_text[0] = '\0';
    name_duplicate_error = 0;
}

void new_player_draw(void)
{
    int screen_width  = gfx_xsize();
    int screen_height = gfx_ysize();
    int panel_x       = (screen_width - PANEL_WIDTH) / 2;
    int btn_x         = panel_x + 20;
    int panel_y       = screen_height / 2 - NEW_PANEL_ABOVE_CENTRE;

    theme_draw_panel(panel_x, panel_y, PANEL_WIDTH, NEW_PANEL_HEIGHT);

    {
        const char *title   = "NAME YOUR MINION";
        int         title_w = (int)strlen(title) * 12;
        int         title_x = panel_x + (PANEL_WIDTH - title_w) / 2;
        int         title_y = panel_y + NEW_TITLE_INSIDE_Y;
        theme_draw_title(title, title_x, title_y, 2);
    }

    theme_draw_divider(panel_x + 20,
                       panel_y + NEW_DIVIDER_INSIDE_Y,
                       PANEL_WIDTH - 40);

    int input_box_x = btn_x;
    int input_box_y = panel_y + NEW_INPUT_INSIDE_Y;
    int input_box_w = BTN_WIDTH;
    int input_box_h = NEW_INPUT_HEIGHT;

    gfx_color(0, 20, 60);
    gfx_fillrectangle(input_box_x, input_box_y,
                      input_box_w, input_box_h);

    gfx_color(0, 175, 255);
    gfx_fillrectangle(input_box_x,
                      input_box_y + input_box_h - 3,
                      input_box_w, 3);

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

    {
        char display_text[36];
        sprintf(display_text, "%s_", name_input_text);
        int text_w = (int)strlen(display_text) * 8;
        int text_x = input_box_x + (input_box_w - text_w) / 2;
        int text_y = input_box_y + (input_box_h + 8) / 2 - 10;
        gfx_color(255, 255, 255);
        gfx_text(display_text, text_x, text_y, 1);
    }

    const char *helper_text = "Minion names must be unique and fun!";
    int helper_text_w = (int)strlen(helper_text) * 8;
    int helper_text_x = input_box_x + (input_box_w - helper_text_w) / 2;
    int helper_text_y = input_box_y + input_box_h + 28;
    gfx_color(175, 225, 255);
    gfx_text((char *)helper_text, helper_text_x, helper_text_y, 1);

    int confirm_btn_y = helper_text_y + 22 + NEW_CONFIRM_BTN_GAP;
    if (name_duplicate_error)
    {
        const char *error_text_line1 = "That Minion name already exists.";
        const char *error_text_line2 = "Try another one.";
        int error_text_line1_w = (int)strlen(error_text_line1) * 8;
        int error_text_line2_w = (int)strlen(error_text_line2) * 8;
        int error_text_x = input_box_x + (input_box_w - error_text_line1_w) / 2;
        int error_text2_x = input_box_x + (input_box_w - error_text_line2_w) / 2;
        int error_text_y = helper_text_y + 24;
        gfx_color(255, 120, 120);
        gfx_text((char *)error_text_line1, error_text_x, error_text_y, 1);
        gfx_text((char *)error_text_line2, error_text2_x, error_text_y + 18, 1);
        confirm_btn_y = error_text_y + 18 + 20 + NEW_CONFIRM_BTN_GAP;
    }

    theme_draw_button(btn_x, confirm_btn_y,
                      BTN_WIDTH, BTN_HEIGHT,
                      COL_BTN_GREEN, "CONFIRM & START");

    theme_draw_button(BACK_BTN_X, BACK_BTN_Y,
                      BACK_BTN_WIDTH, BACK_BTN_HEIGHT,
                      COL_BTN_GREY, "BACK");
}

int new_player_handle_click(int mouse_x, int mouse_y)
{
    int screen_width  = gfx_xsize();
    int screen_height = gfx_ysize();
    int panel_x       = (screen_width - PANEL_WIDTH) / 2;
    int btn_x         = panel_x + 20;
    int panel_y       = screen_height / 2 - NEW_PANEL_ABOVE_CENTRE;
    int input_box_y   = panel_y + NEW_INPUT_INSIDE_Y;

    int helper_text_y = input_box_y + NEW_INPUT_HEIGHT + 28;
    int confirm_btn_y = helper_text_y + 22 + NEW_CONFIRM_BTN_GAP;
    if (name_duplicate_error)
        confirm_btn_y += 42;

    int click_in_column =
        mouse_x >= btn_x &&
        mouse_x <= btn_x + BTN_WIDTH;

    if (click_in_column &&
        mouse_y >= confirm_btn_y &&
        mouse_y <  confirm_btn_y + BTN_HEIGHT)
    {
        if (name_input_length > 0) {
            if (is_name_taken()) {
                name_duplicate_error = 1;
                return 0;
            }
            play_sound("menu_click.wav");
            load_player_data(name_input_text);
            save_player_data();
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

void new_player_handle_key(char key_pressed)
{
    name_duplicate_error = 0;

    if (key_pressed == 8 || key_pressed == 127) {
        if (name_input_length > 0) {
            name_input_length--;
            name_input_text[name_input_length] = '\0';
        }
    }
    else if (name_input_length < 31 &&
             key_pressed >= 32 &&
             key_pressed <= 126)
    {
        name_input_text[name_input_length] = key_pressed;
        name_input_length++;
        name_input_text[name_input_length] = '\0';
    }
}
