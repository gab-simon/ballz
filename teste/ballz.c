#include <string.h>
#include <stdio.h>
#include <math.h>

/* Allegro */
#include <allegro5/allegro.h>
#include "allegro5/allegro_primitives.h"
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "allegro5/allegro_native_dialog.h"
#include "allegro5/allegro_font.h"
#include "allegro5/allegro_ttf.h"

/* Graficos */
#include "grafico.h"

#define RES_HEIGHT 800
#define RES_WIDTH 600

int main(int argc, char *argv[])
{
	window win = graph_init(RES_WIDTH, RES_HEIGHT);
	srand(time(NULL));

	int i, j;

	bool menu_drew = false;
	bool can_shoot = false;
	bool new_blocks = false;
	bool new_shots = false;

	int launch_interval = 0;
	int launch_index = 0;
	int arrival_counter = 0;
	int new_bouncers_count = 0;

	float offset = 0;

	ALLEGRO_MOUSE_EVENT mouse_down;

	float square_side = RES_WIDTH / 7.8;

	game_t game;
	int squares[ROW][COL];
	bouncer_t **bouncers;
	game.shooting_y = calc_square_f_y(8, square_side) - BOUNCER_RADIUS;

	FILE *score_file;
	score_file = fopen(".highscore", "r");
	if (score_file == NULL)
	{
		score_file = fopen(".highscore", "w");
		game.highscore = 0;
	}
	else
	{
		fscanf(score_file, "%d", &game.highscore);
	}
	fclose(score_file);

	game.STATES = MENU;

	while (game.STATES != EXIT)
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(win.event_queue, &ev);

		int dist_x;
		int dist_y;
		float dist;

		switch (game.STATES)
		{
		case MENU:
			if (!menu_drew)
			{
				draw_menu(&win);
				menu_drew = true;
			}

			if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
			{
				if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
					game.STATES = EXIT;
			}
			if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
			{
				if (ev.mouse.x > RES_WIDTH * 0.5 - 160 && ev.mouse.x < RES_WIDTH * 0.5 + 160 && ev.mouse.y > win.disp_data.height * 0.6 && ev.mouse.y < win.disp_data.height * 0.6 + 80)
				{
					game.STATES = SETUP;
					setup_game(&game, RES_WIDTH);

					setup_squares(squares);

					setup_bouncers(&bouncers, RES_WIDTH, game.shooting_y);

					menu_drew = false;
					can_shoot = false;
					new_blocks = false;
					new_shots = false;

					launch_interval = 0;
					launch_index = 0;
					arrival_counter = 0;
					new_bouncers_count = 0;

					offset = 0;
				}
			}
			break;

		case SETUP:
			if (!new_blocks)
			{
				game.score++;
				if (game.score >= game.highscore)
				{
					game.highscore = game.score;
				}
				for (j = 0; j < COL; j++)
				{
					squares[0][j] = (rand() % 3) * game.score;
				}
				if (game.score > 1)
				{
					int newBallPosition = rand() % COL;
					squares[0][newBallPosition] = -1;
				}
				new_blocks = true;
			}
			if (ev.type == ALLEGRO_EVENT_TIMER)
			{
				if (offset < (1.1 * square_side))
				{
					draw_setup(&win, bouncers[0], squares, offset, &game);
					offset += 2;
				}
				else
				{
					offset = 0;
					for (j = 0; j < COL; j++)
					{
						if (squares[ROW - 1][j] > 0)
						{
							game.STATES = GAMEOVER;
						}
					}
					if (game.STATES != GAMEOVER)
					{
						for (i = ROW - 1; i > 0; i--)
						{
							for (j = 0; j < COL; j++)
							{
								squares[i][j] = squares[i - 1][j];
							}
						}
						for (j = 0; j < COL; j++)
						{
							squares[0][j] = 0;
						};
						game.STATES = WAITING;
						draw_wait(&win, bouncers[0], squares, &game);
					}
					else
					{
						destroy_bouncers(bouncers, &game);
						if (game.score >= game.highscore)
						{
							score_file = fopen(".highscore", "w");
							fprintf(score_file, "%d", game.score);
							fclose(score_file);
						}
						draw_gameover(&win, &game);
					};
					new_blocks = false;
				};
			}
			break;

		case WAITING:
			if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
			{
				if (ev.keyboard.keycode == ALLEGRO_KEY_H)
				{
					al_show_native_message_box(win.display, "Instruções", "TEste textoo textooo uma um texto aqui um textooo", "textooo", NULL, NULL);
				}
			}

			if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
			{
				mouse_down = ev.mouse;

				game.STATES = AIMING;
			}
			break;

		case AIMING:
			dist_x = (mouse_down.x - ev.mouse.x);
			dist_y = (mouse_down.y - ev.mouse.y);
			dist = sqrt(pow(dist_x, 2) + pow(dist_y, 2));

			if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
			{
				if (can_shoot)
				{
					game.dx = SPEED_FACTOR * (dist_x) / dist;
					game.dy = SPEED_FACTOR * (dist_y) / dist;

					game.STATES = SHOOTING;
					can_shoot = false;
				}
				else
				{
					game.STATES = WAITING;
				}
			}
			else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES)
			{
				if ((dist > BOUNCER_RADIUS) && (fabs((float)dist_y / dist_x) > 0.06) && (dist_y < 0))
				{
					draw_aim(&win, bouncers[0], dist_x, dist_y, dist, squares, &game);
					can_shoot = true;
				}
				else
				{
					draw_wait(&win, bouncers[0], squares, &game);
					can_shoot = false;
				}
			}
			break;

		case SHOOTING:
			if (ev.type == ALLEGRO_EVENT_TIMER)
			{
				if (new_shots)
				{
					bouncer_t **new_bouncers = realloc(bouncers, sizeof(bouncer_t) * game.bouncers);
					if (new_bouncers == NULL)
					{
						perror("Error reallocking bouncers");
						exit(1);
					}
					bouncers = new_bouncers;

					for (i = game.bouncers - new_bouncers_count; i < game.bouncers; i++)
					{
						bouncers[i] = createBouncer(game.shooting_x, game.shooting_y);
					}

					new_bouncers_count = 0;
					new_shots = false;
				}

				launch_interval++;
				if (launch_interval >= 5 && launch_index < game.bouncers)
				{
					launch_interval = 0;
					bouncers[launch_index]->x = game.shooting_x;
					bouncers[launch_index]->y = game.shooting_y;
					bouncers[launch_index]->dx = game.dx;
					bouncers[launch_index]->dy = game.dy;
					game.thrown_bouncers += 1;
					launch_index++;
				}

				for (i = 0; i < launch_index; i++)
				{
					if (bouncers[i] != NULL)
					{
						if (bouncers[i]->x <= BOUNCER_RADIUS)
						{
							bouncers[i]->dx = -bouncers[i]->dx;
						}

						if (bouncers[i]->x >= RES_WIDTH - BOUNCER_RADIUS)
						{
							bouncers[i]->dx = -bouncers[i]->dx;
						}

						if (bouncers[i]->y <= BOUNCER_RADIUS)
						{
							bouncers[i]->dy = -bouncers[i]->dy;
						}

						for (int c = 0; c < COL; c++)
						{
							if (calc_square_i_x(c, square_side) <= bouncers[i]->x && calc_square_f_x(c, square_side) >= bouncers[i]->x)
							{
								for (int l = 1; l < ROW; l++)
								{
									if (squares[l][c] > 0 && calc_square_i_y(l, square_side) <= bouncers[i]->y + BOUNCER_RADIUS && calc_square_f_y(l, square_side) >= bouncers[i]->y - BOUNCER_RADIUS)
									{
										bouncers[i]->dy = -bouncers[i]->dy;
										squares[l][c]--;
									}
									if (squares[l][c] < 0 && calc_square_mid_y(l, square_side) <= bouncers[i]->y + 2 * BOUNCER_RADIUS && calc_square_mid_y(l, square_side) >= bouncers[i]->y - 2 * BOUNCER_RADIUS)
									{
										squares[l][c] = 0;
										new_bouncers_count++;
									}
								}
							}
						}

						for (int l = 0; l < ROW; l++)
						{
							if (calc_square_i_y(l, square_side) <= bouncers[i]->y && calc_square_f_y(l, square_side) >= bouncers[i]->y)
							{
								for (int c = 1; c < COL; c++)
								{
									if (squares[l][c] > 0 && calc_square_i_x(c, square_side) <= bouncers[i]->x + BOUNCER_RADIUS && calc_square_f_x(c, square_side) >= bouncers[i]->x - BOUNCER_RADIUS)
									{
										bouncers[i]->dx = -bouncers[i]->dx;
										squares[l][c]--;
									}
									if (squares[l][c] < 0 && calc_square_mid_x(c, square_side) <= bouncers[i]->x + 2 * BOUNCER_RADIUS && calc_square_mid_x(c, square_side) >= bouncers[i]->x - 2 * BOUNCER_RADIUS)
									{
										squares[l][c] = 0;
										new_bouncers_count++;
									}
								}
							}
						}

						bouncers[i]->x += bouncers[i]->dx;
						bouncers[i]->y += bouncers[i]->dy;

						draw_shoot(&win, bouncers, game.bouncers, squares, &game);

						if ((bouncers[i]->dy > 0) && (bouncers[i]->y >= game.shooting_y))
						{
							arrival_counter++;
							bouncers[i]->dx = 0;
							bouncers[i]->dy = 0;

							if (arrival_counter == 1)
							{
								bouncer_t *auxBouncer = bouncers[i];
								bouncers[i] = bouncers[0];
								bouncers[0] = auxBouncer;
							}

							if (arrival_counter == game.bouncers)
							{
								bouncer_t *auxBouncer = bouncers[i];
								bouncers[i] = bouncers[game.bouncers - 1];
								bouncers[game.bouncers - 1] = auxBouncer;
							}
						}

						if (arrival_counter > 0 && bouncers[i]->y >= game.shooting_y)
						{
							if (bouncers[i]->x > bouncers[0]->x)
							{
								bouncers[i]->x -= SPEED_FACTOR;
							}
							if (bouncers[i]->x < bouncers[0]->x)
							{
								bouncers[i]->x += SPEED_FACTOR;
							}
							if (bouncers[i]->x <= bouncers[0]->x + SPEED_FACTOR && bouncers[i]->x >= bouncers[0]->x - SPEED_FACTOR)
							{
								bouncers[i]->x = bouncers[0]->x;

								if (arrival_counter == game.bouncers && i == game.bouncers - 1)
								{
									game.STATES = SETUP;
									new_shots = (new_bouncers_count > 0);
									game.bouncers += new_bouncers_count;

									launch_index = 0;
									game.thrown_bouncers = 0;
									game.shooting_x = bouncers[0]->x;
									arrival_counter = 0;
								}
							}
						}
					}
				}
			}
			break;

		case GAMEOVER:
			if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
			{
				if (ev.mouse.x > RES_WIDTH * 0.5 - 160 && ev.mouse.x < RES_WIDTH * 0.5 + 160 && ev.mouse.y > win.disp_data.height * 0.6 && ev.mouse.y < win.disp_data.height * 0.6 + 80)
				{
					game.STATES = SETUP;
					setup_game(&game, RES_WIDTH);

					setup_squares(squares);

					setup_bouncers(&bouncers, RES_WIDTH, game.shooting_y);

					menu_drew = false;
					can_shoot = false;
					new_blocks = false;
					new_shots = false;

					launch_interval = 0;
					launch_index = 0;
					arrival_counter = 0;
					new_bouncers_count = 0;

					offset = 0;
				}
				if (ev.mouse.x > RES_WIDTH * 0.5 - 160 && ev.mouse.x < RES_WIDTH * 0.5 + 160 && ev.mouse.y > win.disp_data.height * 0.75 && ev.mouse.y < win.disp_data.height * 0.75 + 80)
				{
					menu_drew = false;
					game.STATES = MENU;
				}
			}
			break;

		default:
			break;
		}

		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			game.STATES = EXIT;
		}
	}

	destroy_bouncers(bouncers, &game);
	graph_deinit(win);
	return 0;
}