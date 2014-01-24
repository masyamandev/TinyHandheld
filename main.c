#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#define DDR_MASK 0b11111111

#define MAX_BRIGHT 3

//#include "games/test.c"
#include "games/snake.c"

uint16_t frameLength;
volatile uint16_t timeCounter;
volatile uint32_t cycleCounter;

volatile uint16_t randomSeed;

const uint8_t TRANSFORM_Y[DISPLAY_HEIGHT] = {
		0b00000100,
		0b10000000,
		0b01000000,
		0b00001000,
		0b00100000,
		0b00010000,
		0b00000010,
		0b00000001
	};
const uint8_t TRANSFORM_X[DISPLAY_WIDTH] = {
		0b00000100,
		0b00000010,
		0b01000000,
		0b00000001,
		0b00001000,
		0b10000000,
		0b00100000,
		0b00010000
	};
//const uint8_t TRANSFORM_X[DISPLAY_WIDTH] = {
//		0b00010000,
//		0b00100000,
//		0b10000000,
//		0b00001000,
//		0b00000001,
//		0b01000000,
//		0b00000010,
//		0b00000100
//	};

uint8_t displayRaw[DISPLAY_HEIGHT][MAX_BRIGHT];
uint8_t displayLine;

void setFrameLength(uint32_t msec)
{
	frameLength = msec * F_CPU / 1000 / 256 / 8;
}

void clearScreen()
{
	uint8_t y, x;
	for (y = 0; y < DISPLAY_HEIGHT; y++)
		for (x = 0; x < DISPLAY_WIDTH; x++)
			display[x][y] = COLOR_BLACK;
}

void showScreen(uint8_t *screen)
{
	uint8_t y, x;
	for (y = 0; y < DISPLAY_HEIGHT; y++)
	{
		for (x = 0; x < DISPLAY_WIDTH; x++)
		{
			display[x][y] = ((screen[y] & (1 << (7 - x))) != 0) ? COLOR_WHITE : COLOR_BLACK;
		}
	}
}

void refreshDisplay()
{
	uint8_t i, j, b;
	for (b = 0; b < MAX_BRIGHT; b++) // brightness
	{
		for (i = 0; i < DISPLAY_HEIGHT; i++) // y
		{
			displayRaw[i][b] = 0;
			for (j = 0; j < DISPLAY_WIDTH; j++) // x
			{
				if (display[j][i] > b) {
					displayRaw[i][b] |= TRANSFORM_X[j];
				}
			}
		}
	}
}

int main(void)
{
	DDRB = DDR_MASK;
	DDRD = DDR_MASK;
	
	DDRC = !DDR_MASK;
	PORTC = DDR_MASK;

	displayLine = 0;
	timeCounter = 0;
	cycleCounter = 0;

	// init timer
	// prescale timer to 1/8th the clock rate
	TCCR0 |= (1<<CS01);
//	// prescale timer to 1/1st the clock rate
//	TCCR0 |= (1<<CS00);
	// enable timer overflow interrupt
	TIMSK |= (1<<TOIE0);
	
	setFrameLength(50);

	sei();

	// show splash
	showScreen(&SPLASH);
	refreshDisplay();
	buttonsHit = 0;
	while (!buttonsHit) {}

	// init game
	initGame();

	frameCounter = 0;
	while (1)
	{
		if (frameCounter != cycleCounter) {
			frameCounter = cycleCounter;

			// do game loop
			gameLoop();
			// draw frame
			refreshDisplay();
		}
	}
}

uint8_t getRandom(uint8_t maxValue) {
	uint8_t random = randomSeed % maxValue;
	randomSeed = randomSeed / maxValue;
	randomSeed ^= (randomSeed * 53) + timeCounter;
	return random;
}

// Timer interrupt
ISR(TIMER0_OVF_vect)
{
	// update counters
	timeCounter ++;
	if (timeCounter >= frameLength) {
		timeCounter = 0;
		cycleCounter ++;
	}

	// update display
	displayLine++;
	uint8_t line = (displayLine) & 0b111;
	uint8_t bright = (displayLine >> 3) & 0b111;
	if (bright >= 3) { // (bright >= sqrt(8))
		bright = 2;
	} else if (bright >= 1) {
		bright = 1;
	} else {
		bright = 0;
	}

	uint8_t lineTransformed = displayRaw[line][bright];

	PORTD = 0xFF;
	PORTB = lineTransformed;
	PORTD = ~TRANSFORM_Y[line];

	// update keyboard
	uint8_t prevButtonsPressed = buttonsPressed;
	buttonsPressed = ~PINC;
	uint8_t newButtonsHit = buttonsPressed & (~prevButtonsPressed);
	buttonsHit |= newButtonsHit;

	// update random
	if (newButtonsHit) {
		randomSeed = (randomSeed * 31) + timeCounter;
	}
}
