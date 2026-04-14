/* ================================================================
   theme.c  —  Minion City Night Theme

   COLOUR PALETTE
   ──────────────
   Sky            deep navy  #080C26  →  dark steel-blue  #1E3060
   Panels         navy  #0E1232  with gold left-bar  #FFD700
   Buttons        steel-blue / amber / crimson / slate
   Text           white labels / sky-blue subtitles / gold titles
   City           charcoal buildings / amber+cyan windows /
                  dark asphalt road / gold lampposts

   PIXEL SIZES  (change these to make things bigger or smaller)
   ───────────
   MINION_PIXEL   — size of one pixel block in the minion sprite
   HEART_PIXEL    — size of one pixel block in the heart sprite

   BUTTON COLOUR HELPERS  (defined in theme.h)
   ──────────────────────
   COL_BTN_DEFAULT  — blue/slate
   COL_BTN_RED      — crimson
   COL_BTN_GREEN    — amber-green
   ================================================================ */

#include <stdio.h>
#include <string.h>
#include "theme.h"
#include "gfx.h"

/* Shorthand so we don't write gfx_color() on every line */
#define SET_COLOR(r, g, b)  gfx_color((r), (g), (b))

/* ================================================================
   MINION SPRITE  —  16 columns × 16 rows
   Colour codes:
     1 = yellow skin
     2 = blue overalls
     3 = black outline / pupil
     4 = white goggle glass
     5 = grey goggle rim
   ================================================================ */
static const int minion_sprite[16][16] = {
    { 0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0 },  /* row 0  top of head  */
    { 0,0,0,0,1,1,1,5,5,1,1,1,0,0,0,0 },  /* row 1  hair         */
    { 0,0,0,1,1,1,5,4,4,5,1,1,1,0,0,0 },  /* row 2  goggle top   */
    { 0,0,0,3,3,5,4,3,3,4,5,3,3,0,0,0 },  /* row 3  pupils       */
    { 0,0,0,3,3,5,4,3,3,4,5,3,3,0,0,0 },  /* row 4  pupils       */
    { 0,0,0,1,1,1,5,4,4,5,1,1,1,0,0,0 },  /* row 5  goggle bot   */
    { 0,0,0,1,1,1,1,5,5,1,1,1,1,0,0,0 },  /* row 6  face         */
    { 0,0,0,1,1,3,1,1,1,1,3,1,1,0,0,0 },  /* row 7  mouth line   */
    { 0,0,0,1,1,1,3,3,3,3,1,1,1,0,0,0 },  /* row 8  chin         */
    { 0,0,0,2,1,1,1,1,1,1,1,1,2,0,0,0 },  /* row 9  overalls top */
    { 0,0,1,1,2,2,2,2,2,2,2,2,1,1,0,0 },  /* row 10 body         */
    { 0,3,3,1,1,2,2,2,2,2,2,1,1,3,3,0 },  /* row 11 suspenders   */
    { 0,0,3,1,2,2,2,2,2,2,2,2,1,3,0,0 },  /* row 12 overalls mid */
    { 0,0,0,2,2,2,2,2,2,2,2,2,2,0,0,0 },  /* row 13 overalls bot */
    { 0,0,0,0,0,3,3,0,0,3,3,0,0,0,0,0 },  /* row 14 upper legs   */
    { 0,0,0,0,3,3,3,0,0,3,3,3,0,0,0,0 },  /* row 15 feet         */
};

/* Set the gfx colour for a given minion colour code */
static void set_minion_color(int code)
{
    switch (code) {
        case 1: SET_COLOR(255, 204,   0); break;  /* yellow skin   */
        case 2: SET_COLOR(  0, 102, 204); break;  /* blue overalls */
        case 3: SET_COLOR(  0,   0,   0); break;  /* black outline */
        case 4: SET_COLOR(255, 255, 255); break;  /* white goggle  */
        case 5: SET_COLOR(155, 155, 155); break;  /* grey goggle   */
        default: break;
    }
}

/* Draw the single minion centred at (cx, cy) */
void theme_draw_minion(int cx, int cy)
{
    int row, col;
    int pixel = MINION_PIXEL;
    int start_x = cx - (16 * pixel) / 2;
    int start_y = cy - (16 * pixel) / 2;

    for (row = 0; row < 16; row++) {
        for (col = 0; col < 16; col++) {
            int code = minion_sprite[row][col];
            if (code == 0) continue;   /* transparent */
            set_minion_color(code);
            gfx_fillrectangle(start_x + col * pixel,
                              start_y + row * pixel,
                              pixel, pixel);
        }
    }
}

/* ================================================================
   TWO-MINION SPRITE  —  13 columns × 18 rows
   (Two minion faces side-by-side sharing one body)
   Colour codes are the same as the single minion above.
   ================================================================ */
