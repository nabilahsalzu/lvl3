#ifndef RESULT_H
#define RESULT_H

#include "state.h"

void result_set_score(int final_score);
void draw_result();
void result_set_end_reason(int reason);

GameState result_handle_click(int mx, int my);

#endif