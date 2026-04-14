/* ================================================================
   game.c  —  Word Scramble Gameplay
   ================================================================ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "game.h"
#include "gfx.h"
#include "theme.h"
#include "player.h"
#include "result.h"

/* ================================================================
   SECTION 1 — WORD AND TILE LIMITS
   ================================================================ */
#define MAX_WORDS        70
#define MAX_WORD_LENGTH  20
#define MAX_TILES        20
#define TILE_SIZE_BIG    50   /* tile px — short words               */
#define TILE_SIZE_SMALL  26   /* tile px — long words                */
#define TILE_GAP          8   /* px gap between tiles                */
#define TIME_PER_WORD    10   /* seconds per word                    */
#define TOTAL_LIVES       3   /* hearts at start                     */

/* ================================================================
   SECTION 2 — EXACT SCREEN POSITIONS  (pixels from top-left)
   Edit any number here to move that element on screen.
   ================================================================ */

/* HUD bar — player name and category */
#define HUD_Y              8
#define HUD_HEIGHT        36
#define HUD_LEFT_PAD      24

/* Lives and score row */
#define LIVES_ROW_Y       58
#define LIVES_LABEL_X     20
#define HEARTS_X          80   /* X of the first heart               */
#define HEARTS_SPACING    55   /* px between hearts                  */
#define SCORE_RIGHT_PAD   70   /* px from right edge                 */

/* Title */
#define TITLE_Y          100

/* Timer bar */
#define TIMER_Y          152
#define TIMER_W          360
#define TIMER_H           12

/* Scrambled letters */
#define SCRAMBLE_LABEL_Y 182
#define SCRAMBLE_ROW_Y   202

/* Arrow and answer row gaps */
#define ARROW_GAP          8   /* below scramble tiles               */
#define ANS_LABEL_GAP     28   /* below scramble tiles               */
#define ANS_SLOT_GAP      18   /* below answer label                 */

/* Buttons */
#define BTN_GAP_ABOVE     24   /* above buttons from answer row      */
#define BTN_W            140
#define BTN_H             38
#define BTN_SPACING       16   /* gap between buttons                */

/* Feedback banner */
#define FEEDBACK_GAP      22   /* above feedback from buttons        */

/* Minion decorations */
#define MINION_LEFT_X     44
#define MINION_RIGHT_PAD  44
#define MINION_Y_PCT      64   /* screen_h * 64 / 100                */

/* ================================================================
   SECTION 3 — GAME STATE
   ================================================================ */
static char  word_pool[MAX_WORDS][MAX_WORD_LENGTH];
static int   pool_size     = 0;
static int   word_idx      = 0;   /* which word we are playing now   */

char  current_word[MAX_WORD_LENGTH];
char  scrambled[MAX_WORD_LENGTH];
char  current_category[50] = "WORD SCRAMBLE";

int   score           = 0;
int   lives           = TOTAL_LIVES;
int   has_saved_score = 0;

time_t start_time;
int    time_limit = TIME_PER_WORD;

char   feedback_msg[100] = "";
time_t feedback_expiry   = 0;
int    showing_feedback  = 0;

static int tile_used[MAX_TILES];
static int answer_slots[MAX_TILES];
static int slots_filled = 0;
static int tile_count   = 0;
static int tile_size    = TILE_SIZE_BIG;

static int tile_x[MAX_TILES], tile_y[MAX_TILES];
static int slot_x[MAX_TILES], slot_y[MAX_TILES];

static int hint_count = 0;

typedef struct { int x, y, w, h; } Button;
static Button quit_btn, clear_btn, hint_btn;

/* ================================================================
   SECTION 4 — SOUND
   ================================================================ */
static void play_sound(const char *f)
{
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "aplay -q sounds/%s &", f);
    system(cmd);
}

void play_bg_music(void)
{
    system("pkill -f 'aplay.*bg_music' 2>/dev/null");
    system("(while true; do aplay -q sounds/bg_music.wav 2>/dev/null; done) &");
}