static const int two_minion_sprite[18][13] = {
    { 0,0,1,1,1,1,1,1,1,1,1,0,0 },  /* row 0  top of head        */
    { 0,1,5,5,5,1,1,1,5,5,5,1,0 },  /* row 1  goggle rims top    */
    { 1,5,4,4,4,5,1,5,4,4,4,5,1 },  /* row 2  goggle glass       */
    { 5,4,4,4,4,4,5,4,4,4,4,4,5 },  /* row 3  goggle glass mid   */
    { 5,4,4,3,3,4,5,4,3,3,4,4,5 },  /* row 4  pupils             */
    { 5,4,4,3,3,4,5,4,3,3,4,4,5 },  /* row 5  pupils             */
    { 1,5,4,4,4,5,1,5,4,4,4,5,1 },  /* row 6  goggle glass bot   */
    { 1,1,5,5,5,1,1,1,5,5,5,1,1 },  /* row 7  face               */
    { 1,1,1,1,1,3,3,3,1,1,1,1,1 },  /* row 8  mouth              */
    { 2,1,1,1,1,1,1,1,1,1,1,1,2 },  /* row 9  overalls top       */
    { 1,3,2,2,2,2,2,2,2,2,2,3,1 },  /* row 10 suspenders         */
    { 1,2,2,2,3,3,3,3,3,2,2,2,1 },  /* row 11 pocket top         */
    { 1,2,2,2,3,2,2,2,3,2,2,2,1 },  /* row 12 pocket middle      */
    { 3,2,2,2,2,3,3,3,2,2,2,2,3 },  /* row 13 overalls mid       */
    { 3,2,2,2,2,2,2,2,2,2,2,2,3 },  /* row 14 overalls bot       */
    { 0,0,2,2,2,2,2,2,2,2,2,2,0 },  /* row 15 upper legs         */
    { 0,0,0,0,3,3,0,3,3,0,0,0,0 },  /* row 16 lower legs         */
    { 0,0,0,3,3,3,0,3,3,3,0,0,0 },  /* row 17 feet               */
};

/* Draw the two-minion sprite centred at (cx, cy) */
void theme_draw_two_minions(int cx, int cy, int pixel_size)
{
    int row, col;
    int sprite_cols = 13;
    int sprite_rows = 18;
    int start_x = cx - (sprite_cols * pixel_size) / 2;
    int start_y = cy - (sprite_rows * pixel_size) / 2;

    for (row = 0; row < sprite_rows; row++) {
        for (col = 0; col < sprite_cols; col++) {
            int code = two_minion_sprite[row][col];
            if (code == 0) continue;
            set_minion_color(code);
            gfx_fillrectangle(start_x + col * pixel_size,
                              start_y + row * pixel_size,
                              pixel_size, pixel_size);
        }
    }
}

/* ================================================================
   HEART SPRITE  —  11 columns × 11 rows
   Colour codes:
     1 = main red area
     2 = bright highlight (top-left of each lobe)
   ================================================================ */
static const int heart_shape[11][11] = {
    { 0,0,1,1,0,0,0,1,1,0,0 },  /* row 0  top lobes        */
    { 0,1,1,1,1,0,1,1,1,1,0 },  /* row 1  lobe fill        */
    { 1,1,2,1,1,1,1,2,1,1,1 },  /* row 2  highlight        */
    { 1,1,1,1,1,1,1,1,1,1,1 },  /* row 3                   */
    { 1,1,1,1,1,1,1,1,1,1,1 },  /* row 4  widest point     */
    { 0,1,1,1,1,1,1,1,1,1,0 },  /* row 5                   */
    { 0,0,1,1,1,1,1,1,1,0,0 },  /* row 6                   */
    { 0,0,0,1,1,1,1,1,0,0,0 },  /* row 7                   */
    { 0,0,0,0,1,1,1,0,0,0,0 },  /* row 8                   */
    { 0,0,0,0,0,1,0,0,0,0,0 },  /* row 9  tip              */
    { 0,0,0,0,0,0,0,0,0,0,0 },  /* row 10 empty            */
};

/* Draw one heart at (x, y).
   filled = 1 → red heart (life remaining)
   filled = 0 → dark grey (life lost) */
void theme_draw_heart(int x, int y, int filled)
{
    int row, col;
    int pixel = HEART_PIXEL;

    for (row = 0; row < 11; row++) {
        for (col = 0; col < 11; col++) {
            int code = heart_shape[row][col];
            if (code == 0) continue;

            if (filled) {
                if (code == 2) SET_COLOR(255, 130, 130);  /* highlight */
                else           SET_COLOR(215,  38,  38);  /* main red  */
            } else {
                SET_COLOR(50, 50, 65);  /* empty / lost life */
            }

            gfx_fillrectangle(x + col * pixel, y + row * pixel,
                              pixel, pixel);
        }
    }
}

/* ================================================================
   CITY BACKGROUND PIECES
   ================================================================ */

/* Draw one city building.
   (bx, by) = bottom-left corner of the building
   bw = building width   bh = building height
   r,g,b   = wall colour
   wr,wg,wb = window colour (lit windows) */
