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
#include <math.h>

#define RES_HEIGHT 800
#define RES_WIDTH 550

#define FPS 60
#define BUFFER 15

#define BOUNCER_SIZE 15
#define GRID 15.0

#define COL 7
#define ROW 9

int out = -1000;

enum {ARMING, SHOOTING, GAMEOVER, EXIT} state;

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

    sprites.block[0] = sprite_grab(1, 16, 64, 64);
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
    int wx, wy, x, y, w, h, life;
    bool show;
};
struct obj
    life = {},
    ball = {};

int colidir(int Ax, int Ay, int Aw, int Ah, int Bx, int By, int Bw, int Bh)
{
    if ((Ax + Aw >= Bx && Ax <= Bx + Bw && Ay + Ah >= By && Ay <= By + Bh) || (Ax + Aw >= Bx && Ax <= Bx + Bw && !Ay && !Ah && !By && !Bh))
        return 1;
    return 0;
}

// void blocks(int level, ALLEGRO_FONT *fonte, int row)
// {
//     int i, j;

//     if (level > 9)
//     {
//         al_draw_text(fonte, al_map_rgb(0xFF, 0xFF, 0), 320, 240, ALLEGRO_ALIGN_CENTER, "Game Over");
//     }

//     block[1][2].life = 30;

//     for (i = 0; i < 1 + level; i++)
//     {
//         for (j = 0; j < 7; j++)
//         {
//             if (block[i][j].x != out && i > 0)
//             {
//                 block[i][j].life = between(0,50);
//                 if (block[i][j].life > 30)
//                 {
//                     printf("| %d |", block[level][j].life);
//                     al_draw_bitmap(sprites.block[0], block[i][j].wx + (j * 70), block[i][j].wy + (i * 70), 0);

//                     if (colidir(block[i][j].x, block[i][j].y, 32, 32, ball.x, ball.y, 16, 16))
//                     {
//                         // if (bouncer_x + bouncer_y <= block[i][j].x || bouncer_x + 1 >= block[i][j].x + block[i][j].w)
//                         //     velx = -velx;
//                         // else
//                         //     vely = -vely;
//                         block[i][j].x = out;
//                         // pontos += 10;
//                     }
//                 }
//             }
//         }
//         printf("\n");
//     }
// }

float distancia(float ball_x, float ball_y, float mouse_x, float mouse_y)
{
    float sqr_X = mouse_x - ball_x;
    sqr_X = sqr_X * sqr_X;

    float sqr_Y = mouse_y - ball_y;
    sqr_Y = sqr_Y * sqr_Y;

    float distance = sqrt(sqr_X + sqr_Y);

    return distance;
}

float modulo(int x2, float x1, float distancia)
{
    float result = (x2 - x1) / distancia;

    return result;
}

void mostra_jogo()
{
    // ALLEGRO_BITMAP *ball = al_load_bitmap("utils/ball.png");
    ALLEGRO_BITMAP *spritess = al_load_bitmap("utils/blocks.png");
    ALLEGRO_BITMAP *background = al_load_bitmap("utils/bg_game.png");
    ALLEGRO_SAMPLE *som_pulo = al_load_sample("utils/sfx_wing.ogg");
    ALLEGRO_SAMPLE *batida = al_load_sample("utils/sfx_hit.ogg");
    ALLEGRO_SAMPLE *ponto = al_load_sample("utils/sfx_point.ogg");
    ALLEGRO_FONT *fonte = al_load_font("utils/flappy_font.ttf", 48, 0);
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / FPS);

    ALLEGRO_EVENT_QUEUE *fila_eventos = al_create_event_queue();
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_mouse_event_source());
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));

    srand(time(NULL));
    sprites_init();

    bool redraw = true;
    double tempo_inicial, tempo_final;

    float bouncer_x, bouncer_y;

    ball.x = RES_WIDTH / 2;
    ball.y = 600;

    int blocks[ROW][COL];
    bouncer_t **bouncers;

    int pontuacao = 0;
    int level = 1;
    float dist;

    al_start_timer(timer);
    while (state != EXIT)
    {
        tempo_inicial = al_get_time();

        blocks_build(blocks);
        setup_bouncers(&bouncers, RES_WIDTH, 5);

        while (!al_is_event_queue_empty(fila_eventos))
        {
            ALLEGRO_EVENT evento;
            al_wait_for_event(fila_eventos, &evento);

            switch (evento.type)
            {
            case ALLEGRO_EVENT_KEY_DOWN:
                if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                {
                   state = EXIT;
                }
                break;

            case ALLEGRO_EVENT_TIMER:
                redraw = true;
                for (int i = 0; i < 5; i++) {
						bouncers[i] = createBouncer(ball.x, ball.y);
					}


                if (state == SHOOTING)
                {
                    if (ball.x < 0 || ball.x > RES_WIDTH - 5)
                        ball.wx = -ball.wx;

                    if (ball.y < 0 || ball.y > RES_HEIGHT - 5)
                        ball.wy = -ball.wy;

                    if (ball.y > 600)
                    {
                        state = ARMING;
                        ball.wx = 0.0;
                        ball.wy = 0.0;
                        ball.y = 599;
                        level++;
                    }

                    ball.x = ball.x + ball.wx;
                    ball.y = ball.y + ball.wy;
                }
                break;

            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                if (state != SHOOTING)
                {
                    dist = distancia(ball.x, ball.y, bouncer_x, bouncer_y);
                    ball.wx = 20 * modulo(bouncer_x, ball.x, dist);
                    ball.wy = 20 * modulo(bouncer_y, ball.y, dist);
                }

                // row = between(1, 17);

                state = SHOOTING;
                al_play_sample(som_pulo, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                break;

            case ALLEGRO_EVENT_MOUSE_AXES:
                if (evento.mouse.y > 150)
                {
                    bouncer_x = evento.mouse.x;
                    bouncer_y = evento.mouse.y;
                }
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
            blocks_draw(blocks);
            al_draw_textf(fonte, al_map_rgb(0xFF, 0xFF, 0xFF), 10, 10, ALLEGRO_ALIGN_LEFT, "%i", pontuacao);
            al_draw_circle(ball.x, ball.y, 8, al_map_rgb_f(1, 0, 1), 20);
            // al_draw_bitmap(sprites.ball, ball.x, ball.y, 0);
            al_draw_text(fonte, AZUL, 250, 400, ALLEGRO_ALIGN_CENTRE, "pontuacao: ");
            if (state == ARMING)
            {
                al_draw_line(ball.x + 4, ball.y, bouncer_x, bouncer_y, al_map_rgb_f(1, 0, 0), 2);
            }
            al_flip_display();
        }
    }

    sprites_deinit();

    al_destroy_timer(timer);
    al_destroy_font(fonte);
    al_destroy_sample(ponto);
    al_destroy_sample(batida);
    al_destroy_sample(som_pulo);
    // al_destroy_bitmap(ball);
    al_destroy_bitmap(spritess);
    al_destroy_bitmap(background);
    al_destroy_event_queue(fila_eventos);
}