#include "game.h"
#include "grafico.h"

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

#define RES_WIDTH 700
#define RES_HEIGHT 394

#define FPS 60

#define BOUNCER_SIZE 15
#define GRID 15.0

void mostra_jogo()
{
    ALLEGRO_BITMAP *ball = al_load_bitmap("utils/ball.png");
    ALLEGRO_BITMAP *cursor = al_load_bitmap("utils/cursor.png");
    ALLEGRO_BITMAP *background = al_load_bitmap("utils/bg.jpg");
    ALLEGRO_SAMPLE *som_pulo = al_load_sample("utils/sfx_wing.ogg");
    ALLEGRO_SAMPLE *batida = al_load_sample("utils/sfx_hit.ogg");
    ALLEGRO_SAMPLE *ponto = al_load_sample("utils/sfx_point.ogg");
    ALLEGRO_FONT *fonte = al_load_font("utils/flappy_font.ttf", 48, 0);
    ALLEGRO_FONT *fonte_game_over = al_load_font("utils/flappy_font.ttf", 200, 0);
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / FPS);

    ALLEGRO_EVENT_QUEUE *fila_eventos = al_create_event_queue();
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_mouse_event_source());
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));

    srand(time(NULL));

    bool sair = false, estado = false;
    int y = 100;
    float aceleracao = 0;
    double tempo_inicial, tempo_final;
    int raio = 5;
    bool game_over = false;

    float bouncer_x, bouncer_y,
        bouncer_dx = -GRID,
        bouncer_dy = GRID;

    bouncer_x = RES_WIDTH / 3.0 - BOUNCER_SIZE / 2.0;
    bouncer_y = RES_HEIGHT / 3.0 - BOUNCER_SIZE / 2.0;

    int pontuacao = 0;

    al_start_timer(timer);
    while (sair == false)
    {
        tempo_inicial = al_get_time();

        // al_clear_to_color(al_map_rgb(0, 0, 0));
        al_draw_bitmap(background, 0, 0, 0);
        al_draw_textf(fonte, al_map_rgb(0xFF, 0xFF, 0xFF), 10, 10, ALLEGRO_ALIGN_LEFT, "%i", pontuacao);
        al_draw_bitmap(ball, bouncer_x, bouncer_y, 0);

        if (game_over)
        {
            al_draw_text(fonte, al_map_rgb(0xFF, 0xFF, 0), 320, 240, ALLEGRO_ALIGN_CENTER, "Game Over");
        }

        al_flip_display();

        // y = atualizar_posicao(aceleracao, y);
        aceleracao -= 0.25;

        if (y > 480 - raio)
        {
            y = 480 - raio;
            aceleracao = 0;
            if (game_over == false)
            {
                al_play_sample(batida, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
            }
        }

        if (y < raio)
        {
            y = raio;
            aceleracao = 0;
        }

        while (!al_is_event_queue_empty(fila_eventos))
        {
            ALLEGRO_EVENT evento;
            al_wait_for_event(fila_eventos, &evento);

            if (evento.type == ALLEGRO_EVENT_KEY_DOWN)
            {
                if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                {
                    sair = true;
                }
            }
            else if (evento.type == ALLEGRO_EVENT_TIMER)
            {
                if (estado)
                {
                    if (bouncer_x < 0 || bouncer_x > RES_WIDTH - 5)
                        bouncer_dx = -bouncer_dx;

                    if (bouncer_y < 0 || bouncer_y > RES_HEIGHT - 5)
                        bouncer_dy = -bouncer_dy;

                    bouncer_x += bouncer_dx;
                    bouncer_y += bouncer_dy;
                    if (game_over == false)
                    {
                        // al_play_sample(ponto, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                        pontuacao++;
                    }
                    // if (bouncer_y > 360){
                    //     estado = false;
                    //     bouncer_x = RES_WIDTH / 2;
                    //     bouncer_y = 280;
                    // }
                }
            }
            else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
            {
                estado = true;
                if (game_over == false)
                {
                    game_over = true;
                    al_play_sample(som_pulo, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                }
            }
            else if (evento.type == ALLEGRO_EVENT_MOUSE_AXES)
            {
                bouncer_x = evento.mouse.x;
                bouncer_y = evento.mouse.y;
            }

            tempo_final = al_get_time() - tempo_inicial;
            if (tempo_final < 1.0 / FPS)
            {
                al_rest(1.0 / FPS - tempo_final);
            }
        }

        // if (game_over == false)
        // {
        //     for (i = 0; i < 4; i++)
        //     {
        //         game_over = verifica_colisao(70, y, canos[i]);

        //         if (game_over)
        //         {
        //             al_play_sample(batida, 1.0, 0.0, 1.0,
        //                            ALLEGRO_PLAYMODE_ONCE, NULL);
        //             break;
        //         }
        //     }

        //     for (i = 0; i < 4; i++)
        //     {
        //         canos[i] = atualizar_cano(canos[i]);
        //     }
        // }

        // tempo_final = al_get_time() - tempo_inicial;
        // if (tempo_final < 1.0 / FPS)
        // {
        //     al_rest(1.0 / FPS - tempo_final);
        // }
    }

    al_destroy_timer(timer);
    al_destroy_font(fonte);
    al_destroy_font(fonte_game_over);
    al_destroy_sample(ponto);
    al_destroy_sample(batida);
    al_destroy_sample(som_pulo);
    al_destroy_bitmap(ball);
    al_destroy_bitmap(background);
    al_destroy_event_queue(fila_eventos);
}