static void draw_building(int bx, int by,
                           int bw, int bh,
                           int r,  int g,  int b,
                           int wr, int wg, int wb)
{
    /* Drop shadow — offset 4px right and down */
    SET_COLOR(0, 0, 0);
    gfx_fillrectangle(bx + 4, by - bh + 4, bw, bh);

    /* Main wall */
    SET_COLOR(r, g, b);
    gfx_fillrectangle(bx, by - bh, bw, bh);

    /* Left edge highlight (slightly brighter) */
    int left_r = r + 22; if (left_r > 255) left_r = 255;
    int left_g = g + 22; if (left_g > 255) left_g = 255;
    int left_b = b + 22; if (left_b > 255) left_b = 255;
    SET_COLOR(left_r, left_g, left_b);
    gfx_fillrectangle(bx, by - bh, 4, bh);

    /* Right edge shadow (slightly darker) */
    int dark_r = r - 22; if (dark_r < 0) dark_r = 0;
    int dark_g = g - 22; if (dark_g < 0) dark_g = 0;
    int dark_b = b - 22; if (dark_b < 0) dark_b = 0;
    SET_COLOR(dark_r, dark_g, dark_b);
    gfx_fillrectangle(bx + bw - 4, by - bh, 4, bh);

    /* Roof ledge */
    SET_COLOR(left_r, left_g, left_b);
    gfx_fillrectangle(bx, by - bh, bw, 5);

    /* Black outline border */
    SET_COLOR(0, 0, 0);
    gfx_fillrectangle(bx,          by - bh, bw, 2);  /* top    */
    gfx_fillrectangle(bx,          by - 2,  bw, 2);  /* bottom */
    gfx_fillrectangle(bx,          by - bh,  2, bh); /* left   */
    gfx_fillrectangle(bx + bw - 2, by - bh,  2, bh); /* right  */

    /* Windows grid
       ww = window width   wh = window height
       gx = horizontal gap between windows
       gy = vertical gap between windows */
    {
        int window_w    = 8;
        int window_h    = 10;
        int gap_x       = 10;
        int gap_y       = 14;
        int win_x, win_y;

        for (win_y = by - bh + 12; win_y + window_h < by - 5;
             win_y += window_h + gap_y) {
            for (win_x = bx + 8; win_x + window_w < bx + bw - 5;
                 win_x += window_w + gap_x) {

                /* Some windows are dark — use position to decide */
                int is_lit = ((win_x + win_y * 3) + win_x / 11) % 4 != 0;

                if (is_lit) {
                    /* Window frame (dark) */
                    SET_COLOR(wr / 4, wg / 4, wb / 4);
                    gfx_fillrectangle(win_x - 1, win_y - 1,
                                      window_w + 2, window_h + 2);
                    /* Window glow */
                    SET_COLOR(wr, wg, wb);
                    gfx_fillrectangle(win_x, win_y, window_w, window_h);
                    /* Inner bright spot */
                    int bright_r = wr + 55; if (bright_r > 255) bright_r = 255;
                    int bright_g = wg + 45; if (bright_g > 255) bright_g = 255;
                    SET_COLOR(bright_r, bright_g, wb);
                    gfx_fillrectangle(win_x + 2, win_y + 2,
                                      window_w - 4, window_h - 4);
                } else {
                    /* Dark unlit window */
                    SET_COLOR(r - 12 < 0 ? 0 : r - 12,
                              g - 12 < 0 ? 0 : g - 12,
                              b - 12 < 0 ? 0 : b - 12);
                    gfx_fillrectangle(win_x, win_y, window_w, window_h);
                }
            }
        }
    }
}

/* Draw a rooftop antenna at (cx, top_y) */
static void draw_antenna(int cx, int top_y)
{
    SET_COLOR(75, 75, 90);
    gfx_fillrectangle(cx - 1, top_y - 24, 2, 24);  /* pole  */

    SET_COLOR(220, 60, 60);
    gfx_fillrectangle(cx - 2, top_y - 27, 4, 4);   /* light */

    SET_COLOR(255, 160, 160);
    gfx_fillrectangle(cx - 1, top_y - 26, 2, 2);   /* glow  */
}

/* Draw a rooftop water tower centred at (cx) with base at (bottom_y) */
static void draw_water_tower(int cx, int bottom_y)
{
    /* Three wooden support legs */
    SET_COLOR(80, 52, 22);
    gfx_fillrectangle(cx - 12, bottom_y - 28, 3, 28);
    gfx_fillrectangle(cx +  9, bottom_y - 28, 3, 28);
    gfx_fillrectangle(cx -  1, bottom_y - 28, 3, 28);

    /* Tank shadow */
    SET_COLOR(0, 0, 0);
    gfx_fillrectangle(cx - 12 + 3, bottom_y - 50 + 3, 24, 22);

    /* Tank body */
    SET_COLOR(105, 65, 30);
    gfx_fillrectangle(cx - 12, bottom_y - 50, 24, 22);

    /* Tank wooden slat lines */
    SET_COLOR(65, 40, 12);
    gfx_fillrectangle(cx - 12, bottom_y - 44, 24, 3);
    gfx_fillrectangle(cx - 12, bottom_y - 36, 24, 3);

    /* Roof cap */
    SET_COLOR(85, 50, 18);
    gfx_fillrectangle(cx - 13, bottom_y - 52, 26, 4);
}

/* Draw a street lamppost at (x) with base at (ground_y) */
static void draw_lamppost(int x, int ground_y)
{
    /* Pole */
    SET_COLOR(52, 52, 68);
    gfx_fillrectangle(x,      ground_y - 62, 4,  62);
    gfx_fillrectangle(x - 13, ground_y - 62, 15,  4); /* arm */

    /* Lamp housing */
    SET_COLOR(195, 162, 55);
    gfx_fillrectangle(x - 17, ground_y - 70, 13, 10);

    /* Bulb glow */
    SET_COLOR(255, 228, 115);
    gfx_fillrectangle(x - 15, ground_y - 68, 9, 6);

    /* Ground glow puddle */
    SET_COLOR(35, 28, 6);
    gfx_fillrectangle(x - 22, ground_y - 3, 34, 3);
}

/* Draw a dark night cloud centred at (cx, cy) at pixel scale p */
static void draw_city_cloud(int cx, int cy, int p)
{
    static const int cloud_shape[5][9] = {
        { 0,0,1,1,1,1,1,0,0 },
        { 0,1,1,1,1,1,1,1,0 },
        { 1,1,1,1,1,1,1,1,1 },
        { 0,1,1,1,1,1,1,1,0 },
        { 0,0,1,1,1,1,1,0,0 },
    };
    int start_x = cx - (9 * p) / 2;
    int start_y = cy - (5 * p) / 2;
    int row, col;

    /* Slightly expanded dark outline */
    SET_COLOR(42, 48, 72);
    for (row = 0; row < 5; row++) {
        for (col = 0; col < 9; col++) {
            if (!cloud_shape[row][col]) continue;
            gfx_fillrectangle(start_x + col * p - 1,
                              start_y + row * p - 1,
                              p + 2, p + 2);
        }
    }

    /* Main cloud body */
    SET_COLOR(68, 78, 112);
    for (row = 0; row < 5; row++) {
        for (col = 0; col < 9; col++) {
            if (!cloud_shape[row][col]) continue;
            gfx_fillrectangle(start_x + col * p,
                              start_y + row * p,
                              p, p);
        }
    }

    /* Top highlight row */
    SET_COLOR(95, 108, 148);
    for (col = 2; col < 6; col++) {
        if (!cloud_shape[0][col]) continue;
        gfx_fillrectangle(start_x + col * p, start_y, p, p);
    }
}

