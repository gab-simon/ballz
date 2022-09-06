#ifndef __HELPER__
#define __HELPER__

#include <stdlib.h>

#define BOUNCER_RADIUS 12
#define SPEED_FACTOR 20
#define MOUSE_SENSIBILITY 2

#define ROW 8
#define COL 7
typedef enum {
   MENU,
   SETUP,
   WAITING,
   AIMING,
   SHOOTING,
   GAMEOVER,
   EXIT
 } motion_states;
struct game {
	motion_states STATES;
	int score;
	int highscore;
	int bouncers;
	int thrown_bouncers;

	float dx;
	float dy;

	float shooting_x;
	float shooting_y;
};

typedef struct bouncer bouncer_t;
struct bouncer {
	float x;
	float y;

	float dx;
	float dy;
};

typedef struct game game_t;


float calc_square_side(float dispWidth);

float calc_square_i_x(float j, float l);
float calc_square_i_y(float i, float l);
float calc_square_mid_x(float j, float l);
float calc_square_mid_y(float i, float l);
float calc_square_f_x(float j, float l);
float calc_square_f_y(float i, float l);

bouncer_t *createBouncer(float x, float y);

void setup_game(game_t *game, float dispWidth);
void setup_squares(int squares[][COL]);
void setup_bouncers(bouncer_t ***bouncers, float dispWidth, float shooting_y);
void destroy_bouncers(bouncer_t **bouncers, game_t *game);

#endif