void stop_bg_music(void)
{
    system("pkill -f 'aplay.*bg_music' 2>/dev/null");
}

/* ================================================================
   SECTION 5 — TILE DRAWING
   ================================================================ */
static void draw_tile(int x, int y, int sz, char letter, int is_answer)
{
    /* shadow */
    gfx_color(0, 0, 0);
    gfx_fillrectangle(x + 3, y + 3, sz, sz);

    /* body */
    if (is_answer) gfx_color(18,  90,  32);
    else           gfx_color( 8,  52, 138);
    gfx_fillrectangle(x, y, sz, sz);

    /* top-left bevel */
    if (is_answer) gfx_color(50, 155, 68);
    else           gfx_color(28,  98, 188);
    gfx_fillrectangle(x + 2, y + 2, sz - 4, 3);
    gfx_fillrectangle(x + 2, y + 5, 3, sz - 7);

    /* bottom-right bevel */
    if (is_answer) gfx_color(6, 42, 14);
    else           gfx_color(2, 18, 62);
    gfx_fillrectangle(x + 2,      y + sz - 4, sz - 4, 3);
    gfx_fillrectangle(x + sz - 4, y + 2,      3, sz - 4);

    /* border */
    gfx_color(0, 0, 0);
    gfx_fillrectangle(x,       y,        sz, 2);
    gfx_fillrectangle(x,       y + sz-2, sz, 2);
    gfx_fillrectangle(x,       y,        2, sz);
    gfx_fillrectangle(x + sz-2,y,        2, sz);

    /* letter centred */
    char s[2] = { letter, '\0' };
    int lx = x + (sz - 8) / 2;
    int ly = y + (sz + 8) / 2 - 8;
    gfx_color(0, 0, 0);   gfx_text(s, lx+1, ly+1, 1);
    gfx_color(255, 232, 0); gfx_text(s, lx,   ly,   1);
}

static void draw_slot_empty(int x, int y, int sz)
{
    gfx_color(12, 14, 35); gfx_fillrectangle(x, y, sz, sz);
    gfx_color(6,   6, 18);
    gfx_fillrectangle(x, y, sz, 2);
    gfx_fillrectangle(x, y, 2, sz);
    gfx_color(45, 50, 90);
    gfx_fillrectangle(x,       y+sz-2, sz, 2);
    gfx_fillrectangle(x + sz-2,y,       2, sz);
    gfx_color(0, 0, 0);
    gfx_fillrectangle(x-1, y-1,   sz+2, 2);
    gfx_fillrectangle(x-1, y+sz-1,sz+2, 2);
    gfx_fillrectangle(x-1, y-1,   2, sz+2);
    gfx_fillrectangle(x+sz-1,y-1, 2, sz+2);
}

static void draw_slot_ghost(int x, int y, int sz)
{
    gfx_color(10, 11, 26); gfx_fillrectangle(x, y, sz, sz);
    gfx_color(22, 24, 44);
    gfx_fillrectangle(x,       y+sz-2, sz, 2);
    gfx_fillrectangle(x + sz-2,y,       2, sz);
    gfx_color(0, 0, 0);
    gfx_fillrectangle(x-1, y-1,   sz+2, 2);
    gfx_fillrectangle(x-1, y+sz-1,sz+2, 2);
    gfx_fillrectangle(x-1, y-1,   2, sz+2);
    gfx_fillrectangle(x+sz-1,y-1, 2, sz+2);
}

static void draw_down_arrow(int cx, int top_y)
{
    gfx_color(90, 100, 150);
    gfx_fillrectangle(cx-1, top_y,     2, 12);
    gfx_fillrectangle(cx-5, top_y+8,  10,  2);
    gfx_fillrectangle(cx-3, top_y+10,  6,  2);
    gfx_fillrectangle(cx-1, top_y+12,  2,  2);
}

