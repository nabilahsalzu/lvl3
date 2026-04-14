/* ================================================================
   main.c  —  Game Loop and Window Setup

   This is the entry point of the program.
   It opens the window, then runs the game loop forever until
   the player chooses Exit.

   THE GAME LOOP runs about 60 times per second and does 3 things:
     1. INPUT   — check if the player clicked or pressed a key
     2. UPDATE  — update game logic (timer, word checking, lives)
     3. RENDER  — draw the screen (only what changed)

   RENDER MODES (3 levels — cheapest to most expensive):
     REDRAW_TILES  — only repaint the tile rows after a tile click
     REDRAW_TIMER  — only repaint the timer bar each second
     REDRAW_FULL   — repaint the whole screen

   HOW TO EDIT WINDOW SIZE
   ────────────────────────
   Change WINDOW_SIZE_PERCENT (currently 75% of your screen).
   ================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <X11/Xlib.h>
#include "gfx.h"
#include "game.h"
#include "menu.h"
#include "player.h"
#include "scoreboard.h"
#include "result.h"
#include "category.h"
#include "intro.h"

/* ================================================================
   SECTION 1  —  SETTINGS  (edit these to change behaviour)
   ================================================================ */

/* Window size as a percentage of your monitor resolution.
   75 = window is 75% the size of the screen.
   Change to 100 for fullscreen-like, or 50 for a smaller window. */
#define WINDOW_SIZE_PERCENT       75

/* How long to sleep each frame in microseconds.
   16000 = about 60 frames per second. */
#define FRAME_SLEEP_MICROSECONDS  16000

/* ================================================================
   SECTION 2  —  REDRAW MODE CONSTANTS
   These tell the renderer what to draw this frame.
   ================================================================ */
#define REDRAW_NONE    0   /* nothing changed — skip drawing           */
#define REDRAW_FULL    1   /* redraw the entire screen                 */
#define REDRAW_TILES   2   /* only redraw the tile rows (no flicker)   */
#define REDRAW_TIMER   3   /* only redraw the timer bar                */

/* ================================================================
   SECTION 3  —  RENDER FUNCTION
   Calls the correct draw function for the current screen.
   ================================================================ */
static void render_current_screen(GameState current_state)
{
    switch (current_state)
    {
        case STATE_SELECT_PLAYER:  draw_player_select();  break;
        case STATE_MENU:           draw_menu();           break;
        case STATE_CATEGORY:       draw_category_menu();  break;
        case STATE_INTRO:          draw_intro();          break;
        case STATE_HELP:           draw_help();           break;
        case STATE_PLAYING:        draw_game();           break;
        case STATE_SCOREBOARD:     draw_scoreboard();     break;
        case STATE_RESULT:         draw_result();         break;
        default:                                          break;
    }
    gfx_flush();
}

/* ================================================================
   SECTION 4  —  MAIN  (program starts here)
   ================================================================ */
int main(void)
{
    /* ── Get monitor size to calculate window dimensions ───── */
    Display *x11_display   = XOpenDisplay(NULL);
    int      x11_screen    = DefaultScreen(x11_display);
    int      monitor_width  = DisplayWidth(x11_display,  x11_screen);
    int      monitor_height = DisplayHeight(x11_display, x11_screen);

    int window_width  = monitor_width  * WINDOW_SIZE_PERCENT / 100;
    int window_height = monitor_height * WINDOW_SIZE_PERCENT / 100;

    gfx_open(window_width, window_height, "WORD SCRAMBLE GAME");

    /* ── Game starts on the player select screen ───────────── */
    GameState current_state   = STATE_SELECT_PLAYER;
    int       redraw_mode     = REDRAW_FULL;
    time_t    last_timer_tick = 0;

    /* ============================================================
       MAIN GAME LOOP
       Runs until the player clicks Exit (STATE_EXIT).
       ============================================================ */
    while (current_state != STATE_EXIT)
    {
        time_t current_second = time(NULL);

        /* ── STEP 1: INPUT ─────────────────────────────────── */
        while (gfx_event_waiting())
        {
            char event_code = gfx_wait();

            if (event_code == 1)   /* mouse button was clicked */
            {
                int mouse_x          = gfx_xpos();
                int mouse_y          = gfx_ypos();
                GameState prev_state = current_state;

                switch (current_state)
                {
                    case STATE_SELECT_PLAYER:
                        current_state = player_handle_click(mouse_x, mouse_y);
                        break;
                    case STATE_MENU:
                        current_state = menu_handle_click(mouse_x, mouse_y);
                        break;
                    case STATE_CATEGORY:
                        current_state = category_handle_click(mouse_x, mouse_y);
                        break;
                    case STATE_INTRO:
                        current_state = intro_handle_click(mouse_x, mouse_y);
                        break;
                    case STATE_HELP:
                        current_state = help_handle_click(mouse_x, mouse_y);
                        break;
                    case STATE_PLAYING:
                        current_state = game_handle_input(event_code);
                        redraw_mode   = REDRAW_FULL;
                        break;
                    case STATE_SCOREBOARD:
                        current_state = scoreboard_handle_click(mouse_x, mouse_y);
                        break;
                    case STATE_RESULT:
                        current_state = result_handle_click(mouse_x, mouse_y);
                        if (current_state == STATE_PLAYING)
                            game_init();   /* Play Again — reset the game */
                        break;
                    default:
                        break;
                }

                /* Decide redraw level based on what just happened */
                if (current_state != prev_state)
                    redraw_mode = REDRAW_FULL;     /* screen changed      */
                else if (current_state == STATE_PLAYING)
                    redraw_mode = REDRAW_TILES;    /* tile click in game  */
                else
                    redraw_mode = REDRAW_FULL;
            }
            else   /* keyboard key was pressed */
            {
                if (current_state == STATE_SELECT_PLAYER) {
                    player_handle_key(event_code);
                    redraw_mode = REDRAW_FULL;
                }
                else if (current_state == STATE_PLAYING) {
                    current_state = game_handle_input(event_code);
                    redraw_mode   = REDRAW_FULL;
                }
            }
        }

        /* ── STEP 2: UPDATE GAME LOGIC ─────────────────────── */
        if (current_state == STATE_PLAYING)
        {
            GameState next_state = game_update();
            if (next_state != current_state) {
                current_state = next_state;
                redraw_mode   = REDRAW_FULL;
            }
        }

        /* ── STEP 3: RENDER ────────────────────────────────── */
        if (redraw_mode == REDRAW_FULL)
        {
            render_current_screen(current_state);
            redraw_mode     = REDRAW_NONE;
            last_timer_tick = current_second;
        }
        else if (redraw_mode == REDRAW_TILES && current_state == STATE_PLAYING)
        {
            draw_tiles_only();
            gfx_flush();
            redraw_mode     = REDRAW_NONE;
            last_timer_tick = current_second;
        }
        else if (redraw_mode   == REDRAW_NONE
              && current_state == STATE_PLAYING
              && current_second != last_timer_tick)
        {
            /* One second passed — update the timer bar only */
            draw_timer_only();
            gfx_flush();
            last_timer_tick = current_second;
        }

        usleep(FRAME_SLEEP_MICROSECONDS);
    }

    return 0;
}