/* Draw 85 randomly-positioned stars in the upper sky area */
static void draw_stars(int sky_width, int sky_height)
{
    int i;
    for (i = 0; i < 85; i++) {
        /* Use fixed math instead of rand() so stars don't move each frame */
        int star_x  = (i * 137 + 41) % sky_width;
        int star_y  = (i *  97 + 23) % sky_height;
        int bright  = 145 + (i * 53) % 110;

        if (bright > 220) {
            /* Bright star — draw a small cross shape */
            SET_COLOR(bright, bright, bright - 25);
            gfx_fillrectangle(star_x - 1, star_y,     3, 1);
            gfx_fillrectangle(star_x,     star_y - 1, 1, 3);
        } else {
            /* Dim star — just a 2×2 dot */
            SET_COLOR(bright, bright, bright);
            gfx_fillrectangle(star_x, star_y, 2, 2);
        }
    }
}

/* Draw a circular moon at (cx, cy) with radius r */
static void draw_moon(int cx, int cy, int radius)
{
    int dx, dy;

    /* Dark background square first (covers sky behind moon) */
    SET_COLOR(35, 32, 15);
    gfx_fillrectangle(cx - radius - 5, cy - radius - 5,
                      (radius + 5) * 2, (radius + 5) * 2);

    /* Moon disc — draw pixel by pixel in a circle */
    SET_COLOR(225, 215, 135);
    for (dy = -radius; dy <= radius; dy++) {
        for (dx = -radius; dx <= radius; dx++) {
            if (dx * dx + dy * dy <= radius * radius)
                gfx_fillrectangle(cx + dx, cy + dy, 1, 1);
        }
    }

    /* Moon craters — darker patches */
    SET_COLOR(195, 182, 105);
    gfx_fillrectangle(cx + 4, cy - 5, 6, 6);
    gfx_fillrectangle(cx - 9, cy + 2, 5, 5);
    gfx_fillrectangle(cx + 1, cy + 7, 4, 4);

    /* Crater highlights */
    SET_COLOR(208, 196, 120);
    gfx_fillrectangle(cx + 5, cy - 4, 3, 3);
    gfx_fillrectangle(cx - 8, cy + 3, 3, 3);
}

/* Draw the road strip at y=ground_y spanning full screen width */
static void draw_road(int ground_y, int screen_w)
{
    /* Asphalt base */
    SET_COLOR(25, 25, 32);
    gfx_fillrectangle(0, ground_y, screen_w, 30);

    /* Kerb lines */
    SET_COLOR(55, 55, 70);
    gfx_fillrectangle(0, ground_y,      screen_w, 3);
    gfx_fillrectangle(0, ground_y + 27, screen_w, 3);

    /* Centre lane dashes — yellow */
    {
        int x;
        SET_COLOR(190, 170, 38);
        for (x = 0; x < screen_w; x += 40)
            gfx_fillrectangle(x, ground_y + 13, 24, 4);
    }

    /* Lane divider dots — grey */
    {
        int x;
        SET_COLOR(65, 65, 80);
        for (x = 0; x < screen_w; x += 28)
            gfx_fillrectangle(x, ground_y + 7, 16, 2);
    }
}

/* ================================================================
   MAIN BACKGROUND  — draws the full night city scene
   ================================================================ */
