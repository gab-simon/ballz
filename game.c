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

#define RES_HEIGHT 1000
#define RES_WIDTH 500

#define FPS 60
#define BUFFER 15

#define BOUNCER_SIZE 15
#define GRID 15.0

int out = -500;

void must_init(bool test, const char *description)
{
    if (test)
        return;

    printf("couldn't initialize %s\n", description);
    exit(1);
}

// randomização
int between(int lo, int hi)
{
    return lo + (rand() % (hi - lo));
}

typedef struct SPRITES
{
    ALLEGRO_BITMAP *_sheet;

    ALLEGRO_BITMAP *ball;
    ALLEGRO_BITMAP *life;

    ALLEGRO_BITMAP *block[2];
} SPRITES;
SPRITES sprites;

ALLEGRO_BITMAP *sprite_grab(int x, int y, int w, int h)
{
    ALLEGRO_BITMAP *sprite = al_create_sub_bitmap(sprites._sheet, x, y, w, h);
    must_init(sprite, "sprite grab");
    return sprite;
}

void sprites_init()
{
    sprites._sheet = al_load_bitmap("utils/spritesheet.png");
    must_init(sprites._sheet, "spritesheet");

    sprites.ball = sprite_grab(1, 4, 16, 16);

    sprites.life = sprite_grab(0, 14, 6, 6);

    sprites.block[0] = sprite_grab(1, 5, 16, 16);
    sprites.block[1] = sprite_grab(8, 18, 23, 55);
}

void sprites_deinit()
{
    al_destroy_bitmap(sprites.ball);

    al_destroy_bitmap(sprites.life);

    al_destroy_bitmap(sprites.block[0]);
    al_destroy_bitmap(sprites.block[1]);

    al_destroy_bitmap(sprites._sheet);
}

// Blocks
struct obj
{
    int wx, wy, x, y, w, h;
};
struct obj
    block[10][17],
    life = {},
    ball = {};

int colidir(int Ax, int Ay, int Aw, int Ah, int Bx, int By, int Bw, int Bh)
{
    if ((Ax + Aw >= Bx && Ax <= Bx + Bw && Ay + Ah >= By && Ay <= By + Bh) || (Ax + Aw >= Bx && Ax <= Bx + Bw && !Ay && !Ah && !By && !Bh))
        return 1;
    return 0;
}

void blocks()
{
    int i, j, draw;

    for (i = 0; i < 10; i++)
    {
        for (j = 0; j < 17; j++)
        {
            if (block[i][j].x != out)
            {
                draw = between(0, 2);
                al_draw_bitmap(sprites.block[0], block[i][j].wx + (j * 32), block[i][j].wy + (i * 32), 0);
                if (colidir(block[i][j].x, block[i][j].y, block[i][j].w, block[i][j].h, ball.x, ball.y, ball.h, ball.w))
                {
                    // if (bouncer_x + bouncer_y <= block[i][j].x || bouncer_x + 1 >= block[i][j].x + block[i][j].w)
                    //     velx = -velx;
                    // else
                    //     vely = -vely;
                    block[i][j].x = out;
                    // pontos += 10;
                }
            }
        }
    }
}

void mostra_jogo()
{
    // ALLEGRO_BITMAP *ball = al_load_bitmap("utils/ball.png");
    ALLEGRO_BITMAP *spritess = al_load_bitmap("utils/blocks.png");
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
    sprites_init();

    bool sair = false, estado = false, redraw = true;
    double tempo_inicial, tempo_final;
    bool game_over = false;

    float bouncer_x, bouncer_y,
        bouncer_dx = -GRID,
        bouncer_dy = GRID;

    ball.x = RES_WIDTH / 3.0 - BOUNCER_SIZE / 2.0;
    ball.y = RES_HEIGHT / 3.0 - BOUNCER_SIZE / 2.0;

    int pontuacao = 0;

    al_start_timer(timer);
    while (sair == false)
    {
        tempo_inicial = al_get_time();

        // al_clear_to_color(al_map_rgb(0, 0, 0));

        // if (game_over)
        // {
        //     al_draw_text(fonte, al_map_rgb(0xFF, 0xFF, 0), 320, 240, ALLEGRO_ALIGN_CENTER, "Game Over");
        // }

        while (!al_is_event_queue_empty(fila_eventos))
        {
            ALLEGRO_EVENT evento;
            al_wait_for_event(fila_eventos, &evento);

            switch (evento.type)
            {
            case ALLEGRO_EVENT_KEY_DOWN:
                if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                {
                    sair = true;
                }
                break;

            case ALLEGRO_EVENT_TIMER:

                if (estado)
                {
                    if (ball.x < 0 || ball.x > RES_WIDTH - 5)
                        bouncer_dx = -bouncer_dx;

                    if (ball.y < 0 || ball.y > RES_HEIGHT - 5)
                        bouncer_dy = -bouncer_dy;

                    ball.x += bouncer_dx;
                    ball.y += bouncer_dy;
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
                redraw = true;
                break;

            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                estado = true;
                if (game_over == false)
                {
                    game_over = true;
                    al_play_sample(som_pulo, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                }
                break;

            case ALLEGRO_EVENT_MOUSE_AXES:
                ball.x = evento.mouse.x;
                ball.y = evento.mouse.y;
                break;
            }

            tempo_final = al_get_time() - tempo_inicial;
            if (tempo_final < 1.0 / FPS)
            {
                al_rest(1.0 / FPS - tempo_final);
            }
        }

        if (redraw && al_is_event_queue_empty(fila_eventos))
        {
            redraw = false;
            al_draw_bitmap(background, 0, 0, 0);
            blocks();
            al_draw_textf(fonte, al_map_rgb(0xFF, 0xFF, 0xFF), 10, 10, ALLEGRO_ALIGN_LEFT, "%i", pontuacao);
            al_draw_bitmap(sprites.ball, ball.x, ball.y, 0);
            al_flip_display();
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
    sprites_deinit();

    al_destroy_timer(timer);
    al_destroy_font(fonte);
    al_destroy_font(fonte_game_over);
    al_destroy_sample(ponto);
    al_destroy_sample(batida);
    al_destroy_sample(som_pulo);
    // al_destroy_bitmap(ball);
    al_destroy_bitmap(spritess);
    al_destroy_bitmap(background);
    al_destroy_event_queue(fila_eventos);
}