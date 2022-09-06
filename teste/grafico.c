#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "grafico.h"

#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#define min(x, y) (((x) < (y)) ? (x) : (y))

window graph_init(int res_width, int res_height) {
	window win = {NULL, NULL, NULL, {res_width, res_height, 0, 60}, NULL};

	/* Inicializa a Allegro */
	al_init();
	al_init_image_addon();
	al_init_native_dialog_addon();
	al_init_font_addon();
	al_init_primitives_addon();
	al_init_ttf_addon();

	win.display = al_create_display(res_width, res_height);

	al_install_mouse();
	al_install_keyboard();
	win.timer = al_create_timer(1.0 / win.disp_data.refresh_rate);

	/* Define Eventos */
	win.event_queue = al_create_event_queue();

	al_register_event_source(win.event_queue, al_get_display_event_source(win.display));
	al_register_event_source(win.event_queue, al_get_mouse_event_source());
	al_register_event_source(win.event_queue, al_get_keyboard_event_source());
	al_register_event_source(win.event_queue, al_get_timer_event_source(win.timer));

	al_clear_to_color(PRETO);
	al_flip_display();
	al_start_timer(win.timer);

	char font_path[100];
	sprintf(font_path, "%s/resources/dimitri/Minecraft.ttf", al_get_current_directory());
	printf("font_path: %s\n", font_path);


	fonts_t *fonts = malloc(sizeof(fonts_t));

	printf("loading title font\n");
	
	fonts->title_font = al_load_font(font_path, 100, 0);
	printf("fonts->title_font: %p\n", fonts->title_font);

	printf("loading big font\n");
	fonts->big_font = al_load_font(font_path, 70, 0);
	printf("fonts->big_font: %p\n", fonts->big_font);


	printf("loading medium font\n");
	fonts->medium_font = al_load_font(font_path, 50, 0);
	printf("fonts->medium_font: %p\n", fonts->medium_font);

	printf("loading small font\n");
	fonts->small_font = al_load_font(font_path, 30, 0);
	printf("fonts->small_font: %p\n", fonts->small_font);

	win.fonts = fonts;
	printf("====================================================\n");

	return (win);
}

void draw_menu(window *win) {
	ALLEGRO_BITMAP *bg_menu = al_load_bitmap("resources/utils/bg_menu.png");
	al_draw_bitmap(bg_menu, 0, 0, 0);

	al_draw_text(win->fonts->title_font, BRANCO, win->disp_data.width * 0.5, win->disp_data.height * 0.2, ALLEGRO_ALIGN_CENTER, "BALLz");
	
	ALLEGRO_BITMAP *button = al_load_bitmap("resources/button.png");
	al_draw_bitmap(button, (win->disp_data.width - al_get_bitmap_width(button)) * 0.5, win->disp_data.height * 0.6, 0);
	al_draw_bitmap(button, (win->disp_data.width - al_get_bitmap_width(button)) * 0.5, win->disp_data.height * 0.8, 0);

	al_draw_text(win->fonts->medium_font, BRANCO, win->disp_data.width * 0.5, win->disp_data.height * 0.6 + 7, ALLEGRO_ALIGN_CENTER, "Play");

	al_draw_text(win->fonts->medium_font, BRANCO, win->disp_data.width * 0.5, win->disp_data.height * 0.8 + 7, ALLEGRO_ALIGN_CENTER, "INFO");

	al_flip_display();
}

void draw_squares(window *win, int squares[][7], float offsetY) {
	float l = calc_square_side(win->disp_data.width);
	int i, j;
	for (i = 0; i < ROW; ++i) {
		for (j = 0; j < COL; ++j) {
			if (squares[i][j] > 0) {
				al_draw_filled_rectangle(calc_square_i_x(j, l), calc_square_i_y(i, l) + offsetY, calc_square_f_x(j, l), calc_square_f_y(i, l) + offsetY, al_map_rgb(240 - 3 * squares[i][j]%80, 172 - 3 * squares[i][j]%57, 46 + 3 * squares[i][j]%70));
				char text[10];
				int textOffset = al_get_font_line_height(win->fonts->small_font)/2;
				sprintf(text, "%d", squares[i][j]);
				al_draw_text(win->fonts->small_font, BRANCO, calc_square_mid_x(j, l), calc_square_mid_y(i, l) + offsetY - textOffset, ALLEGRO_ALIGN_CENTER, text);
			}
			if (squares[i][j] == -1) {
				al_draw_filled_circle(calc_square_mid_x(j, l), calc_square_mid_y(i, l) + offsetY, BOUNCER_RADIUS, BRANCO);
				al_draw_circle(calc_square_mid_x(j, l), calc_square_mid_y(i, l) + offsetY, BOUNCER_RADIUS + 10, BRANCO, 5);
			}
		}
	}
}

void draw_ground(window *win, bouncer_t *bouncer) {
	al_draw_filled_rectangle(0, calc_square_f_y(8, calc_square_side(win->disp_data.width)), win->disp_data.width, win->disp_data.height, CINZA_ESCURO);
}

void draw_score(window *win, game_t *game) {
	char text[20];
	sprintf(text, "Score: %d", game->score);
	al_draw_text(win->fonts->small_font, BRANCO, 20, game->shooting_y + BOUNCER_RADIUS + 5, ALLEGRO_ALIGN_LEFT, text);
	sprintf(text, "Highscore: %d", game->highscore);
	al_draw_text(win->fonts->small_font, BRANCO, win->disp_data.width-20, game->shooting_y + BOUNCER_RADIUS + 5, ALLEGRO_ALIGN_RIGHT, text);
}

