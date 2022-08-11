#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <time.h>
#include <string.h>

/* Graficos */
#include "grafico.h"

const int FPS = 60;

void mostra_texto()
{
}

struct squads
{
    int x;
    int y;
};

int atualizar_posicao(int aceleracao, int posicao)
{
    return posicao - aceleracao;
}

void desenha_squad(struct squads c)
{
    al_draw_filled_rectangle(c.x, 0, c.x + 80, c.y,
                             al_map_rgb(0, 0xFF, 0));

    al_draw_filled_rectangle(c.x, c.y + 120, c.x + 80, 480,
                             al_map_rgb(0, 0xFF, 0));
}

int gera_altura_cano()
{
    return rand() % 350 + 10;
}

struct squads atualizar_squad(struct squads c)
{
    c.x = c.x - 2;

    if (c.x < -80)
    {
        c.x = 640 + 20;
        c.y = gera_altura_cano();
    }

    return c;
}

// bool verifica_colisao(int x, int y, struct squads c)
// {

// }

void monta_menu(int opcao_selecionada)
{
    ALLEGRO_BITMAP *iniciar = al_load_bitmap("utils/iniciar.png");
    ALLEGRO_BITMAP *iniciar_pressed = al_load_bitmap("utils/iniciar_pressed.png");
    ALLEGRO_BITMAP *config = al_load_bitmap("utils/config.png");
    ALLEGRO_BITMAP *config_pressed = al_load_bitmap("utils/config_pressed.png");
    ALLEGRO_BITMAP *sair = al_load_bitmap("utils/sair.png");
    ALLEGRO_BITMAP *sair_pressed = al_load_bitmap("utils/sair_pressed.png");

    // Primeira opção do menu (iniciar)
    if (opcao_selecionada == 1)
    {
        al_draw_bitmap(iniciar_pressed, 350 - al_get_bitmap_width(iniciar_pressed) / 2,
                       240 - 10 -
                           al_get_bitmap_height(iniciar_pressed) -
                           al_get_bitmap_height(config) / 2,
                       0);
    }
    else
    {
        al_draw_bitmap(iniciar, 350 - al_get_bitmap_width(iniciar) / 2,
                       240 - 10 -
                           al_get_bitmap_height(iniciar) -
                           al_get_bitmap_height(config) / 2,
                       0);
    }

    // Segundo botão do menu (config)
    if (opcao_selecionada == 2)
    {
        al_draw_bitmap(config_pressed, 350 - al_get_bitmap_width(config_pressed) / 2,
                       240 - al_get_bitmap_height(config_pressed) / 2,
                       0);
    }
    else
    {
        al_draw_bitmap(config, 350 - al_get_bitmap_width(config) / 2,
                       240 - al_get_bitmap_height(config) / 2,
                       0);
    }

    // Terceiro botão do menu (sair)
    if (opcao_selecionada == 3)
    {
        al_draw_bitmap(sair_pressed, 350 - al_get_bitmap_width(sair_pressed) / 2,
                       240 + 10 + al_get_bitmap_height(config) / 2,
                       0);
    }
    else
    {
        al_draw_bitmap(sair, 350 - al_get_bitmap_width(sair_pressed) / 2,
                       240 + 10 + al_get_bitmap_height(config) / 2,
                       0);
    }

    al_destroy_bitmap(config);
    al_destroy_bitmap(config_pressed);
    al_destroy_bitmap(iniciar);
    al_destroy_bitmap(iniciar_pressed);
    al_destroy_bitmap(sair);
    al_destroy_bitmap(sair_pressed);
}

