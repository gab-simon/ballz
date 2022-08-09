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

    // y = atualizar_posicao(aceleracao, y);
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