void draw_bouncers_count(window *win, game_t *game) {
	char text[20];
	sprintf(text, "x%d", game->bouncers - game->thrown_bouncers);
	al_draw_text(win->fonts->small_font, CINZA, game->shooting_x - BOUNCER_RADIUS - 20, game->shooting_y - BOUNCER_RADIUS - 30, ALLEGRO_ALIGN_LEFT, text);
}

void draw_wait(window *win, bouncer_t *bouncer, int squares[][7], game_t *game) {
	if(al_event_queue_is_empty(win->event_queue)) {
		al_clear_to_color(PRETO);
		al_draw_filled_circle(bouncer->x, bouncer->y, BOUNCER_RADIUS, BRANCO);
		draw_ground(win, bouncer);
		draw_squares(win, squares, 0);
		draw_score(win, game);
		draw_bouncers_count(win, game);
		al_flip_display();
	}
}

void draw_setup(window *win, bouncer_t *bouncer, int squares[][COL], float offsetY, game_t *game) {
	if(al_event_queue_is_empty(win->event_queue)) {
		al_clear_to_color(PRETO);
		al_draw_filled_circle(bouncer->x, bouncer->y, BOUNCER_RADIUS, BRANCO);
		draw_ground(win, bouncer);
		draw_squares(win, squares, offsetY);
		draw_score(win, game);
		draw_bouncers_count(win, game);
		al_flip_display();
	}
}

void draw_aim(window *win, bouncer_t *bouncer, float distX, float distY, float dist, int squares[][COL], game_t *game) {
	if(al_is_event_queue_empty(win->event_queue)) {
		al_clear_to_color(PRETO);
		al_draw_filled_circle(bouncer->x, bouncer->y, BOUNCER_RADIUS, BRANCO);
		draw_squares(win, squares, 0);
		draw_ground(win, bouncer);
		draw_bouncers_count(win, game);

		float size = min(win->disp_data.height * 0.33 + dist, win->disp_data.height * 0.8);
		float spacing = (size - 80)/16;
		float position = size;

		al_draw_line(bouncer->x, bouncer->y, distX, distY, BRANCO, 2);
		position -= spacing;

		al_draw_filled_triangle(bouncer->x + (BOUNCER_RADIUS + 70) * distX/dist, 
								bouncer->y + (BOUNCER_RADIUS + 70) * distY/dist,

								bouncer->x + (BOUNCER_RADIUS + 5) * distX/dist + 7 * distY/dist, 
								bouncer->y + (BOUNCER_RADIUS + 5) * distY/dist - 7 * distX/dist, 

								bouncer->x + (BOUNCER_RADIUS + 5) * distX/dist - 7 * distY/dist, 
								bouncer->y + (BOUNCER_RADIUS + 5) * distY/dist + 7 * distX/dist, BRANCO);

		draw_score(win, game);
		al_flip_display();
	}
}

void draw_shoot(window *win, bouncer_t **bouncers, int bouncersCount, int squares[][COL], game_t *game) {
	if(al_is_event_queue_empty(win->event_queue)) {
		al_clear_to_color(PRETO);
		for(int i=0; i<bouncersCount; i++) {
			if (bouncers[i]) {
				al_draw_filled_circle(bouncers[i]->x, bouncers[i]->y, BOUNCER_RADIUS, BRANCO);
			}
		}
		if (game->thrown_bouncers != game->bouncers) {
			draw_bouncers_count(win, game);
		}
		draw_squares(win, squares, 0);
		draw_ground(win, bouncers[0]);
		draw_score(win, game);
		al_flip_display();
	}
}

void draw_gameover(window *win, game_t *game) {
	if(al_is_event_queue_empty(win->event_queue)) {
		char text[20];

		al_clear_to_color(PRETO);
		al_draw_text(win->fonts->big_font, BRANCO, win->disp_data.width * 0.5, win->disp_data.height * 0.2, ALLEGRO_ALIGN_CENTRE, "GAME OVER");
		sprintf(text, "Score: %d", game->score);
		al_draw_text(win->fonts->medium_font, BRANCO, win->disp_data.width * 0.5, win->disp_data.height * 0.4, ALLEGRO_ALIGN_CENTRE, text);

		if(game->score >= game->highscore) { 
			al_draw_text(win->fonts->small_font, VERDE_CLARO, win->disp_data.width * 0.5, win->disp_data.height * 0.5, ALLEGRO_ALIGN_CENTRE, "New Highscore!");
		}

		ALLEGRO_BITMAP *button = al_load_bitmap("resources/button.png");
		al_draw_bitmap(button, (win->disp_data.width - al_get_bitmap_width(button)) * 0.5, win->disp_data.height * 0.6, 0);
		al_draw_text(win->fonts->medium_font, BRANCO, win->disp_data.width * 0.5, win->disp_data.height * 0.6 + 7, ALLEGRO_ALIGN_CENTER, "Play");

		al_draw_bitmap(button, (win->disp_data.width - al_get_bitmap_width(button)) * 0.5, win->disp_data.height * 0.75, 0);
		al_draw_text(win->fonts->medium_font, BRANCO, win->disp_data.width * 0.5, win->disp_data.height * 0.75 + 7, ALLEGRO_ALIGN_CENTER, "Menu");
		
		al_flip_display();
	}
}


void graph_deinit(window win) {
	al_destroy_timer(win.timer);
	al_destroy_event_queue(win.event_queue);
	al_destroy_display(win.display);

	al_destroy_font(win.fonts->title_font);
	al_destroy_font(win.fonts->medium_font);
	al_destroy_font(win.fonts->small_font);
	free(win.fonts);
}