#ifndef THEME_H
#define THEME_H

#include "gfx.h"

// ============================================================
//  WORD SCRAMBLE GAME – THEME  (Minion City Night)
// ============================================================

// ── Color macros ─────────────────────────────────────────────
#define COL_BG_MAIN          8,   12,  38
#define COL_BG_PANEL        12,   18,  48
#define COL_BG_CARD         10,   14,  38

#define COL_ACCENT_PRIMARY  255, 215,   0   // gold
#define COL_ACCENT_SECONDARY  0, 115, 195   // steel blue
#define COL_DANGER          160,  35,  35
#define COL_SUCCESS          35, 140,  60
#define COL_WARNING         235, 145,   0

#define COL_TEXT_PRIMARY    215, 228, 255
#define COL_TEXT_DIM        155, 195, 255
#define COL_TEXT_DARK         0,   0,   0

#define COL_BTN_DEFAULT       0,  80, 170
#define COL_BTN_GREEN        35, 140,  60
#define COL_BTN_RED         160,  35,  35
#define COL_BTN_GOLD        180, 130,   0
#define COL_BTN_GREY         65,  65,  95

#define COL_TILE_IDLE         7,  48, 132
#define COL_TILE_CLICKED     14, 100, 188

#define COL_BORDER            0,   0,   0

// ── Layout constants ─────────────────────────────────────────
#define THEME_BTN_W_WIDE   300
#define THEME_BTN_W_MED    200
#define THEME_BTN_W_SMALL  120
#define THEME_BTN_H         44
#define THEME_BTN_H_SMALL   34
#define THEME_MARGIN        20

#define MINION_PIXEL   5
#define HEART_PIXEL    4

// ── Function declarations ─────────────────────────────────────

void theme_draw_bg(int w, int h);
void theme_draw_header_bar(int w);

void theme_draw_panel(int x, int y, int pw, int ph);

void theme_draw_button(int x, int y, int bw, int bh,
                       int r, int g, int b, const char *label);
void theme_draw_button_outline(int x, int y, int bw, int bh,
                               int r, int g, int b, const char *label);

void theme_draw_title(const char *text, int x, int y, int scale);
void theme_draw_subtitle(const char *text, int x, int y);
void theme_draw_label(const char *text, int x, int y);
void theme_draw_divider(int x, int y, int len);

void theme_draw_lives(int lives, int max_lives, int cx, int y);
void theme_draw_score_badge(int score, int x, int y);
void theme_draw_tile(int x, int y, int size, char letter, int clicked);
void theme_draw_feedback(const char *msg, int is_correct, int w, int y);
void theme_draw_timer_bar(int time_left, int time_max,
                          int x, int y, int bar_w, int bar_h);
void theme_draw_score_row(int rank, int value, int is_best, int is_latest,
                          int x, int y, int row_w);

/* Single minion (original 16×16 sprite) */
void theme_draw_minion(int cx, int cy);

/* Two minions side-by-side (18×13 sprite from 2minions binary) */
void theme_draw_two_minions(int cx, int cy, int pixel_size);

void theme_draw_heart(int x, int y, int filled);

#endif /* THEME_H */
