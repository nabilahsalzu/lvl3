#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "player_internal.h"
#include "player.h"
#include "gfx.h"
#include "theme.h"
#include "new_player.h"

static char name_input_text[32] = "";
static int  name_input_length   = 0;

static void play_sound(const char *sound_filename)
{
    char command[128];
    snprintf(command, sizeof(command), "aplay -q sounds/%s &", sound_filename);
    system(command);
}

void new_player_reset(void)
{
    name_input_length = 0;
    name_input_text[0] = '\0';
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
        const char *title   = "ENTER NAME";
        int         title_w = (int)strlen(title) * 8 * 2;
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

    int confirm_btn_y = input_box_y + input_box_h + NEW_CONFIRM_BTN_GAP;
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
    int confirm_btn_y = input_box_y + NEW_INPUT_HEIGHT + NEW_CONFIRM_BTN_GAP;

    int click_in_column =
        mouse_x >= btn_x &&
        mouse_x <= btn_x + BTN_WIDTH;

    if (click_in_column &&
        mouse_y >= confirm_btn_y &&
        mouse_y <  confirm_btn_y + BTN_HEIGHT)
    {
        if (name_input_length > 0) {
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