/* ================================================================
   SECTION 6 — WORD LOADING AND SCRAMBLING
   ================================================================ */
void load_words_from_file(const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (!f) { printf("Cannot open %s\n", filename); return; }

    pool_size = 0;
    while (pool_size < MAX_WORDS &&
           fscanf(f, "%19s", word_pool[pool_size]) == 1)
        pool_size++;
    fclose(f);

    /* shuffle word order */
    int i;
    for (i = pool_size - 1; i > 0; i--) {
        int  j = rand() % (i + 1);
        char t[MAX_WORD_LENGTH];
        strcpy(t, word_pool[i]);
        strcpy(word_pool[i], word_pool[j]);
        strcpy(word_pool[j], t);
    }
    
    if (pool_size > 20) {
        pool_size = 20;
    }
}

static void scramble_word(char *word)
{
    int len = (int)strlen(word);
    if (len <= 1) return;
    char orig[MAX_WORD_LENGTH];
    strcpy(orig, word);
    int tries = 0;
    do {
        int i;
        for (i = len - 1; i > 0; i--) {
            int  j = rand() % (i + 1);
            char t = word[i]; word[i] = word[j]; word[j] = t;
        }
        tries++;
    } while (strcmp(word, orig) == 0 && tries < 20);
}

/* ================================================================
   SECTION 7 — ANSWER CHECK
   ================================================================ */
static void check_answer(void)
{
    char answer[MAX_WORD_LENGTH + 1];
    int  i;
    for (i = 0; i < tile_count; i++)
        answer[i] = scrambled[answer_slots[i]];
    answer[tile_count] = '\0';

    if (strcmp(answer, current_word) == 0) {
        score += 10;
        sprintf(feedback_msg, "CORRECT!  +10 Points");
        play_sound("correct.wav");
        showing_feedback = 1;
        feedback_expiry  = time(NULL) + 3;
    } else {
        lives--;
        sprintf(feedback_msg, "WRONG!  Answer: %s", current_word);
        play_sound("wrong.wav");
        showing_feedback = 1;
        feedback_expiry  = time(NULL) + 3;
    }
    
    draw_game(); /* Force full repaint to show feedback banner immediately */
}

/* ================================================================
   SECTION 8 — HINT
   ================================================================ */
static void do_hint(void)
{
    if (hint_count >= 1) return; /* HINT button can be used once only per word */

    int  target_slot    = hint_count;
    char needed_letter  = current_word[hint_count];
    int  i;

    for (i = 0; i < tile_count; i++) {
        if (!tile_used[i] && scrambled[i] == needed_letter) {
            /* clear slot if occupied */
            if (answer_slots[target_slot] != -1) {
                tile_used[answer_slots[target_slot]] = 0;
                slots_filled--;
            }
            tile_used[i]              = 1;
            answer_slots[target_slot] = i;
            slots_filled++;
            hint_count++;
            play_sound("hint.wav");
            if (slots_filled == tile_count) check_answer();
            return;
        }
    }
    hint_count++;
}

/* ================================================================
   SECTION 9 — LOAD NEXT WORD
   ================================================================ */
static void load_next_word(void)
{
    if (word_idx >= pool_size) return;

    strcpy(current_word, word_pool[word_idx]);
    strcpy(scrambled, current_word);
    scramble_word(scrambled);

    tile_count       = (int)strlen(scrambled);
    slots_filled     = 0;
    hint_count       = 0;
    showing_feedback = 0;

    int i;
    for (i = 0; i < tile_count; i++) {
        tile_used[i]    = 0;
        answer_slots[i] = -1;
    }
    start_time = time(NULL);
    word_idx++;
}

/* ================================================================
   SECTION 10 — GAME INIT
   ================================================================ */
