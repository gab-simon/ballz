#include "utils.h"
#include <stddef.h>

int display_matrix_ji(float xi, float l)
{
	return (xi / (1.1 * l)) - 1 / 11;
}
int display_matrix_i(float yi, float l)
{
	return yi / (1.1 * l);
}
int display_matrix_jf(float xf, float l)
{
	return (xf / (1.1 * l)) - 1;
}
int display_matrix_f(float yf, float l)
{
	return (yf / (1.1 * l)) - 10 / 11;
}

float block_side(float display_width)
{
	return display_width / 7.8;
}
float block_i_x(float j, float l)
{
	return (j * 1.1 * l) + 0.1 * l;
}
float block_i_y(float i, float l)
{
	return i * 1.1 * l;
}
float block_mid_x(float j, float l)
{
	return (block_i_x(j, l) + block_f_x(j, l)) / 2;
}
float block_mid_y(float i, float l)
{
	return (block_i_y(i, l) + block_f_y(i, l)) / 2;
}
float block_f_x(float j, float l)
{
	return (j + 1) * 1.1 * l;
}
float block_f_y(float i, float l)
{
	return i * 1.1 * l + l;
}

ball_t *buildBall(float x, float y)
{
	ball_t *ball = malloc(sizeof(ball_t));
	ball->dx = 0;
	ball->dy = 0;
	ball->x = x;
	ball->y = y;

	return ball;
}

void build_game(game_t *game, float display_width)
{
	game->score = 0;
	game->balls = 1;
	game->shot_balls = 0;
	game->dx = 0;
	game->dy = 0;
	game->shooting_x = (display_width - BALL_RADIUS) / 2.0;
}

void build_blocks(int blocks[][COL])
{
	for (int i = 0; i < ROW; ++i)
		for (int j = 0; j < COL; ++j)
			blocks[i][j] = 0;
}

void build_balls(ball_t ***balls, float display_width, float shooting_y)
{
	*balls = calloc(sizeof(ball_t), 1);
	*balls[0] = buildBall(display_width * 0.5, shooting_y);
}

void destroy_balls(ball_t **balls, game_t *game)
{
	if (balls != NULL)
	{
		for (int i = 0; i < game->balls; ++i)
		{
			if (balls[i] != NULL)
				free(balls[i]);
		}
		free(balls);
	}
}