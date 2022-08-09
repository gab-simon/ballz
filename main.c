#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Allegro */
#include <allegro5/allegro.h>
#include "allegro5/allegro_image.h"
#include "allegro5/allegro_native_dialog.h"

/* Graficos */
#include "grafico.h"

#define RES_WIDTH 800
#define RES_HEIGHT 600

#define BUFFER_W 320
#define BUFFER_H 240

#define BOUNCER_SIZE 80
#define GRID 4

long frames;
long score;

void must_init(bool test, const char *description)
{
    if(test) return;

    printf("couldn't initialize %s\n", description);
    exit(1);
}

// --- hud ---

ALLEGRO_FONT *font;
long score_display;

void hud_init()
{
    font = al_create_builtin_font();
    must_init(font, "font");

    score_display = 0;
}

void hud_deinit()
{
    al_destroy_font(font);
}

void hud_update()
{
    if (frames % 2)
        return;

    for (long i = 5; i > 0; i--)
    {
        long diff = 1 << i;
        if (score_display <= (score - diff))
            score_display += diff;
    }
}

void hud_draw()
{

    al_draw_textf(
        font,
        al_map_rgb_f(1, 1, 1),
        1, 1,
        0,
        "%06ld",
        score_display);

    // int spacing = LIFE_W + 1;
    // for(int i = 0; i < ship.lives; i++)
    //     al_draw_bitmap(sprites.life, 1 + (i * spacing), 10, 0);

    // if(ship.lives < 0)
    al_draw_text(
        font,
        al_map_rgb_f(1, 1, 1),
        BUFFER_W / 2, BUFFER_H / 2,
        ALLEGRO_ALIGN_CENTER,
        "G A M E  O V E R");
}

//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    Window win;
    ALLEGRO_BITMAP *bouncer_img, *texto_img;

    bool key[4] = {false, false, false, false},
         redraw = true,
         sair = false;
    float bouncer_x, bouncer_y, texto_x, texto_y;
    int texto_width, texto_height;

    srand(time(NULL));

    win = graphinit(RES_WIDTH, RES_HEIGHT);

    bouncer_img = imagemArq("mysha.png", BOUNCER_SIZE, BOUNCER_SIZE, win);
    bouncer_x = rand() % (win.disp_data.width - BOUNCER_SIZE);
    bouncer_y = rand() % (win.disp_data.height - BOUNCER_SIZE);

    texto_img = imagemTexto("Ballz", &texto_width, &texto_height, win);
    texto_x = 300;
    texto_y = 100;

    /* Enquanto  a tecla ESC  n�o for  pressionada, ir� executar
       o c�digo especificado.
    */
    while (!sair)
    {
        ALLEGRO_EVENT ev;
        al_wait_for_event(win.event_queue, &ev);

        hud_init();

        switch (ev.type)
        {
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            sair = true;
            break;

        case ALLEGRO_EVENT_TIMER:
            hud_update();
            /* Para mover imagem com teclas de Dire��o */
            if (key[KEY_UP] && bouncer_y >= GRID)
                bouncer_y -= GRID;
            if (key[KEY_DOWN] && bouncer_y <= win.disp_data.height - BOUNCER_SIZE - GRID)
                bouncer_y += GRID;
            if (key[KEY_LEFT] && bouncer_x >= GRID)
                bouncer_x -= GRID;
            if (key[KEY_RIGHT] && bouncer_x <= win.disp_data.width - BOUNCER_SIZE - GRID)
                bouncer_x += GRID;

            //   case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
            // image1 = image;
            // image = image2;
            // image2 = image1;
            // break;

            redraw = true;
            break;

        case ALLEGRO_EVENT_KEY_DOWN:
            switch (ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_UP:
                key[KEY_UP] = true;
                break;
            case ALLEGRO_KEY_DOWN:
                key[KEY_DOWN] = true;
                break;
            case ALLEGRO_KEY_LEFT:
                key[KEY_LEFT] = true;
                break;
            case ALLEGRO_KEY_RIGHT:
                key[KEY_RIGHT] = true;
                break;
            }
            break;

        case ALLEGRO_EVENT_KEY_UP:
            switch (ev.keyboard.keycode)
            {
            case ALLEGRO_KEY_UP:
                key[KEY_UP] = false;
                break;
            case ALLEGRO_KEY_DOWN:
                key[KEY_DOWN] = false;
                break;
            case ALLEGRO_KEY_LEFT:
                key[KEY_LEFT] = false;
                break;
            case ALLEGRO_KEY_RIGHT:
                key[KEY_RIGHT] = false;
                break;
            case ALLEGRO_KEY_H:
                al_clear_to_color(VERDE);
                printf("abrir janela grafica dps... ");
                break;
            case ALLEGRO_KEY_ESCAPE:
                sair = true;
                break;
            }
            break;

        case ALLEGRO_EVENT_MOUSE_AXES:
        case ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY:
            bouncer_x = ev.mouse.x;
            bouncer_y = ev.mouse.y;
            break;

        default:
            break;
        }

        if (redraw && al_is_event_queue_empty(win.event_queue))
        {
            redraw = false;
            al_clear_to_color(CINZA_ARDOSIA);
            al_draw_bitmap(bouncer_img, bouncer_x, bouncer_y, 0);
            al_draw_bitmap(texto_img, texto_x, texto_y, 0);
            al_flip_display();
            al_draw_rectangle(101, 101, 300, 175, al_map_rgb(0, 0, 0), 1);

            hud_draw();
        }
    }

    al_destroy_bitmap(bouncer_img);
    hud_deinit();
    graphdeinit(win);
}
