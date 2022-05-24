#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>

const short int WIDTH = 20;
const short int HEIGHT = 10;

enum {UP = 0, RIGHT, DOWN, LEFT};


typedef struct 
{
	short int body[100][2];
	short int length;
	short int alive;

} Snake;


typedef struct 
{
	short int x;
	short int y;

} Coords;


void msSleep(short int milliseconds);
void listenArrows(short int *direction, short int ch);

void generateField(char field[HEIGHT][WIDTH]);
void generateFood(char field[HEIGHT][WIDTH], Snake *snake, Coords *food);
void newSnake(Snake *snake);
void generateSnake(Snake *snake, char field[HEIGHT][WIDTH]);
void isAlive(Snake *snake);
void snakeMoving(Snake *snake, short int direction);
void eating(Snake *snake, Coords *food, short int direction, short int *record);
void renderField(char field[HEIGHT][WIDTH], short int record);



int main()
{
	char field[HEIGHT][WIDTH];

	short int direction = RIGHT;

	short int ch;

	short int record = 0;

	Snake snake;
	snake.length = 0;
	snake.alive = 1;

	Coords food;
	food.x = 0;
	food.y = 0;

	initscr();
	keypad(stdscr, TRUE);
	cbreak();
	nonl();
	noecho();
	timeout(0);
	curs_set(0);

	while ((ch = getch()) != 'q' && snake.alive) {
		listenArrows(&direction, ch);
		generateField(field);
		generateSnake(&snake, field);
		generateFood(field, &snake, &food);
		snakeMoving(&snake, direction);
		eating(&snake, &food, direction, &record);
		renderField(field, record);
		msSleep(500);
	}


	curs_set(1);
	clear();
	refresh();
	resetty();
	endwin();

	if (!snake.alive) {
		renderField(field, record);
		printf("You are lose!\r");
	}

	return 0;
}


void msSleep(short int milliseconds)
{
	short int msec = 0;
	clock_t before = clock();

	do {
	  clock_t difference = clock() - before;
	  msec = difference * 1000 / CLOCKS_PER_SEC;
	  msec++;
	} while (msec < milliseconds);
}


void listenArrows(short int *direction, short int ch)
{

	switch (ch) {

		case KEY_UP:
			puts("up");
			*direction = UP;
		    break;
	 
		case KEY_DOWN:
			*direction = DOWN;
		    break;
	 
		case KEY_LEFT:
			*direction = LEFT;
		    break;
	 
		case KEY_RIGHT:
			*direction = RIGHT;
		    break;

	}
}


void generateField(char field[HEIGHT][WIDTH]) 
{
	system("clear");

	short int i, j;

	for (i = 0; i < HEIGHT; ++i)
	{
		j = 0;

		field[i][j] = '#';
		for (j = 1; j < WIDTH - 2; ++j)
		{
			if (i == 0 || i == HEIGHT - 1) {
				field[i][j] = '#';

			} else {
				field[i][j] = ' ';
			}
		}
		field[i][j] = '#';
		field[i][j + 1] = '\0';
	}
}


void generateFood(char field[HEIGHT][WIDTH], Snake *snake, Coords *food) 
{
	if (food->x == 0 && food->y == 0) {
		srand(time(NULL));

		short int cross = 1;

		while (cross) {
			food->x = rand() % ((WIDTH - 2) - 2) + 2;
			food->y = rand() % ((HEIGHT - 2) - 2) + 2;

			for (int i = 0; i < snake->length; ++i)
			{
				if (snake->body[i][0] != food->x && snake->body[i][1] != food->y) {
					cross = 0;

				} else {
					cross = 1;
				}
			}
		}	
	}

	field[food->y][food->x] = '*';
}


void newSnake(Snake *snake)
{
	srand(time(NULL));
	short int x = 1;
	short int y = rand() % ((HEIGHT - 1) - 1) + 1;

	snake->length = 3;

	for (short int i = 0; i < snake->length; ++i)
	{
		snake->body[i][0] = x;
		snake->body[i][1] = y;
		++x;
	}
}