void theme_draw_bg(int screen_w, int screen_h)
{
    int y, x;

    /* ── 1. Night sky gradient ─────────────────────────────
       Top:    deep navy    (8, 12, 38)
       Bottom: steel-blue   (28, 44, 76) */
    for (y = 0; y < screen_h; y++) {
        int gradient = y * 100 / screen_h;
        SET_COLOR(8  + gradient * 20 / 100,
                  12 + gradient * 32 / 100,
                  38 + gradient * 38 / 100);
        gfx_fillrectangle(0, y, screen_w, 1);
    }

    /* ── 2. Stars (upper 62% of screen) ─────────────────── */
    draw_stars(screen_w, screen_h * 62 / 100);

    /* ── 3. Moon ─────────────────────────────────────────── */
    draw_moon(screen_w * 80 / 100, screen_h * 11 / 100, 26);

    /* ── 4. Clouds ───────────────────────────────────────── */
    draw_city_cloud(screen_w * 12 / 100, screen_h *  9 / 100, 5);
    draw_city_cloud(screen_w * 38 / 100, screen_h *  6 / 100, 4);
    draw_city_cloud(screen_w * 65 / 100, screen_h * 12 / 100, 5);
    draw_city_cloud(screen_w * 88 / 100, screen_h *  8 / 100, 3);

    /* Ground line — everything below this is city */
    int ground_y = screen_h * 72 / 100;

    /* ── 5. Far building silhouettes (darkest, no windows) ── */
    SET_COLOR(16, 20, 40);
    gfx_fillrectangle(0,                          ground_y - 105, screen_w * 18 / 100, 105);
    gfx_fillrectangle(screen_w * 22 / 100,        ground_y - 140, screen_w * 14 / 100, 140);
    gfx_fillrectangle(screen_w * 38 / 100,        ground_y -  90, screen_w * 10 / 100,  90);
    gfx_fillrectangle(screen_w * 50 / 100,        ground_y - 125, screen_w * 16 / 100, 125);
    gfx_fillrectangle(screen_w * 68 / 100,        ground_y - 150, screen_w * 12 / 100, 150);
    gfx_fillrectangle(screen_w * 82 / 100,        ground_y -  95, screen_w * 18 / 100,  95);

    /* Mid silhouettes (slightly lighter) */
    SET_COLOR(23, 28, 52);
    gfx_fillrectangle(screen_w *  5 / 100, ground_y -  75, screen_w * 12 / 100,  75);
    gfx_fillrectangle(screen_w * 19 / 100, ground_y - 115, screen_w *  9 / 100, 115);
    gfx_fillrectangle(screen_w * 44 / 100, ground_y -  65, screen_w *  8 / 100,  65);
    gfx_fillrectangle(screen_w * 55 / 100, ground_y - 100, screen_w * 10 / 100, 100);
    gfx_fillrectangle(screen_w * 73 / 100, ground_y -  80, screen_w *  9 / 100,  80);
    gfx_fillrectangle(screen_w * 86 / 100, ground_y - 120, screen_w * 10 / 100, 120);

    /* ── 6. Midground buildings (with windows) ────────────── */
    draw_building(screen_w *  2 / 100, ground_y, screen_w * 11 / 100, 170,
                  40,46,66,   205,170,78);
    draw_building(screen_w * 14 / 100, ground_y, screen_w *  9 / 100, 140,
                  33,38,58,   175,205,252);
    draw_building(screen_w * 24 / 100, ground_y, screen_w *  7 / 100, 108,
                  48,52,70,   252,196,78);
    draw_building(screen_w * 64 / 100, ground_y, screen_w *  8 / 100, 125,
                  48,52,70,   252,196,78);
    draw_building(screen_w * 73 / 100, ground_y, screen_w *  9 / 100, 145,
                  33,38,58,   175,205,252);
    draw_building(screen_w * 83 / 100, ground_y, screen_w * 11 / 100, 175,
                  40,46,66,   205,170,78);

    /* ── 7. Foreground buildings (tallest, most detail) ───── */
    draw_building(screen_w *  0 / 100, ground_y, screen_w *  8 / 100, 195,
                  52,58,80,   252,210,95);
    draw_building(screen_w *  9 / 100, ground_y, screen_w * 10 / 100, 215,
                  36,42,62,   195,228,252);
    draw_building(screen_w * 20 / 100, ground_y, screen_w *  7 / 100, 160,
                  60,63,85,   252,190,72);

    /* Centre-left skyscraper (tallest) */
    draw_building(screen_w * 28 / 100, ground_y, screen_w * 12 / 100, 255,
                  28,33,56,   182,212,252);
    draw_antenna(screen_w * 28 / 100 + screen_w * 6 / 100, ground_y - 255);
    draw_water_tower(screen_w * 28 / 100 + screen_w * 9 / 100, ground_y - 255);

    /* Centre-right skyscraper */
    draw_building(screen_w * 60 / 100, ground_y, screen_w * 12 / 100, 250,
                  28,33,56,   182,212,252);
    draw_antenna(screen_w * 60 / 100 + screen_w * 6 / 100, ground_y - 250);

    draw_building(screen_w * 73 / 100, ground_y, screen_w *  7 / 100, 155,
                  60,63,85,   252,190,72);
    draw_building(screen_w * 81 / 100, ground_y, screen_w * 10 / 100, 210,
                  36,42,62,   195,228,252);
    draw_building(screen_w * 92 / 100, ground_y, screen_w *  8 / 100, 190,
                  52,58,80,   252,210,95);

    /* ── 8. Road ─────────────────────────────────────────── */
    draw_road(ground_y, screen_w);

    /* ── 9. Pavement (footpath below road) ───────────────── */
    SET_COLOR(35, 35, 45);
    gfx_fillrectangle(0, ground_y + 30,
                      screen_w, screen_h - ground_y - 30);

    /* Pavement tile lines — vertical */
    SET_COLOR(42, 42, 54);
    for (x = 0; x < screen_w; x += 32)
        gfx_fillrectangle(x, ground_y + 30, 1, screen_h - ground_y - 30);

    /* Pavement tile lines — horizontal */
    for (y = ground_y + 32; y < screen_h; y += 18)
        gfx_fillrectangle(0, y, screen_w, 1);

    /* ── 10. Street lampposts ────────────────────────────── */
    draw_lamppost(screen_w * 10 / 100, ground_y + 30);
    draw_lamppost(screen_w * 35 / 100, ground_y + 30);
    draw_lamppost(screen_w * 65 / 100, ground_y + 30);
    draw_lamppost(screen_w * 90 / 100, ground_y + 30);
}

/* ================================================================
   HEADER BAR  — gold + blue stripe at very top of screen
   ================================================================ */
void theme_draw_header_bar(int screen_w)
{
    /* Gold bar */
    SET_COLOR(185, 145, 0);
    gfx_fillrectangle(0, 0, screen_w, 8);

    /* Gold highlight */
    SET_COLOR(255, 215, 0);
    gfx_fillrectangle(0, 0, screen_w, 3);

    /* Black divider */
    SET_COLOR(0, 0, 0);
    gfx_fillrectangle(0, 8, screen_w, 1);

    /* Blue stripe */
    SET_COLOR(0, 115, 195);
    gfx_fillrectangle(0, 9, screen_w, 3);

    /* Black divider */
    SET_COLOR(0, 0, 0);
    gfx_fillrectangle(0, 12, screen_w, 1);
}

/* ================================================================
   PANEL  — dark navy box with gold left bar
   ================================================================ */
