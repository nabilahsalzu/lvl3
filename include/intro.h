#ifndef INTRO_H
#define INTRO_H

#include "game.h"

// Initializes the intro text based on the selected puzzle file
void intro_init(const char* filename);

// Draws the intro text and the "I'm Ready" button
void draw_intro();

// Handles clicking the "I'm Ready" button
GameState intro_handle_click(int mx, int my);

#endif