void game_init_with_file(const char *filename)
{
    srand((unsigned)time(NULL));
    load_words_from_file(filename);

    if      (strstr(filename, "puzzle1.txt")) strcpy(current_category, "GRU'S HOME & LAB");
    else if (strstr(filename, "puzzle2.txt")) strcpy(current_category, "MINION WORLD TOUR");
    else if (strstr(filename, "puzzle3.txt")) strcpy(current_category, "DR. NEFARIO'S TECH");
    else                                      strcpy(current_category, "BANANA SURPRISE");

    score            = 0;
    lives            = TOTAL_LIVES;
    word_idx         = 0;
    showing_feedback = 0;
    has_saved_score  = 0;

    load_next_word();
    play_sound("start.wav");
    play_bg_music();
}

void game_init(void) { game_init_with_file("data/puzzle1.txt"); }

/* ================================================================
   SECTION 11 — SAVE SCORE
   ================================================================ */
void save_final_score(void)
{
    if (has_saved_score) return;
    has_saved_score = 1;
    char path[64];
    sprintf(path, "players/%s.txt", current_player.name);
    FILE *f = fopen(path, "a");
    if (!f) return;
    fprintf(f, "%d\n", score);
    fclose(f);
}

/* ================================================================
   SECTION 12 — DRAW GAME  (full repaint)

   Layout (Y positions defined at top of file):
     HUD_Y=8        Player name + category bar
     LIVES_ROW_Y=58 Hearts + score
     TITLE_Y=100    "WORD SCRAMBLE"
     TIMER_Y=152    Countdown bar
     SCRAMBLE_LABEL_Y=182  "Scrambled:" label
     SCRAMBLE_ROW_Y=202    Scrambled tiles
     + ANS_LABEL_GAP        "Your Answer:" label
     + ANS_SLOT_GAP         Answer slots
     + BTN_GAP_ABOVE        CLEAR / HINT / MENU buttons
     + FEEDBACK_GAP         Feedback banner
   ================================================================ */
