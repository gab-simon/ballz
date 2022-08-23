/* Allegro */
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <time.h>
#include <string.h>

/* Inicializa */
void mostra_jogo();
float distancia(float ball_x, float ball_y, float mouse_x, float mouse_y);
float modulo(int x2, float x1, float distancia);