#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Allegro */
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>

#include "grafico.h"

#define COL 7
#define ROW 9

Window graphinit(int res_width, int res_height)
{
    int i;
    Window win = {NULL, NULL, NULL, {0, 0, 0, 0}};

    /* Inicializa a Allegro */
    al_init();
    al_init_image_addon();
    al_init_native_dialog_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    /* Define Janela */
#ifdef __DEBUG__
    puts("Modos display a 60Hz disponíveis:\n");
#endif

    for (i = 0; i < al_get_num_display_modes(); ++i)
    {
        al_get_display_mode(i, &win.disp_data);

        if (win.disp_data.refresh_rate == 60)
        { // 60 Hz
#ifdef __DEBUG__
            printf("(%d): %d %d\n", i, win.disp_data.width, win.disp_data.height);
#endif
            if (win.disp_data.width == res_width && win.disp_data.height == res_height)
                break;
        }
    }

#ifdef __DEBUG__
    printf("\n\tModo selecionado --> (%d): %d %d\n", i, win.disp_data.width, win.disp_data.height);
#endif

    al_get_display_mode(i, &win.disp_data);
    al_set_new_display_flags(ALLEGRO_WINDOWED); // | ALLEGRO_RESIZABLE);
                                                // ALLEGRO_FULLSCREEN
                                                // ALLEGRO_FRAMELESS
    win.display = al_create_display(win.disp_data.width, win.disp_data.height);

    /* Instala o handler de mouse do Allegro */
    al_install_mouse();
    /* Instala o handler de teclado do Allegro */
    al_install_keyboard();
    /* Instala o handler de timer do Allegro */
    win.timer = al_create_timer(1.0 / win.disp_data.refresh_rate);

    /* Define Eventos */
    win.event_queue = al_create_event_queue();

    al_register_event_source(win.event_queue, al_get_display_event_source(win.display));
    al_register_event_source(win.event_queue, al_get_mouse_event_source());
    al_register_event_source(win.event_queue, al_get_keyboard_event_source());
    al_register_event_source(win.event_queue, al_get_timer_event_source(win.timer));

    al_clear_to_color(AZUL);
    al_flip_display();
    al_start_timer(win.timer);

    return (win);
}

void graphdeinit(Window win)
{
    al_destroy_timer(win.timer);
    al_destroy_event_queue(win.event_queue);
    al_destroy_display(win.display);
}

ALLEGRO_BITMAP *imagemArq(char *nomeArqImg, int width, int height, Window win)
{
    ALLEGRO_BITMAP *bmp, *img;

    bmp = al_create_bitmap(width, height);
    al_set_target_bitmap(bmp);
    al_clear_to_color(NEVE);

    if (nomeArqImg)
    {
        img = al_load_bitmap(nomeArqImg);
        al_draw_bitmap(img, 0, 0, 0);
    }

    al_set_target_bitmap(al_get_backbuffer(win.display));

    return bmp;
}

/* Cria regi�o em display para exibir Texto em ambiente ALLEGRO */
/* */
ALLEGRO_BITMAP *imagemTexto(char *texto, int *width, int *height, Window win)
{
    ALLEGRO_BITMAP *bmp;
    ALLEGRO_FONT *font;

    font = al_load_ttf_font("/usr/share/fonts/truetype/freefont/FreeSans.ttf", 24, 0);
    *width = al_get_text_width(font, texto) + 50;
    *height = al_get_font_line_height(font) + 50;

    bmp = al_create_bitmap(*width, *height);

    al_set_target_bitmap(bmp);
    al_clear_to_color(SALMAO);

    if (!font)
    {
        printf("\nErro na fonte de texto\n");
        exit(1);
    }

    al_draw_text(font, AZUL, *width / 2.0, *height / 2.0, ALLEGRO_ALIGN_CENTRE, texto);

    al_set_target_bitmap(al_get_backbuffer(win.display));

    return bmp;
}

bouncer_t *createBouncer(float x, float y) {
	bouncer_t *bouncer = malloc(sizeof(bouncer_t));
	bouncer->dx = 0;
	bouncer->dy = 0;
	bouncer->x = x;
	bouncer->y = y;

	return bouncer;
}

void blocks_build(int blocks[][COL]) {
	for (int i = 0; i < ROW; ++i) {
		for (int j = 0; j < COL; ++j) {
			blocks[i][j] = 2;
		}
	}
}

void blocks_draw(int blocks[][7]) {
	int i, j;
    ALLEGRO_FONT *fonte = al_load_font("utils/flappy_font.ttf", 48, 0);

    // al_draw_filled_rectangle(0, 0, 64, 64, al_map_rgb(250,127,114));
    // al_draw_filled_rectangle(64 + 5, 0, 128, 64, al_map_rgb(250,127,114));
    // al_draw_filled_rectangle(128 + 5, 0, 192, 64, al_map_rgb(250,127,114));

	for (i = 0; i < ROW; ++i) {
		for (j = 0; j < COL; ++j) {
			if (blocks[i][j] > 0) {
				al_draw_filled_rectangle((64 * j) + 5, (64 * i) + 5, (64 * j) + 64, (64 * i) + 64, al_map_rgb(250,127,114));
				char text[10];
				int textOffset = al_get_font_line_height(fonte)/2;
				sprintf(text, "%d", blocks[i][j]);
				al_draw_text(fonte, BRANCO_ANTIGO, 64 * j, 64 * i, ALLEGRO_ALIGN_CENTER, text);
			}
			if (blocks[i][j] == -1) {
				al_draw_filled_circle(64 * j, 64 * i + 5, 12, BRANCO);
				al_draw_circle(64 * j, 64 * i + 5, 12 + 10, BRANCO, 5);
			}
		}
	}
}

void setup_bouncers(bouncer_t ***bouncers, float dispWidth, float shooting_y) {
	*bouncers = calloc(sizeof(bouncer_t), 1);
	*bouncers[0] = createBouncer(dispWidth * 0.5, shooting_y);
}