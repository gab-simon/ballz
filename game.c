#include "grafico.h"

void mostra_jogo()
{
    ALLEGRO_BITMAP *fundo =
        al_load_bitmap("utils/bg.jpg");
    ALLEGRO_SAMPLE *som_pulo =
        al_load_sample("utils/sfx_wing.ogg");
    ALLEGRO_SAMPLE *batida =
        al_load_sample("utils/sfx_hit.ogg");
    ALLEGRO_SAMPLE *ponto =
        al_load_sample("utils/sfx_point.ogg");
    ALLEGRO_FONT *fonte =
        al_load_font("utils/flappy_font.ttf", 48, 0);
    ALLEGRO_FONT *fonte_game_over =
        al_load_font("utils/flappy_font.ttf", 200, 0);
    ALLEGRO_TIMER *timer = al_create_timer(1.0);

    ALLEGRO_EVENT_QUEUE *fila_eventos = al_create_event_queue();
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_mouse_event_source());
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));

    srand(time(NULL));

    int y = 240;
    float aceleracao = 0;
    double tempo_inicial, tempo_final;
    bool sair = false;
    int raio = 20;
    bool game_over = false;

    // inicialização dos canos

    int pontuacao = 0;
    al_start_timer(timer);
    while (sair == false)
    {
        tempo_inicial = al_get_time();

        // al_clear_to_color(al_map_rgb(0, 0, 0));
        al_draw_bitmap(fundo, 0, 0, 0);


        al_draw_textf(fonte, al_map_rgb(0xFF, 0xFF, 0xFF),
                      10, 10, ALLEGRO_ALIGN_LEFT, "%i", pontuacao);

        al_draw_filled_circle(70, y, raio,
                              al_map_rgb(0xFF, 0, 0));

        if (game_over)
        {
            al_draw_text(fonte, al_map_rgb(0xFF, 0xFF, 0),
                         320, 240, ALLEGRO_ALIGN_CENTER, "Game Over");
        }

        al_flip_display();

        y = atualizar_posicao(aceleracao, y);
        aceleracao -= 0.25;

        if (y > 480 - raio)
        {
            y = 480 - raio;
            aceleracao = 0;
            if (game_over == false)
            {
                game_over = true;
                al_play_sample(batida, 1.0, 0.0, 1.0,
                               ALLEGRO_PLAYMODE_ONCE, NULL);
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
                if (evento.keyboard.keycode ==
                    ALLEGRO_KEY_ESCAPE)
                {
                    sair = true;
                }
            }
            else if (evento.type ==
                     ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
            {
                if (game_over == false)
                {
                    aceleracao = 5;
                    al_play_sample(som_pulo, 1.0, 0.0, 1.0,
                                   ALLEGRO_PLAYMODE_ONCE, NULL);
                }
            }
            else if (evento.type ==
                     ALLEGRO_EVENT_TIMER)
            {
                if (game_over == false)
                {
                    al_play_sample(ponto, 1.0, 0.0, 1.0,
                                   ALLEGRO_PLAYMODE_ONCE, NULL);
                    pontuacao++;
                }
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
    al_destroy_bitmap(fundo);
    al_destroy_event_queue(fila_eventos);
}