void draw_game(void)
{
    int sw = gfx_xsize();   /* screen width  */
    int sh = gfx_ysize();   /* screen height */

    /* background + gold top stripe */
    theme_draw_bg(sw, sh);
    theme_draw_header_bar(sw);

    /* minions left and right */
    int minion_y = sh * MINION_Y_PCT / 100;
    theme_draw_two_minions(MINION_LEFT_X,          minion_y, MINION_PIXEL);
    theme_draw_two_minions(sw - MINION_RIGHT_PAD,  minion_y, MINION_PIXEL);

    /* compute tile size — use 65% of screen width for tile rows */
    int tile_area = sw * 65 / 100;
    tile_size = (tile_area - (tile_count - 1) * TILE_GAP) / tile_count;
    if (tile_size > TILE_SIZE_BIG)   tile_size = TILE_SIZE_BIG;
    if (tile_size < TILE_SIZE_SMALL) tile_size = TILE_SIZE_SMALL;

    /* centre tile rows horizontally */
    int row_w = tile_count * (tile_size + TILE_GAP) - TILE_GAP;
    int row_x = (sw - row_w) / 2;

    /* ── HUD bar ───────────────────────────────────────── */
    theme_draw_panel(10, HUD_Y, sw - 20, HUD_HEIGHT);

    char pname[48];
    sprintf(pname, "Player: %s", current_player.name);
    int hud_ty = HUD_Y + (HUD_HEIGHT + 8) / 2 - 8;
    gfx_color(0,   0,   0);   gfx_text(pname, HUD_LEFT_PAD+1, hud_ty+1, 1);
    gfx_color(155, 195, 255); gfx_text(pname, HUD_LEFT_PAD,   hud_ty,   1);

    char cat[64];
    sprintf(cat, "[ %s ]", current_category);
    int cat_w = (int)strlen(cat) * 8;
    int cat_x = sw / 2 - cat_w / 2;
    gfx_color(0,   0,   0);   gfx_text(cat, cat_x+1, hud_ty+1, 1);
    gfx_color(215, 228, 255); gfx_text(cat, cat_x,   hud_ty,   1);

    char qnum[32];
    sprintf(qnum, "%d/%d", word_idx, pool_size);
    int qnum_w = (int)strlen(qnum) * 8;
    int qnum_x = sw - HUD_LEFT_PAD - qnum_w - 10;
    gfx_color(0,   0,   0);   gfx_text(qnum, qnum_x+1, hud_ty+1, 1);
    gfx_color(155, 195, 255); gfx_text(qnum, qnum_x,   hud_ty,   1);

    /* ── Hearts + score ────────────────────────────────── */
    int lives_ty = LIVES_ROW_Y + 4;
    gfx_color(0,   0,   0);   gfx_text("LIFE:", LIVES_LABEL_X+1, lives_ty+1, 1);
    gfx_color(200, 200, 220); gfx_text("LIFE:", LIVES_LABEL_X,   lives_ty,   1);

    int i;
    for (i = 0; i < TOTAL_LIVES; i++){
        theme_draw_heart(HEARTS_X + i * HEARTS_SPACING, LIVES_ROW_Y - 5,i < lives ? 1 : 0);
    	}

    char sbuf[24];
    sprintf(sbuf, "SCORE: %d", score);
    int sw2 = (int)strlen(sbuf) * 8;
    int sx  = sw - sw2 - SCORE_RIGHT_PAD;
    gfx_color(0,   0,   0);   gfx_text(sbuf, sx+1, LIVES_ROW_Y+1, 1);
    gfx_color(255, 215, 0);   gfx_text(sbuf, sx,   LIVES_ROW_Y,   1);

    /* ── Title ─────────────────────────────────────────── */
    {
        const char *t  = "WORD SCRAMBLE";
        int         tw = (int)strlen(t) * 16;
        int         tx = (sw - tw) / 2 + 40;
        theme_draw_title(t, tx, TITLE_Y, 2);
    }

    /* ── Timer ─────────────────────────────────────────── */
    int timer_x   = (sw - TIMER_W) / 2;
    int time_left = time_limit - (int)(time(NULL) - start_time);
    if (time_left < 0) time_left = 0;
    theme_draw_timer_bar(time_left, time_limit, timer_x, TIMER_Y, TIMER_W, TIMER_H);

    char tl[16]; sprintf(tl, "%ds", time_left);
    theme_draw_subtitle(tl, timer_x + TIMER_W + 8, TIMER_Y + (TIMER_H+8)/2 - 8);

    /* ── "Scrambled:" label ─────────────────────────────── */
    {
        const char *lbl = "Scrambled:";
        int         lw  = (int)strlen(lbl) * 8;
        theme_draw_subtitle(lbl, sw/2 - lw/2, SCRAMBLE_LABEL_Y);
    }

    /* ── Scrambled tiles ────────────────────────────────── */
    for (i = 0; i < tile_count; i++) {
        tile_x[i] = row_x + i * (tile_size + TILE_GAP);
        tile_y[i] = SCRAMBLE_ROW_Y;
        if (!tile_used[i])
            draw_tile(tile_x[i], tile_y[i], tile_size, scrambled[i], 0);
        else
            draw_slot_ghost(tile_x[i], tile_y[i], tile_size);
    }

    /* ── Arrow ──────────────────────────────────────────── */
    draw_down_arrow(sw / 2, SCRAMBLE_ROW_Y + tile_size + ARROW_GAP);

    /* ── "Your Answer:" label ───────────────────────────── */
    int ans_label_y = SCRAMBLE_ROW_Y + tile_size + ANS_LABEL_GAP;
    {
        const char *lbl = "Your Answer:";
        int         lw  = (int)strlen(lbl) * 8;
        theme_draw_subtitle(lbl, sw/2 - lw/2, ans_label_y);
    }

    /* ── Answer slots ───────────────────────────────────── */
    int ans_row_y = ans_label_y + ANS_SLOT_GAP;
    for (i = 0; i < tile_count; i++) {
        slot_x[i] = row_x + i * (tile_size + TILE_GAP);
        slot_y[i] = ans_row_y;
        int ti = answer_slots[i];
        if (ti == -1) draw_slot_empty(slot_x[i], slot_y[i], tile_size);
        else          draw_tile(slot_x[i], slot_y[i], tile_size, scrambled[ti], 1);
    }

    /* ── Buttons ────────────────────────────────────────── */
    int btn_y    = ans_row_y + tile_size + BTN_GAP_ABOVE;
    int btns_w   = 3 * BTN_W + 2 * BTN_SPACING;
    int btn_sx   = (sw - btns_w) / 2;

    int clear_x  = btn_sx;
    int hint_x   = btn_sx + BTN_W + BTN_SPACING;
    int menu_x   = btn_sx + 2 * (BTN_W + BTN_SPACING);

    clear_btn.x = clear_x; clear_btn.y = btn_y; clear_btn.w = BTN_W; clear_btn.h = BTN_H;
    hint_btn.x  = hint_x;  hint_btn.y  = btn_y; hint_btn.w  = BTN_W; hint_btn.h  = BTN_H;
    quit_btn.x  = menu_x;  quit_btn.y  = btn_y; quit_btn.w  = BTN_W; quit_btn.h  = BTN_H;

    theme_draw_button(clear_x, btn_y, BTN_W, BTN_H, COL_BTN_RED,     "CLEAR");
    theme_draw_button(hint_x,  btn_y, BTN_W, BTN_H, COL_BTN_DEFAULT, "HINT");
    theme_draw_button(menu_x,  btn_y, BTN_W, BTN_H, 80, 60, 150,     "QUIT");

    /* ── Feedback ───────────────────────────────────────── */
    if (showing_feedback) {
        int ok = strstr(feedback_msg, "CORRECT") != NULL;
        theme_draw_feedback(feedback_msg, ok, sw, btn_y + BTN_H + FEEDBACK_GAP);
    }
}