void theme_draw_panel(int x, int y, int panel_w, int panel_h)
{
    int i;

    /* Drop shadow */
    SET_COLOR(0, 0, 0);
    gfx_fillrectangle(x + 5, y + 5, panel_w, panel_h);

    /* Panel body */
    SET_COLOR(12, 18, 48);
    gfx_fillrectangle(x, y, panel_w, panel_h);

    /* Subtle inner gradient (top half gets slightly lighter) */
    for (i = 0; i < panel_h / 2; i++) {
        int brightness = i * 14 / (panel_h / 2 + 1);
        SET_COLOR(12 + brightness,
                  18 + brightness / 2,
                  48 + brightness);
        gfx_fillrectangle(x + 2, y + 2 + i, panel_w - 4, 1);
    }

    /* Top shine line */
    SET_COLOR(38, 68, 155);
    gfx_fillrectangle(x + 2, y + 2, panel_w - 4, 3);

    /* Gold left accent bar */
    SET_COLOR(185, 142, 0);
    gfx_fillrectangle(x, y, 4, panel_h);
    SET_COLOR(255, 215, 0);
    gfx_fillrectangle(x, y, 2, panel_h);

    /* Black border */
    SET_COLOR(0, 0, 0);
    gfx_fillrectangle(x,             y,             panel_w, 2);  /* top    */
    gfx_fillrectangle(x,             y + panel_h - 2, panel_w, 2);  /* bottom */
    gfx_fillrectangle(x,             y,             2, panel_h);  /* left   */
    gfx_fillrectangle(x + panel_w - 2, y,           2, panel_h);  /* right  */
}

/* ================================================================
   BUTTON  — 3D bevel style with centred label
   r,g,b = button background colour
   ================================================================ */
void theme_draw_button(int x, int y, int btn_w, int btn_h,
                       int r, int g, int b, const char *label)
{
    int text_w, text_x, text_y;

    /* Drop shadow */
    SET_COLOR(0, 0, 0);
    gfx_fillrectangle(x + 4, y + 5, btn_w, btn_h);

    /* Button body */
    SET_COLOR(r, g, b);
    gfx_fillrectangle(x, y, btn_w, btn_h);

    /* Bottom-right bevel (darker = pressed-in shadow) */
    {
        int dark_r = r - 50; if (dark_r < 0) dark_r = 0;
        int dark_g = g - 50; if (dark_g < 0) dark_g = 0;
        int dark_b = b - 50; if (dark_b < 0) dark_b = 0;
        SET_COLOR(dark_r, dark_g, dark_b);
        gfx_fillrectangle(x,           y + btn_h - 4, btn_w,     4);
        gfx_fillrectangle(x + btn_w - 4, y,           4,         btn_h);
    }

    /* Top-left bevel (lighter = raised highlight) */
    {
        int light_r = r + 58; if (light_r > 255) light_r = 255;
        int light_g = g + 58; if (light_g > 255) light_g = 255;
        int light_b = b + 58; if (light_b > 255) light_b = 255;
        SET_COLOR(light_r, light_g, light_b);
        gfx_fillrectangle(x, y,           btn_w, 4);
        gfx_fillrectangle(x, y,           4,     btn_h);
    }

    /* Black border */
    SET_COLOR(0, 0, 0);
    gfx_fillrectangle(x,             y,             btn_w, 2);
    gfx_fillrectangle(x,             y + btn_h - 2, btn_w, 2);
    gfx_fillrectangle(x,             y,             2,     btn_h);
    gfx_fillrectangle(x + btn_w - 2, y,             2,     btn_h);

    /* Label — centred horizontally and vertically */
    text_w = (int)strlen(label) * 8;
    text_x = x + (btn_w - text_w) / 2;
    text_y = y + (btn_h + 8) / 2 - 8;

    SET_COLOR(0, 0, 0);
    gfx_text((char *)label, text_x + 1, text_y + 1, 1);  /* shadow */
    SET_COLOR(255, 255, 255);
    gfx_text((char *)label, text_x, text_y, 1);
}

/* Draw an outline-only button (for selected / hover state) */
void theme_draw_button_outline(int x, int y, int btn_w, int btn_h,
                               int r, int g, int b, const char *label)
{
    int text_w, text_x, text_y;

    /* Just the border lines */
    SET_COLOR(r, g, b);
    gfx_fillrectangle(x,             y,             btn_w, 2);
    gfx_fillrectangle(x,             y + btn_h - 2, btn_w, 2);
    gfx_fillrectangle(x,             y,             2,     btn_h);
    gfx_fillrectangle(x + btn_w - 2, y,             2,     btn_h);

    /* Label in the same colour */
    text_w = (int)strlen(label) * 8;
    text_x = x + (btn_w - text_w) / 2;
    text_y = y + (btn_h + 8) / 2 - 8;

    SET_COLOR(r, g, b);
    gfx_text((char *)label, text_x, text_y, 1);
}

/* ================================================================
   TEXT HELPERS
   ================================================================ */

/* Large title text with thick black outline (gold) */
void theme_draw_title(const char *text, int x, int y, int scale)
{
    int raised_y = y - 4;  /* raise 4px so text sits at visual centre */

    /* Black outline — draw in 8 directions */
    SET_COLOR(0, 0, 0);
    gfx_text((char *)text, x - 2, raised_y,     scale);
    gfx_text((char *)text, x + 2, raised_y,     scale);
    gfx_text((char *)text, x,     raised_y - 2, scale);
    gfx_text((char *)text, x,     raised_y + 2, scale);
    gfx_text((char *)text, x - 1, raised_y - 1, scale);
    gfx_text((char *)text, x + 1, raised_y - 1, scale);
    gfx_text((char *)text, x - 1, raised_y + 1, scale);
    gfx_text((char *)text, x + 1, raised_y + 1, scale);

    /* Gold text on top */
    SET_COLOR(255, 215, 0);
    gfx_text((char *)text, x, raised_y, scale);
}

