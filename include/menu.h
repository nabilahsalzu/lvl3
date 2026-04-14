#ifndef MENU_H
#define MENU_H

#include "state.h"

void draw_menu();
void draw_help();

GameState menu_handle_click(int mouse_x, int mouse_y);
GameState help_handle_click(int mouse_x, int mouse_y);
GameState game_update();

#endif
