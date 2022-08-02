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

#define BOUNCER_SIZE 80
#define GRID 4

//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
  Window win;
  ALLEGRO_BITMAP *bouncer_img, *texto_img;

  bool key[4] = { false, false, false, false },
    redraw = true, 
    sair = false;
  float bouncer_x, bouncer_y, texto_x, texto_y;
  int texto_width, texto_height;


  srand (time(NULL));
  
  win = graphinit(RES_WIDTH, RES_HEIGHT);
  
  bouncer_img = imagemArq("mysha.png", BOUNCER_SIZE, BOUNCER_SIZE, win);
  bouncer_x = rand() % (win.disp_data.width - BOUNCER_SIZE);
  bouncer_y = rand() % (win.disp_data.height - BOUNCER_SIZE);

  texto_img = imagemTexto ("Ballz", &texto_width, &texto_height, win);
  texto_x = 300;
  texto_y = 100;
 
  /* Enquanto  a tecla ESC  n�o for  pressionada, ir� executar
     o c�digo especificado.
  */
  while (!sair) {
    ALLEGRO_EVENT ev;
    al_wait_for_event(win.event_queue, &ev);
    
    switch (ev.type) {
      case ALLEGRO_EVENT_DISPLAY_CLOSE: sair = true; break;
	
      case ALLEGRO_EVENT_TIMER:
	/* Para mover imagem com teclas de Dire��o */
	if(key[KEY_UP] && bouncer_y >= GRID)
	  bouncer_y -= GRID;
 	if(key[KEY_DOWN] && bouncer_y <= win.disp_data.height - BOUNCER_SIZE - GRID)
	  bouncer_y += GRID;	
	if(key[KEY_LEFT] && bouncer_x >= GRID)
	  bouncer_x -= GRID;
	if(key[KEY_RIGHT] && bouncer_x <= win.disp_data.width - BOUNCER_SIZE - GRID)
	  bouncer_x += GRID;

        //   case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
        // image1 = image;
        // image = image2;
        // image2 = image1;
        // break;
	
	redraw = true;
	break;
	
      case ALLEGRO_EVENT_KEY_DOWN:
	switch(ev.keyboard.keycode) {
	  case ALLEGRO_KEY_UP: key[KEY_UP] = true; break;
	  case ALLEGRO_KEY_DOWN: key[KEY_DOWN] = true; break;
	  case ALLEGRO_KEY_LEFT: key[KEY_LEFT] = true; break;
	  case ALLEGRO_KEY_RIGHT: key[KEY_RIGHT] = true; break;
	}
	break;
	
      case ALLEGRO_EVENT_KEY_UP:
	switch(ev.keyboard.keycode) {
	  case ALLEGRO_KEY_UP: key[KEY_UP] = false; break;
	  case ALLEGRO_KEY_DOWN: key[KEY_DOWN] = false; break;
	  case ALLEGRO_KEY_LEFT: key[KEY_LEFT] = false; break;
	  case ALLEGRO_KEY_RIGHT: key[KEY_RIGHT] = false; break;
	  case ALLEGRO_KEY_ESCAPE: sair = true; break;
	}
        break;
	
      case ALLEGRO_EVENT_MOUSE_AXES:
      case ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY:
	bouncer_x = ev.mouse.x;
	bouncer_y = ev.mouse.y;
	break;
	
      default: break;
    }
    
    if(redraw && al_is_event_queue_empty(win.event_queue)) {
      redraw = false;
      al_clear_to_color(AZUL);
      al_draw_bitmap(bouncer_img, bouncer_x, bouncer_y, 0);
      al_draw_bitmap(texto_img, texto_x, texto_y, 0);
      al_flip_display();
    }

  }

  al_destroy_bitmap(bouncer_img);
  graphdeinit(win);
}