/* Subtitle text — sky-blue with 1px black shadow */
void theme_draw_subtitle(const char *text, int x, int y)
{
    SET_COLOR(0, 0, 0);
    gfx_text((char *)text, x + 1, y - 3, 1);  /* shadow */
    SET_COLOR(215, 228, 255);
    gfx_text((char *)text, x, y - 4, 1);
}

/* Label text — softer blue, no shadow */
void theme_draw_label(const char *text, int x, int y)
{
    SET_COLOR(155, 195, 255);
    gfx_text((char *)text, x, y - 4, 1);
}

/* Horizontal divider line — gold with black shadow */
void theme_draw_divider(int x, int y, int length)
{
    SET_COLOR(0, 0, 0);
    gfx_fillrectangle(x, y + 1, length, 2);  /* shadow */
    SET_COLOR(195, 150, 0);
    gfx_fillrectangle(x, y,     length, 2);  /* gold   */
}

/* ================================================================
   LIVES DISPLAY  — a row of hearts centred at (cx)
   ================================================================ */
void theme_draw_lives(int lives_remaining, int max_lives, int cx, int y)
{
    int i;
    int heart_pixel   = HEART_PIXEL;
    int heart_width   = 11 * heart_pixel + 6;   /* sprite width + gap */
    int total_width   = max_lives * heart_width - 6;
    int start_x       = cx - total_width / 2;

    for (i = 0; i < max_lives; i++) {
        int is_filled = (i < lives_remaining) ? 1 : 0;
        theme_draw_heart(start_x + i * heart_width, y, is_filled);
    }
}

/* ================================================================
   SCORE BADGE  — gold box showing the score number
   ================================================================ */
void theme_draw_score_badge(int score_value, int x, int y)
{
    char score_text[32];
    int  text_x, text_y;
    int  badge_w, badge_h;

    sprintf(score_text, "SCORE  %d", score_value);
    badge_w = (int)strlen(score_text) * 8 + 24;
    badge_h = 28;

    /* Shadow */
    SET_COLOR(0, 0, 0);
    gfx_fillrectangle(x + 3, y + 3, badge_w, badge_h);

    /* Gold background */
    SET_COLOR(185, 140, 0);
    gfx_fillrectangle(x, y, badge_w, badge_h);

    /* Top shine */
    SET_COLOR(255, 218, 55);
    gfx_fillrectangle(x + 2, y + 2, badge_w - 4, 5);

    /* Black border */
    SET_COLOR(0, 0, 0);
    gfx_fillrectangle(x,              y,              badge_w, 2);
    gfx_fillrectangle(x,              y + badge_h - 2, badge_w, 2);
    gfx_fillrectangle(x,              y,              2, badge_h);
    gfx_fillrectangle(x + badge_w - 2, y,             2, badge_h);

    /* Score text centred */
    text_x = x + (badge_w - (int)strlen(score_text) * 8) / 2;
    text_y = y + (badge_h + 8) / 2 - 8;

    SET_COLOR(0, 0, 0);
    gfx_text(score_text, text_x + 1, text_y + 1, 1);
    SET_COLOR(28, 8, 0);
    gfx_text(score_text, text_x, text_y, 1);
}

/* ================================================================
   LETTER TILE  — a 3D-bevelled tile with a letter on it
   size = tile side length in pixels
   clicked = 1 → brighter / selected colour
   ================================================================ */
void theme_draw_tile(int x, int y, int size, char letter, int clicked)
{
    char letter_str[2] = { letter, '\0' };
    int  letter_x, letter_y;

    /* Drop shadow */
    SET_COLOR(0, 0, 0);
    gfx_fillrectangle(x + 3, y + 3, size, size);

    /* Tile body — brighter when clicked (answer slot) */
    SET_COLOR(clicked ? 14 : 7,
              clicked ? 100 : 48,
              clicked ? 188 : 132);
    gfx_fillrectangle(x, y, size, size);

    /* Top-left bevel highlight */
    SET_COLOR(clicked ? 58  : 28,
              clicked ? 155 : 92,
              clicked ? 235 : 182);
    gfx_fillrectangle(x + 2, y + 2, size - 4, 4);
    gfx_fillrectangle(x + 2, y + 2, 4, size - 4);

    /* Bottom-right bevel shadow */
    SET_COLOR(0,
              clicked ? 42 : 20,
              clicked ? 95 : 58);
    gfx_fillrectangle(x + 2,       y + size - 5, size - 4, 3);
    gfx_fillrectangle(x + size - 5, y + 2,       3,        size - 4);

    /* Black border */
    SET_COLOR(0, 0, 0);
    gfx_fillrectangle(x,          y,          size, 2);
    gfx_fillrectangle(x,          y + size - 2, size, 2);
    gfx_fillrectangle(x,          y,          2,    size);
    gfx_fillrectangle(x + size - 2, y,        2,    size);

    /* Letter centred on the tile */
    letter_x = x + (size - 8) / 2;
    letter_y = y + (size + 8) / 2 - 4;

    SET_COLOR(0, 0, 0);
    gfx_text(letter_str, letter_x + 1, letter_y + 1, 1);  /* shadow */
    SET_COLOR(255, 232, 0);
    gfx_text(letter_str, letter_x, letter_y, 1);
}

/* ================================================================
   FEEDBACK BANNER  — correct/wrong message box
   is_correct = 1 → green banner
   is_correct = 0 → red banner
   ================================================================ */