/* ================================================================
   SECTION 13 — DRAW TIMER ONLY  (partial repaint, no full clear)
   ================================================================ */
void draw_timer_only(void)
{
    int sw = gfx_xsize();
    int sh = gfx_ysize();

    /* repaint background strip behind timer */
    int y;
    for (y = TIMER_Y - 2; y < TIMER_Y + TIMER_H + 18; y++) {
        int g = y * 100 / sh;
        gfx_color(8 + g*20/100, 12 + g*32/100, 38 + g*38/100);
        gfx_fillrectangle(0, y, sw, 1);
    }

    int timer_x   = (sw - TIMER_W) / 2;
    int time_left = time_limit - (int)(time(NULL) - start_time);
    if (time_left < 0) time_left = 0;

    theme_draw_timer_bar(time_left, time_limit, timer_x, TIMER_Y, TIMER_W, TIMER_H);

    char tl[16]; sprintf(tl, "%ds", time_left);
    theme_draw_subtitle(tl, timer_x + TIMER_W + 8, TIMER_Y + (TIMER_H+8)/2 - 8);
}

/* ================================================================
   SECTION 14 — DRAW TILES ONLY  (partial repaint after click)
   ================================================================ */
void draw_tiles_only(void)
{
    int sw = gfx_xsize();
    int sh = gfx_ysize();

    /* recompute tile size */
    int tile_area = sw * 65 / 100;
    int ts = (tile_area - (tile_count - 1) * TILE_GAP) / tile_count;
    if (ts > TILE_SIZE_BIG)   ts = TILE_SIZE_BIG;
    if (ts < TILE_SIZE_SMALL) ts = TILE_SIZE_SMALL;

    int row_w     = tile_count * (ts + TILE_GAP) - TILE_GAP;
    int row_x     = (sw - row_w) / 2;
    int ans_row_y = SCRAMBLE_ROW_Y + ts + ANS_LABEL_GAP + ANS_SLOT_GAP;

    /* repaint HUD */
    theme_draw_panel(10, HUD_Y, sw - 20, HUD_HEIGHT);
    char pname[48]; sprintf(pname, "Player: %s", current_player.name);
    int hud_ty = HUD_Y + (HUD_HEIGHT + 8) / 2 - 8;
    gfx_color(0,   0,   0);   gfx_text(pname, HUD_LEFT_PAD+1, hud_ty+1, 1);
    gfx_color(155, 195, 255); gfx_text(pname, HUD_LEFT_PAD,   hud_ty,   1);
    char cat[64]; sprintf(cat, "[ %s ]", current_category);
    int cat_w = (int)strlen(cat) * 8;
    int cat_x = sw/2 - cat_w/2;
    gfx_color(0,   0,   0);   gfx_text(cat, cat_x+1, hud_ty+1, 1);
    gfx_color(215, 228, 255); gfx_text(cat, cat_x,   hud_ty,   1);

    char qnum[32]; sprintf(qnum, "%d/%d", word_idx, pool_size);
    int qnum_w = (int)strlen(qnum) * 8;
    int qnum_x = sw - HUD_LEFT_PAD - qnum_w - 10;
    gfx_color(0,   0,   0);   gfx_text(qnum, qnum_x+1, hud_ty+1, 1);
    gfx_color(155, 195, 255); gfx_text(qnum, qnum_x,   hud_ty,   1);

    /* repaint lives + score */
    int y;
    for (y = LIVES_ROW_Y; y < LIVES_ROW_Y + 24; y++) {
        int g = y * 100 / sh;
        gfx_color(8 + g*20/100, 12 + g*32/100, 38 + g*38/100);
        gfx_fillrectangle(0, y, sw, 1);
    }
    gfx_color(0,   0,   0);   gfx_text("LIFE:", LIVES_LABEL_X+1, LIVES_ROW_Y+5, 1);
    gfx_color(200, 200, 220); gfx_text("LIFE:", LIVES_LABEL_X,   LIVES_ROW_Y+4, 1);
    int i;
    for (i = 0; i < TOTAL_LIVES; i++)
        theme_draw_heart(HEARTS_X + i*HEARTS_SPACING, LIVES_ROW_Y - 5, i < lives ? 1 : 0);
    char sbuf[24]; sprintf(sbuf, "SCORE: %d", score);
    int sw2 = (int)strlen(sbuf)*8, sx = sw - sw2 - SCORE_RIGHT_PAD;
    gfx_color(0,   0,   0);   gfx_text(sbuf, sx+1, LIVES_ROW_Y+1, 1);
    gfx_color(255, 215, 0);   gfx_text(sbuf, sx,   LIVES_ROW_Y,   1);

    /* erase only the tile area */
    for (y = SCRAMBLE_ROW_Y - 2; y <= ans_row_y + ts + 4; y++) {
        int g = y * 100 / sh;
        gfx_color(8 + g*20/100, 12 + g*32/100, 38 + g*38/100);
        gfx_fillrectangle(row_x - 6, y, row_w + 12, 1);
    }

    /* redraw tiles */
    for (i = 0; i < tile_count; i++) {
        if (!tile_used[i]) draw_tile(tile_x[i], tile_y[i], ts, scrambled[i], 0);
        else               draw_slot_ghost(tile_x[i], tile_y[i], ts);
    }
    draw_down_arrow(sw/2, SCRAMBLE_ROW_Y + ts + ARROW_GAP);
    for (i = 0; i < tile_count; i++) {
        int ti = answer_slots[i];
        if (ti == -1) draw_slot_empty(slot_x[i], slot_y[i], ts);
        else          draw_tile(slot_x[i], slot_y[i], ts, scrambled[ti], 1);
    }
}

