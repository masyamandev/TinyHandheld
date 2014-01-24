#include "../framework.h"

#define SCREEN_MASK 0b111

#define SPEED_DIV 4

#define MAX_SNAKE_LENGTH 64
#define MAX_SNAKE_LENGTH_BITMASK 63

#define COLOR_HEAD			COLOR_WHITE
#define COLOR_SNAKE			COLOR_BRIGHT_GRAY
#define COLOR_BACKGROUND	COLOR_BLACK

const uint8_t SPLASH[8] = {
		0b00000001,
		0b01111111,
		0b01000000,
		0b01111110,
		0b00000010,
		0b00000010,
		0b11111110,
		0b10000000
	};

struct {
	uint8_t x;
	uint8_t y;
} snake[MAX_SNAKE_LENGTH];

uint8_t sHead, sTail;

uint8_t moveX, moveY, move;
uint8_t foodX, foodY;

#define STATE_GAME	0
#define STATE_DEAD	1
uint8_t state;

void putRandomFood()
{
	do {
		foodX = getRandom(DISPLAY_WIDTH);
		foodY = getRandom(DISPLAY_HEIGHT);
	} while (display[foodX][foodY] != COLOR_BACKGROUND ||
			(foodX == snake[sHead].x && foodY == snake[sHead].y));
}

void checkObstacles()
{
	if (snake[sHead].x == foodX && snake[sHead].y == foodY) {
		// Eat :)
		addTail();
		putRandomFood();
	} else if (display[snake[sHead].x][snake[sHead].y] != COLOR_BACKGROUND && move) {
		// Die :(
//		do {
////		while (sHead != sTail) {
//			display[snake[sTail].x][snake[sTail].y] = COLOR_BACKGROUND;
//			sTail = (sTail + 1) & MAX_SNAKE_LENGTH_BITMASK;
//		} while (snake[sHead].x != snake[sTail].x || snake[sHead].y != snake[sTail].y);
		display[foodX][foodY] = COLOR_BACKGROUND;
		state = STATE_DEAD;
	}
}

//void redrawSnake()
//{
////	uint8_t distBetweenDark = (sHead - sTail) & MAX_SNAKE_LENGTH_BITMASK;
////	uint8_t distFromHead = distBetweenDark - frameCounter % distBetweenDark;
//	uint8_t distBetweenDark = 3;
//	uint8_t distFromHead = 0;
//	uint8_t sn = sHead;
//	uint8_t cnt = 0;
//	do {
//		display[snake[sn].x][snake[sn].y] = ((cnt++) % distBetweenDark == distFromHead) ?
//				COLOR_DARK_GRAY : COLOR_BRIGHT_GRAY;
//		sn = (sn - 1) & MAX_SNAKE_LENGTH_BITMASK;
//		cnt ++;
//	} while (sn != sTail);
//	display[snake[sHead].x][snake[sHead].y] = COLOR_HEAD;
//}

void moveHead(uint8_t dx, uint8_t dy)
{
	uint8_t oldTail = sTail;
	uint8_t oldHead = sHead;
	sTail = (sTail + 1) & MAX_SNAKE_LENGTH_BITMASK;
	sHead = (sHead + 1) & MAX_SNAKE_LENGTH_BITMASK;
	snake[sHead].x = (snake[oldHead].x + dx) & SCREEN_MASK;
	snake[sHead].y = (snake[oldHead].y + dy) & SCREEN_MASK;

	display[snake[oldHead].x][snake[oldHead].y] = COLOR_SNAKE;
	display[snake[oldTail].x][snake[oldTail].y] = COLOR_BACKGROUND;
	display[snake[sTail].x][snake[sTail].y] = COLOR_SNAKE;

	checkObstacles();

//	redrawSnake();

	display[snake[sHead].x][snake[sHead].y] = COLOR_HEAD;
}

void addTail()
{
	uint8_t oldTail = sTail;
	sTail = (sTail - 1) & MAX_SNAKE_LENGTH_BITMASK;
	snake[sTail].x = snake[oldTail].x;
	snake[sTail].y = snake[oldTail].y;
}

void initGame()
{
	clearScreen();
	state = STATE_GAME;

	sHead = 0;
	sTail = 0;

	snake[0].x = 3;
	snake[0].y = 4;
	moveX = 0;
	moveY = 0;
	move = 0;

	putRandomFood();
	addTail();
}

void gameLoop()
{
	if (state == STATE_GAME) {
		if ((frameCounter % SPEED_DIV) == 0) {
			if ((buttonsHit & BUTTON_UP) && (move != BUTTON_DOWN)) {
				moveX = 0;
				moveY = -1;
				move = BUTTON_UP;
			} else if ((buttonsHit & BUTTON_DOWN) && (move != BUTTON_UP)) {
				moveX = 0;
				moveY = 1;
				move = BUTTON_DOWN;
			} else if ((buttonsHit & BUTTON_LEFT) && (move != BUTTON_RIGHT)) {
				moveX = -1;
				moveY = 0;
				move = BUTTON_LEFT;
			} else if ((buttonsHit & BUTTON_RIGHT) && (move != BUTTON_LEFT)) {
				moveX = 1;
				moveY = 0;
				move = BUTTON_RIGHT;
			}
			moveHead(moveX, moveY);

			buttonsHit = 0;
		}

		display[foodX][foodY] = frameCounter % 4;
	} else if (state == STATE_DEAD) {
		if (buttonsHit & (BUTTON_A | BUTTON_B)) {
			initGame();
		}
		buttonsHit = 0;
	}

//	redrawSnake();

//	uint8_t i;
//	for (i = 0; i < 8; i++) {
//		display[7][i] = (buttonsPressed & (1<<i)) ? 3 : 0;
//	}
//	for (i = 0; i < 8; i++) {
//		display[6][i] = (buttonsHit & (1<<i)) ? 3 : 0;
//	}
}