void theme_draw_feedback(const char *message, int is_correct,
                         int screen_w, int y)
{
    int banner_w   = (int)strlen(message) * 8 + 48;
    int banner_h   = 36;
    int banner_x   = (screen_w - banner_w) / 2;
    int text_x, text_y;

    /* Drop shadow */
    SET_COLOR(0, 0, 0);
    gfx_fillrectangle(banner_x + 3, y + 3, banner_w, banner_h);

    /* Banner background — green for correct, red for wrong */
    SET_COLOR(is_correct ? 20  : 135,
              is_correct ? 110 :  20,
              is_correct ?  42 :  20);
    gfx_fillrectangle(banner_x, y, banner_w, banner_h);

    /* Top shine */
    SET_COLOR(is_correct ? 60  : 178,
              is_correct ? 172 :  60,
              is_correct ?  75 :  60);
    gfx_fillrectangle(banner_x + 2, y + 2, banner_w - 4, 4);

    /* Black border */
    SET_COLOR(0, 0, 0);
    gfx_fillrectangle(banner_x,              y,              banner_w, 2);
    gfx_fillrectangle(banner_x,              y + banner_h - 2, banner_w, 2);
    gfx_fillrectangle(banner_x,              y,              2, banner_h);
    gfx_fillrectangle(banner_x + banner_w - 2, y,            2, banner_h);

    /* Message text centred */
    text_x = banner_x + (banner_w - (int)strlen(message) * 8) / 2;
    text_y = y + (banner_h + 8) / 2 - 4;

    SET_COLOR(0, 0, 0);
    gfx_text((char *)message, text_x + 1, text_y + 1, 1);
    SET_COLOR(255, 255, 255);
    gfx_text((char *)message, text_x, text_y, 1);
}

/* ================================================================
   TIMER BAR  — XP-style bar: green → yellow → orange → red
   ================================================================ */
void theme_draw_timer_bar(int time_left, int time_max,
                          int x, int y, int bar_w, int bar_h)
{
    int fill_width;
    int r, g, b;

    /* Dark background track */
    SET_COLOR(6, 10, 28);
    gfx_fillrectangle(x, y, bar_w, bar_h);
    SET_COLOR(3, 5, 16);
    gfx_fillrectangle(x + 1, y + 1, bar_w - 2, bar_h - 2);

    if (time_max <= 0) return;

    /* How many pixels to fill */
    fill_width = bar_w * time_left / time_max;
    if (fill_width < 0)    fill_width = 0;
    if (fill_width > bar_w) fill_width = bar_w;

    /* Colour changes as time runs low:
       > 60% remaining  → green
       > 40% remaining  → yellow-green
       > 20% remaining  → orange
       <= 20% remaining → red */
    if      (time_left > time_max * 3 / 5) { r = 38;  g = 185; b = 65;  }
    else if (time_left > time_max * 2 / 5) { r = 125; g = 185; b = 22;  }
    else if (time_left > time_max / 5)     { r = 235; g = 145; b = 0;   }
    else                                   { r = 210; g = 28;  b = 28;  }

    /* Fill bar */
    SET_COLOR(r, g, b);
    gfx_fillrectangle(x + 1, y + 1, fill_width - 2, bar_h - 2);

    /* Top shine stripe */
    {
        int shine_r = r + 62; if (shine_r > 255) shine_r = 255;
        int shine_g = g + 52; if (shine_g > 255) shine_g = 255;
        SET_COLOR(shine_r, shine_g, b);
        gfx_fillrectangle(x + 1, y + 1, fill_width - 2, 3);
    }

    /* Black border */
    SET_COLOR(0, 0, 0);
    gfx_fillrectangle(x,             y,             bar_w, 1);
    gfx_fillrectangle(x,             y + bar_h - 1, bar_w, 1);
    gfx_fillrectangle(x,             y,             1, bar_h);
    gfx_fillrectangle(x + bar_w - 1, y,             1, bar_h);
}

/* ================================================================
   SCOREBOARD ROW  — one entry in the high score list
   rank       = position number (1, 2, 3 ...)
   value      = score value
   is_best    = 1 if this is the all-time best score
   is_latest  = 1 if this is the most recently submitted score
   ================================================================ */
void theme_draw_score_row(int rank, int value,
                          int is_best, int is_latest,
                          int x, int y, int row_width)
{
    char rank_text[8];
    char score_text[16];

    /* Row background colour */
    if      (is_best)   SET_COLOR(160, 115,   0);  /* gold   — best   */
    else if (is_latest) SET_COLOR(  0,  68, 150);  /* blue   — latest */
    else                SET_COLOR( 10,  15,  45);  /* dark navy       */
    gfx_fillrectangle(x, y, row_width, 34);

    /* Top and bottom border lines */
    SET_COLOR(0, 0, 0);
    gfx_fillrectangle(x, y,      row_width, 1);
    gfx_fillrectangle(x, y + 33, row_width, 1);

    /* Left accent bar colour */
    if      (is_best)   SET_COLOR(255, 204,   0);  /* gold   */
    else if (is_latest) SET_COLOR(  0, 170, 252);  /* bright blue */
    else                SET_COLOR(  0,  95, 188);  /* steel blue  */
    gfx_fillrectangle(x, y, 4, 34);

    /* Rank number and score */
    sprintf(rank_text,  "%d.", rank);
    sprintf(score_text, "%d",  value);

    SET_COLOR(is_best ? 0 : 195, is_best ? 0 : 195, is_best ? 0 : 195);
    gfx_text(rank_text,  x + 12, y + 23, 1);

    SET_COLOR(is_best ? 0 : 252, is_best ? 0 : 252, is_best ? 0 : 252);
    gfx_text(score_text, x + 45, y + 23, 1);

    /* Badge label on the right */
    if (is_best && is_latest) {
        SET_COLOR(0, 0, 0);
        gfx_text("BEST+LATEST", x + row_width - 112, y + 23, 1);
    } else if (is_best) {
        SET_COLOR(0, 0, 0);
        gfx_text("BEST",        x + row_width - 42,  y + 23, 1);
    } else if (is_latest) {
        SET_COLOR(255, 215, 0);
        gfx_text("LATEST",      x + row_width - 58,  y + 23, 1);
    }
}
