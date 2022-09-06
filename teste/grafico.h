
#ifndef __GRAFICO__
#define __GRAFICO__

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>

#include "game_helpers.h"


typedef struct {
  	ALLEGRO_FONT *title_font;
	ALLEGRO_FONT *big_font;
	ALLEGRO_FONT *medium_font;
	ALLEGRO_FONT *small_font;
} fonts_t;

typedef struct {
	ALLEGRO_EVENT_QUEUE *event_queue;
	ALLEGRO_TIMER *timer;
	ALLEGRO_DISPLAY *display;
	ALLEGRO_DISPLAY_MODE disp_data;
	fonts_t *fonts;
} window;

/* Inicia sistema grafico Allegro 5.0 */
window graph_init(int res_width, int res_height);

void draw_menu(window *win);
void draw_setup(window *win, bouncer_t *bouncer, int squares[][COL], float offsetY, game_t *game);
void draw_wait(window *win, bouncer_t *bouncer, int squares[][COL], game_t *game);
void draw_aim(window *win, bouncer_t *bouncer, float distX, float distY, float dist, int squares[][COL], game_t *game);
void draw_shoot(window *win, bouncer_t **bouncer, int balls, int squares[][COL], game_t *game);
void draw_gameover(window *win, game_t *game);

/* Finaliza sistema grafico Allegro 5.0 */
void graph_deinit(window win);

#define PIXEL(r,g,b) (al_map_rgb((unsigned char) r, (unsigned char) g, (unsigned char) b))
#define CINZA PIXEL(128,128,128)
#define CINZA_ESCURO PIXEL(30, 30, 30)
#define PRETO PIXEL(0,0,0)
#define BRANCO PIXEL(255,255,255)
#define VERDE_CLARO PIXEL(144,238,144)


#endif