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
	bool info_drew = false;
	bool can_shoot = false;
	bool new_blocks = false;
	bool new_shots = false;
	bool ovosecreto = false;

	int launch_interval = 0;
	int launch_index = 0;
	int arrival_counter = 0;
	int new_balls_count = 0;

	float offset = 0;

	ALLEGRO_MOUSE_EVENT mouse_down;

	ALLEGRO_SAMPLE *music_menu = al_load_sample("utils/sounds/sfx_menu.ogg");
	ALLEGRO_SAMPLE *music_gameover = al_load_sample("utils/sounds/sfx_gameover.ogg");
	ALLEGRO_SAMPLE *music_shot = al_load_sample("utils/sounds/sfx_wing.ogg");

	ALLEGRO_SAMPLE_ID sfx_menu;
	ALLEGRO_SAMPLE_ID sfx_gameover;
	ALLEGRO_SAMPLE_ID sfx_shot;
	al_reserve_samples(3);

	float square_side = RES_WIDTH / 7.8;

	game_t game;
	int blocks[ROW][COL];

	ball_t **balls;
	game.shooting_y = block_f_y(8, square_side) - BALL_RADIUS;

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

		int distance_x;
		int distance_y;
		float dist;

		switch (game.STATES)
		{
		case MENU:
			if (!menu_drew)
			{
				draw_menu(&win);
				menu_drew = true;
				info_drew = false;
				al_play_sample(music_menu, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, NULL);
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
					al_stop_sample(&sfx_menu);
					game.STATES = SERVING;
					setup_game(&game, RES_WIDTH);

					setup_blocks(blocks);

					setup_balls(&balls, RES_WIDTH, game.shooting_y);

					menu_drew = false;
					can_shoot = false;
					new_blocks = false;
					new_shots = false;

					launch_interval = 0;
					launch_index = 0;
					arrival_counter = 0;
					new_balls_count = 0;

					offset = 0;
				}
				if (ev.mouse.x > RES_WIDTH * 0.5 - 160 && ev.mouse.x < RES_WIDTH * 0.5 + 160 && ev.mouse.y > win.disp_data.height * 0.8 && ev.mouse.y < win.disp_data.height * 0.8 + 80)
				{
					al_stop_sample(&sfx_menu);
					game.STATES = INFO;
				}
			}
			break;

		case INFO:
			if (!info_drew)
			{
				draw_info(&win, &game);
				info_drew = true;
				menu_drew = false;
			}
			if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
			{
				if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
					game.STATES = MENU;
			}
			if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
			{
				if (ev.mouse.x > RES_WIDTH * 0.5 - 160 && ev.mouse.x < RES_WIDTH * 0.5 + 160 && ev.mouse.y > win.disp_data.height * 0.4 && ev.mouse.y < win.disp_data.height * 0.4 + 80)
				{
					game.STATES = MENU;
				}
			}

			break;

		case SERVING:
			if (!new_blocks)
			{
				game.score++;
				if (game.score >= game.highscore)
				{
					game.highscore = game.score;
				}
				for (j = 0; j < COL; j++)
				{
					blocks[0][j] = (rand() % 3) * game.score;
				}
				if (game.score > 1)
				{
					int newBallPosition = rand() % COL;
					blocks[0][newBallPosition] = -1;
				}
				if (game.score >= 5 && ovosecreto)
				{
					int newBallPosition = rand() % COL - 3;
					blocks[0][newBallPosition] = -2;
				}
				new_blocks = true;
			}
			if (ev.type == ALLEGRO_EVENT_TIMER)
			{
				if (offset < (1.1 * square_side))
				{
					draw_setup(&win, balls[0], blocks, offset, &game);
					offset += 2;
				}
				else
				{
					offset = 0;
					for (j = 0; j < COL; j++)
					{
						if (blocks[ROW - 1][j] > 0)
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
								blocks[i][j] = blocks[i - 1][j];
							}
						}
						for (j = 0; j < COL; j++)
						{
							blocks[0][j] = 0;
						};
						game.STATES = WAITING;
						draw_wait(&win, balls[0], blocks, &game);
					}
					else
					{
						destroy_balls(balls, &game);
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
				if (ev.keyboard.keycode == ALLEGRO_KEY_O)
				{
					al_show_native_message_box(win.display, "CHEAT", "CHEAT MODE ON", "Acerte os circulos amarelos para ganhar mais 3 bolas!", NULL, 0);
					ovosecreto = true;
				}
				if (ev.keyboard.keycode == ALLEGRO_KEY_H)
				{
					al_show_native_message_box(win.display, "Instruções", "TEste textoo textooo uma um texto aqui um textooo", "textooo", NULL, 0);
				}
			}

			if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN)
			{
				mouse_down = ev.mouse;

				game.STATES = AIMING;
			}
			break;

		case AIMING:
			distance_x = (mouse_down.x - ev.mouse.x);
			distance_y = (mouse_down.y - ev.mouse.y);
			dist = sqrt(pow(distance_x, 2) + pow(distance_y, 2));

			if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
			{
				if (can_shoot)
				{
					game.dx = SPEED_FACTOR * (distance_x) / dist;
					game.dy = SPEED_FACTOR * (distance_y) / dist;

					al_play_sample(music_shot, 2.0, 0.0, 1.2, ALLEGRO_PLAYMODE_ONCE, &sfx_shot);
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
				if ((dist > BALL_RADIUS) && (fabs((float)distance_y / distance_x) > 0.06) && (distance_y < 0))
				{
					draw_aim(&win, balls[0], distance_x, distance_y, dist, blocks, &game);
					can_shoot = true;
				}
				else
				{
					draw_wait(&win, balls[0], blocks, &game);
					can_shoot = false;
				}
			}
			break;

		case SHOOTING:
			if (ev.type == ALLEGRO_EVENT_TIMER)
			{
				if (new_shots)
				{
					ball_t **new_balls = realloc(balls, sizeof(ball_t) * game.balls);
					if (new_balls == NULL)
					{
						perror("Error reallocking balls");
						exit(1);
					}
					balls = new_balls;

					for (i = game.balls - new_balls_count; i < game.balls; i++)
					{
						balls[i] = createball(game.shooting_x, game.shooting_y);
					}

					new_balls_count = 0;
					new_shots = false;
				}

				launch_interval++;
				if (launch_interval >= 5 && launch_index < game.balls)
				{
					launch_interval = 0;
					balls[launch_index]->x = game.shooting_x;
					balls[launch_index]->y = game.shooting_y;
					balls[launch_index]->dx = game.dx;
					balls[launch_index]->dy = game.dy;
					game.thrown_balls += 1;
					launch_index++;
				}

				for (i = 0; i < launch_index; i++)
				{
					if (balls[i] != NULL)
					{
						if (balls[i]->x <= BALL_RADIUS)
						{
							balls[i]->dx = -balls[i]->dx;
						}

						if (balls[i]->x >= RES_WIDTH - BALL_RADIUS)
						{
							balls[i]->dx = -balls[i]->dx;
						}

						if (balls[i]->y <= BALL_RADIUS)
						{
							balls[i]->dy = -balls[i]->dy;
						}

						for (int c = 0; c < COL; c++)
						{
							if (block_i_x(c, square_side) <= balls[i]->x && block_f_x(c, square_side) >= balls[i]->x)
							{
								for (int l = 1; l < ROW; l++)
								{
									if (blocks[l][c] > 0 && block_i_y(l, square_side) <= balls[i]->y + BALL_RADIUS && block_f_y(l, square_side) >= balls[i]->y - BALL_RADIUS)
									{
										balls[i]->dy = -balls[i]->dy;
										blocks[l][c]--;
									}
									if ((blocks[l][c] < 0 && blocks[l][c] == -1) && block_mid_y(l, square_side) <= balls[i]->y + 2 * BALL_RADIUS && block_mid_y(l, square_side) >= balls[i]->y - 2 * BALL_RADIUS)
									{
										blocks[l][c] = 0;
										new_balls_count++;
									}
									if ((blocks[l][c] < 0 && blocks[l][c] == -2) && block_mid_y(l, square_side) <= balls[i]->y + 2 * BALL_RADIUS && block_mid_y(l, square_side) >= balls[i]->y - 2 * BALL_RADIUS)
									{
										blocks[l][c] = 0;
										new_balls_count += 3;
									}
								}
							}
						}

						for (int l = 0; l < ROW; l++)
						{
							if (block_i_y(l, square_side) <= balls[i]->y && block_f_y(l, square_side) >= balls[i]->y)
							{
								for (int c = 1; c < COL; c++)
								{
									if (blocks[l][c] > 0 && block_i_x(c, square_side) <= balls[i]->x + BALL_RADIUS && block_f_x(c, square_side) >= balls[i]->x - BALL_RADIUS)
									{
										balls[i]->dx = -balls[i]->dx;
										blocks[l][c]--;
									}
									if (blocks[l][c] < 0 && block_mid_x(c, square_side) <= balls[i]->x + 2 * BALL_RADIUS && block_mid_x(c, square_side) >= balls[i]->x - 2 * BALL_RADIUS)
									{
										blocks[l][c] = 0;
										new_balls_count++;
									}
								}
							}
						}

						balls[i]->x += balls[i]->dx;
						balls[i]->y += balls[i]->dy;

						draw_shoot(&win, balls, game.balls, blocks, &game);

						if ((balls[i]->dy > 0) && (balls[i]->y >= game.shooting_y))
						{
							arrival_counter++;
							balls[i]->dx = 0;
							balls[i]->dy = 0;

							if (arrival_counter == 1)
							{
								ball_t *auxball = balls[i];
								balls[i] = balls[0];
								balls[0] = auxball;
							}

							if (arrival_counter == game.balls)
							{
								ball_t *auxball = balls[i];
								balls[i] = balls[game.balls - 1];
								balls[game.balls - 1] = auxball;
							}
						}

						if (arrival_counter > 0 && balls[i]->y >= game.shooting_y)
						{
							if (balls[i]->x > balls[0]->x)
							{
								balls[i]->x -= SPEED_FACTOR;
							}
							if (balls[i]->x < balls[0]->x)
							{
								balls[i]->x += SPEED_FACTOR;
							}
							if (balls[i]->x <= balls[0]->x + SPEED_FACTOR && balls[i]->x >= balls[0]->x - SPEED_FACTOR)
							{
								balls[i]->x = balls[0]->x;

								if (arrival_counter == game.balls && i == game.balls - 1)
								{
									game.STATES = SERVING;
									new_shots = (new_balls_count > 0);
									game.balls += new_balls_count;

									launch_index = 0;
									game.thrown_balls = 0;
									game.shooting_x = balls[0]->x;
									arrival_counter = 0;
								}
							}
						}
					}
				}
			}
			break;

		case GAMEOVER:
			al_play_sample(music_gameover, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &sfx_gameover);
			if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)
			{
				if (ev.mouse.x > RES_WIDTH * 0.5 - 160 && ev.mouse.x < RES_WIDTH * 0.5 + 160 && ev.mouse.y > win.disp_data.height * 0.6 && ev.mouse.y < win.disp_data.height * 0.6 + 80)
				{
					game.STATES = SERVING;
					setup_game(&game, RES_WIDTH);

					setup_blocks(blocks);

					setup_balls(&balls, RES_WIDTH, game.shooting_y);

					menu_drew = false;
					can_shoot = false;
					new_blocks = false;
					new_shots = false;

					launch_interval = 0;
					launch_index = 0;
					arrival_counter = 0;
					new_balls_count = 0;

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

	destroy_balls(balls, &game);
	al_destroy_sample(music_menu);
	al_destroy_sample(music_gameover);
	al_destroy_sample(music_shot);
	graph_deinit(win);
	return 0;
}