/* ================================================================
   SECTION 15 — INPUT HANDLER
   ================================================================ */
GameState game_handle_input(char event)
{
    if (event != 1) return STATE_PLAYING;
    int mx = gfx_xpos(), my = gfx_ypos();

    /* MENU */
    if (mx >= quit_btn.x && mx <= quit_btn.x + quit_btn.w &&
        my >= quit_btn.y && my <= quit_btn.y + quit_btn.h) {
        play_sound("menu_click.wav");
        save_final_score();
        result_set_score(score);
        result_set_end_reason(2);
        stop_bg_music();
        return STATE_RESULT;
    }

    if (showing_feedback) return STATE_PLAYING;

    /* CLEAR */
    if (mx >= clear_btn.x && mx <= clear_btn.x + clear_btn.w &&
        my >= clear_btn.y && my <= clear_btn.y + clear_btn.h) {
        int i;
        for (i = hint_count; i < tile_count; i++) { 
            if (answer_slots[i] != -1) {
                tile_used[answer_slots[i]] = 0; 
                answer_slots[i] = -1; 
            }
        }
        slots_filled = hint_count;
        play_sound("click.wav");
        return STATE_PLAYING;
    }

    /* HINT */
    if (mx >= hint_btn.x && mx <= hint_btn.x + hint_btn.w &&
        my >= hint_btn.y && my <= hint_btn.y + hint_btn.h) {
        do_hint();
        return STATE_PLAYING;
    }

    /* click filled slot → remove tile (hint-locked slots are protected) */
    int i;
    for (i = 0; i < tile_count; i++) {
        if (answer_slots[i] == -1) continue;
        if (mx >= slot_x[i] && mx <= slot_x[i] + tile_size &&
            my >= slot_y[i] && my <= slot_y[i] + tile_size) {
            if (i < hint_count) return STATE_PLAYING;
            tile_used[answer_slots[i]] = 0;
            answer_slots[i] = -1;
            slots_filled--;
            play_sound("click.wav");
            return STATE_PLAYING;
        }
    }

    /* click scrambled tile → place in next empty slot */
    for (i = 0; i < tile_count; i++) {
        if (tile_used[i]) continue;
        if (mx >= tile_x[i] && mx <= tile_x[i] + tile_size &&
            my >= tile_y[i] && my <= tile_y[i] + tile_size) {
            if (slots_filled < tile_count) {
                int s;
                for (s = 0; s < tile_count; s++) {
                    if (answer_slots[s] == -1) {
                        tile_used[i] = 1; answer_slots[s] = i; slots_filled++;
                        play_sound("click.wav"); break;
                    }
                }
                if (slots_filled == tile_count) check_answer();
            }
            return STATE_PLAYING;
        }
    }
    return STATE_PLAYING;
}

