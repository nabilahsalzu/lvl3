#ifndef NEW_PLAYER_H
#define NEW_PLAYER_H

#include "state.h"

int new_player_handle_click(int mouse_x, int mouse_y);
void new_player_handle_key(char key_pressed);
void new_player_draw(void);
void new_player_reset(void);

#endif
