#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "player.h"
#include "player_internal.h"
#include "gfx.h"
#include "theme.h"
#include "new_player.h"
#include "select_player.h"

typedef enum {
    MODE_CHOICE,
    MODE_NEW,
    MODE_OLD
} SelectionMode;

static SelectionMode current_mode = MODE_CHOICE;

Player current_player = { "", 0 };

static void ensure_players_folder_exists(void)
{
    struct stat folder_info = { 0 };
    if (stat("players", &folder_info) == -1)
        mkdir("players", 0700);
}

void save_player_data(void)
{
    ensure_players_folder_exists();

    char filepath[64];
    sprintf(filepath, "players/%s.txt", current_player.name);
    FILE *file = fopen(filepath, "a");
    if (file) {
        fprintf(file, "%d\n", current_player.high_score);
        fclose(file);
    }
}

void load_player_data(const char *player_name)
{
    ensure_players_folder_exists();

    strncpy(current_player.name, player_name, 31);
    current_player.name[31] = '\0';

    char filepath[64];
    sprintf(filepath, "players/%s.txt", player_name);
    FILE *file = fopen(filepath, "r");
    if (file) {
        if (fscanf(file, "%d", &current_player.high_score) != 1)
            current_player.high_score = 0;
        fclose(file);
    } else {
        current_player.high_score = 0;
    }
}

void draw_player_select(void)
{
    int screen_width  = gfx_xsize();
    int screen_height = gfx_ysize();

    theme_draw_bg(screen_width, screen_height);
    theme_draw_header_bar(screen_width);

    if (current_mode == MODE_CHOICE)
    {
        draw_player_choice();
    }
    else if (current_mode == MODE_NEW)
    {
        new_player_draw();
    }
    else if (current_mode == MODE_OLD)
    {
        draw_player_old();
    }
}

GameState player_handle_click(int mouse_x, int mouse_y)
{
    if (current_mode == MODE_CHOICE)
    {
        int action = select_player_choice_handle_click(mouse_x, mouse_y);
        if (action == 1)
        {
            new_player_reset();
            current_mode = MODE_NEW;
        }
        else if (action == 2)
        {
            select_player_load_saved_names();
            current_mode = MODE_OLD;
        }
    }
    else if (current_mode == MODE_NEW)
    {
        int action = new_player_handle_click(mouse_x, mouse_y);
        if (action == 1)
        {
            current_mode = MODE_CHOICE;
        }
        else if (action == 2)
        {
            current_mode = MODE_CHOICE;
            return STATE_MENU;
        }
    }
    else if (current_mode == MODE_OLD)
    {
        int action = select_player_handle_click(mouse_x, mouse_y);
        if (action == 1)
        {
            current_mode = MODE_CHOICE;
        }
        else if (action == 2)
        {
            current_mode = MODE_CHOICE;
            return STATE_MENU;
        }
    }

    return STATE_SELECT_PLAYER;
}

void player_handle_key(char key_pressed)
{
    if (current_mode == MODE_NEW)
        new_player_handle_key(key_pressed);
}