/* ================================================================
   SECTION 16 — UPDATE  (called every frame)
   ================================================================ */
GameState game_update(void)
{
    if (word_idx >= pool_size && !showing_feedback) {
        save_final_score(); result_set_score(score);
        result_set_end_reason(0); stop_bg_music(); return STATE_RESULT;
    }

    if (showing_feedback) {
        if (time(NULL) >= feedback_expiry) {
            if (lives <= 0) {
                save_final_score(); result_set_score(score);
                result_set_end_reason(1); stop_bg_music(); return STATE_RESULT;
            }
            if (word_idx >= pool_size) {
                save_final_score(); result_set_score(score);
                result_set_end_reason(0); stop_bg_music(); return STATE_RESULT;
            }
            load_next_word();
            draw_game(); /* Force repaint for the new word */
        }
        return STATE_PLAYING;
    }

    int elapsed = (int)(time(NULL) - start_time);
    if (elapsed >= time_limit) {
        lives--;
        sprintf(feedback_msg, "TIME'S UP!  Answer: %s", current_word);
        showing_feedback = 1; feedback_expiry = time(NULL) + 3;
        play_sound("timeout.wav");
        draw_game(); /* Force full repaint to show timeout feedback immediately */
    }

    if (lives <= 0 && !showing_feedback) {
        save_final_score(); result_set_score(score);
        result_set_end_reason(1); stop_bg_music(); return STATE_RESULT;
    }
    return STATE_PLAYING;
}