int main()
{
    // Declara a janela do aplicativo
    ALLEGRO_DISPLAY *janela;
    // Variável para controlar os eventos
    ALLEGRO_EVENT_QUEUE *fila_eventos;
    // Declara a imagem de fundo
    ALLEGRO_BITMAP *fundo;
    // Variável que representa a música de fundo
    ALLEGRO_AUDIO_STREAM *musica;

    // Inicializa a biblioteca
    al_init();
    // Inicializa o módulo de imagens
    al_init_image_addon();
    // Inicializando o teclado
    al_install_keyboard();
    // Inicializando o mouse
    al_install_mouse();
    // Inicializar o áudio
    al_install_audio();
    // Inicializar a parte de codecs de áudio
    al_init_acodec_addon();
    // Reservar samples
    al_reserve_samples(4);
    // Inicializar o módulo de primitivas gráficas
    al_init_primitives_addon();
    // Inicializar as bibliotecas de fonte
    al_init_font_addon();
    // al_init_ttf_addon();

    // Cria a fila de eventos
    fila_eventos = al_create_event_queue();
    // Carregar a imagem de fundo
    fundo = al_load_bitmap("utils/bg.jpg");

    // Cria uma janela de 700x394
    janela = al_create_display(700, 394);
    al_set_window_title(janela, "Ballz - ProgII");
    // Pinta a tela de vermelho
    // al_clear_to_color(al_map_rgb(0xFF, 0x00, 0x00));

    // Desenhar a imagem na tela
    al_draw_bitmap(fundo, 0, 0, 0);

    // Aplica as alterações na tela
    al_flip_display();

    // Carregar a música
    musica = al_load_audio_stream("utils/music.ogg", 4, 1024);
    al_attach_audio_stream_to_mixer(musica, al_get_default_mixer());

    // A fila de eventos receberá os eventos da janela
    al_register_event_source(fila_eventos, al_get_display_event_source(janela));
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());

    // Música
    al_set_audio_stream_playing(musica, true);

    // Espera 5 segundos
    // al_rest(5.0);
    int opcao_menu = 1;
    bool sair = false;
    while (sair == false)
    {
        al_draw_bitmap(fundo, 0, 0, 0);
        monta_menu(opcao_menu);
        al_flip_display();

        while (!al_is_event_queue_empty(fila_eventos))
        {
            ALLEGRO_EVENT evento;
            al_wait_for_event(fila_eventos, &evento);

            if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            {
                sair = true;
            }
            else if (evento.type == ALLEGRO_EVENT_KEY_DOWN)
            {
                if (evento.keyboard.keycode ==
                    ALLEGRO_KEY_ESCAPE)
                {
                    sair = true;
                }
                else if (evento.keyboard.keycode ==
                         ALLEGRO_KEY_DOWN)
                {
                    opcao_menu++;
                    if (opcao_menu > 3)
                    {
                        opcao_menu = 1;
                    }
                }
                else if (evento.keyboard.keycode ==
                         ALLEGRO_KEY_UP)
                {
                    opcao_menu--;
                    if (opcao_menu < 1)
                    {
                        opcao_menu = 3;
                    }
                }
                else if (evento.keyboard.keycode ==
                         ALLEGRO_KEY_ENTER)
                {
                    if (opcao_menu == 3)
                    {
                        sair = true;
                    }
                    else
                    {
                        al_set_audio_stream_playing(musica, false);
                        al_unregister_event_source(
                            fila_eventos,
                            al_get_keyboard_event_source());

                        if (opcao_menu == 1)
                        {
                            mostra_jogo();
                        }
                        else if (opcao_menu == 2)
                        {
                            mostra_texto();
                        }

                        al_register_event_source(
                            fila_eventos,
                            al_get_keyboard_event_source());
                        al_rewind_audio_stream(musica);
                        al_set_audio_stream_playing(musica, true);
                    }
                }
            }
        }
    }

    // Música, pode parar!
    al_set_audio_stream_playing(musica, false);

    // Libera a memória usada pela música
    al_destroy_audio_stream(musica);
    // Libera a memória usada pela imagem de fundo
    al_destroy_bitmap(fundo);
    // Libera a memória usada pela janela
    al_destroy_display(janela);

    return 0;
}
