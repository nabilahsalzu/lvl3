#ifndef GAME_H
#define GAME_H

#include "state.h"   /* GameState enum lives here */

/* init */
void      game_init(void);
void      game_init_with_file(const char *filename);
void      save_final_score(void);
void      load_words_from_file(const char *filename);

/* per-frame */
GameState game_handle_input(char c);
GameState game_update(void);

/* drawing — three levels of repaint for smooth rendering */
void      draw_game(void);          /* full redraw       */
void      draw_timer_only(void);    /* timer bar only    */
void      draw_tiles_only(void);    /* tile rows only    */

/* state variables accessible by other modules */
extern char current_category[50];
extern int  score;
extern int  lives;

#endif /* GAME_H */
