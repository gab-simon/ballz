#include "game_helpers.h"
#include <stddef.h>

float calc_square_side(float dispWidth) {
    return dispWidth / 7.8;
}

float calc_square_i_x(float j, float l) {
    return (j * 1.1*l) + 0.1*l;
}

float calc_square_i_y(float i, float l) {
    return i * 1.1*l;
}

float calc_square_mid_x(float j, float l) {
    return (calc_square_i_x(j, l) + calc_square_f_x(j, l))/2;
};

float calc_square_mid_y(float i, float l) {
    return (calc_square_i_y(i, l) + calc_square_f_y(i, l))/2;
}

float calc_square_f_x(float j, float l) {
    return (j+1) * 1.1*l;
}

float calc_square_f_y(float i, float l) {
    return i * 1.1*l + l;
}

int calcjMatrixPositioni(float xi, float l) {
    return (xi/(1.1*l)) - 1/11;
}

int calciMatrixPositioni(float yi, float l) {
    return yi/(1.1*l);
}

int calcjMatrixPositionf(float xf, float l) {
    return (xf/(1.1*l)) - 1;
}

int calciMatrixPositionf(float yf, float l) {
    return (yf/(1.1*l)) - 10/11;
}


bouncer_t *createBouncer(float x, float y) {
	bouncer_t *bouncer = malloc(sizeof(bouncer_t));
	bouncer->dx = 0;
	bouncer->dy = 0;
	bouncer->x = x;
	bouncer->y = y;

	return bouncer;
}

void setup_game(game_t *game, float dispWidth) {
	game->score = 0;
	game->bouncers = 1;
    game->thrown_bouncers = 0;
	game->dx = 0;
	game->dy = 0;
	game->shooting_x = (dispWidth - BOUNCER_RADIUS) / 2.0;
}

void setup_squares(int squares[][COL]) {
	for (int i = 0; i < ROW; ++i) {
		for (int j = 0; j < COL; ++j) {
			squares[i][j] = 0;
		}
	}
}

void setup_bouncers(bouncer_t ***bouncers, float dispWidth, float shooting_y) {
	*bouncers = calloc(sizeof(bouncer_t), 1);
	*bouncers[0] = createBouncer(dispWidth * 0.5, shooting_y);
}

void destroy_bouncers(bouncer_t **bouncers, game_t *game) {
	if(bouncers != NULL) {
		for(int i = 0; i < game->bouncers; ++i) {
			if(bouncers[i] != NULL) {
				free(bouncers[i]);
			}
		}
		free(bouncers);
	}
	
}