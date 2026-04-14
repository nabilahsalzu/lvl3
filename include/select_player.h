#ifndef SELECT_PLAYER_H
#define SELECT_PLAYER_H

#include "state.h"

void select_player_load_saved_names(void);
void draw_player_choice(void);
void draw_player_old(void);
int select_player_choice_handle_click(int mouse_x, int mouse_y);
int select_player_handle_click(int mouse_x, int mouse_y);

#endif
