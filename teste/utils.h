#ifndef __HELPER__
#define __HELPER__

#include <stdlib.h>

#define BALL_RADIUS 12
#define SPEED_FACTOR 20
#define MOUSE_SENSIBILITY 2

#define ROW 8
#define COL 7

typedef enum {
   MENU,
   INFO,
   SERVING,
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
	int balls;
	int thrown_balls;
	float dx;
	float dy;
	float shooting_x;
	float shooting_y;
};
typedef struct game game_t;

typedef struct ball ball_t;
struct ball {
	float x;
	float y;
	float dx;
	float dy;
};



float block_side(float display_width);
float block_i_x(float j, float l);
float block_i_y(float i, float l);
float block_mid_x(float j, float l);
float block_mid_y(float i, float l);
float block_f_x(float j, float l);
float block_f_y(float i, float l);
ball_t *createball(float x, float y);
void setup_game(game_t *game, float display_width);
void setup_blocks(int blocks[][COL]);
void setup_balls(ball_t ***balls, float display_width, float shooting_y);
void destroy_balls(ball_t **balls, game_t *game);

#endif