void generateSnake(Snake *snake, char field[HEIGHT][WIDTH])
{
	if (snake->length == 0) {
		newSnake(snake);
	}

	for (short int i = 0; i < snake->length; ++i)
	{
		field[snake->body[i][1]][snake->body[i][0]] = 'o';
	}
}


void snakeMoving(Snake *snake, short int direction)
{
	short int buff1[2], buff2[2];

	for (short int i = snake->length - 1; -1 < i; --i)
	{
		if (i == snake->length - 1) {
			buff1[0] = snake->body[i][0];
			buff1[1] = snake->body[i][1];

			switch (direction) {
				case UP:
					--snake->body[i][1];
					
					if (snake->body[i][1] < 1) {
						snake->body[i][1] = HEIGHT - 2;
					}

					break;

				case RIGHT:
					++snake->body[i][0];

					if (snake->body[i][0] > WIDTH - 3) {
						snake->body[i][0] = 1;
					}

					break;

				case DOWN:
					++snake->body[i][1];

					if (snake->body[i][1] > HEIGHT - 2) {
						snake->body[i][1] = 1;
					}

					break;

				case LEFT:
					--snake->body[i][0];

					if (snake->body[i][0] < 1) {
						snake->body[i][0] = WIDTH - 3;
					}

					break;
			}

			isAlive(snake);

		} else {
			buff2[0] = snake->body[i][0];
			buff2[1] = snake->body[i][1];

			snake->body[i][0] = buff1[0];
			snake->body[i][1] = buff1[1];

			buff1[0] = buff2[0];
			buff1[1] = buff2[1];
		}
	}
}


void isAlive(Snake *snake)
{
	short int buff[] = {snake->body[snake->length - 1][0], snake->body[snake->length - 1][1]};

	for (short int i = 0; i < snake->length; ++i)
	{
		if (
			i != snake->length - 1 
			&& snake->body[i][0] == buff[0]
			&& snake->body[i][1] == buff[1]
		) {
			snake->alive = 0;
		}
	}
}



void eating(Snake *snake, Coords *food, short int direction, short int *record)
{

	if (snake->body[snake->length - 1][0] == food->x && snake->body[snake->length - 1][1] == food->y) {
		switch (direction) {
			case UP:
				snake->body[snake->length][1] = snake->body[snake->length - 1][1] - 1;
				snake->body[snake->length][0] = snake->body[snake->length - 1][0];

				if (snake->body[snake->length][1] < 1) {
					snake->body[snake->length][1] = HEIGHT - 2;
				}

				food->x = 0;
				food->y = 0;

				++snake->length;
				++*record;

				break;

			case RIGHT:
				snake->body[snake->length][0] = snake->body[snake->length - 1][0] + 1;
				snake->body[snake->length][1] = snake->body[snake->length - 1][1];

				if (snake->body[snake->length][0] > WIDTH - 3) {
					snake->body[snake->length][0] = 1;
				}

				food->x = 0;
				food->y = 0;

				++snake->length;
				++*record;

				break;

			case DOWN:
				snake->body[snake->length][1] = snake->body[snake->length - 1][1] + 1;
				snake->body[snake->length][0] = snake->body[snake->length - 1][0];

				if (snake->body[snake->length][1] > HEIGHT - 2) {
					snake->body[snake->length][1] = 1;
				}

				food->x = 0;
				food->y = 0;

				++snake->length;
				++*record;

				break;

			case LEFT:
				snake->body[snake->length][0] = snake->body[snake->length - 1][0] - 1;
				snake->body[snake->length][1] = snake->body[snake->length - 1][1];

				if (snake->body[snake->length][0] < 1) {
					snake->body[snake->length][0] = WIDTH - 3;
				}

				food->x = 0;
				food->y = 0;

				++snake->length;
				++*record;

				break;
			}

		isAlive(snake);
	}
}


void renderField(char field[HEIGHT][WIDTH], short int record)
{
	printf("Record %d\r\n", record);
	for (short int i = 0; i < HEIGHT; ++i)
	{
		printf("%s\n\r", field[i]);
	}
}