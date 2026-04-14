#ifndef PLAYER_H
#define PLAYER_H

#include "state.h"

typedef struct {
char name[32];
int high_score;
} Player;

extern Player current_player;

void draw_player_select();
void player_handle_key(char c);
void save_player_data();
void load_player_data(const char* name);

GameState player_handle_click(int mx, int my);


#endif