#include "../framework.h"

const uint8_t SPLASH[8][2] = {
		{0b01111110, 0b11111111},
		{0b10000001, 0b10000001},
		{0b10011101, 0b10100101},
		{0b10100101, 0b10000001},
		{0b10100101, 0b10000001},
		{0b10011010, 0b10100101},
		{0b10000000, 0b10000001},
		{0b01111110, 0b11111111}
	};

void initGame()
{
}

void gameLoop()
{
	//			{
	//				uint8_t bright = frameCounter & 0b11;
	//				if (frameCounter & 0b100) {
	//					bright = 3 - bright;
	//				}
	//				uint8_t figure = (frameCounter >> 3) & 1;
	//
	//				uint8_t i, j;
	//				for (i = 0; i < 8; i++)
	//				{
	//					for (j = 0; j < 8; j++)
	//					{
	//						display[i][j] = ((SPLASH[i][figure] & (1 << (7 - j))) != 0) ? bright : 0;
	//					}
	//				}
	//			}
				{
					uint8_t line = frameCounter % 16;

					uint8_t i, j;
					for (i = 0; i < 8; i++)
					{
						for (j = 0; j < 8; j++)
						{
							uint8_t lineDist = ((i <= 3 ? 3 - i : i - 4) + j - line + 4) & 7;
							display[i][j] = lineDist <= 3 ? lineDist : 7 - lineDist;
						}
					}
				}
	//			{
	//				uint8_t line = frameCounter % 16;
	//
	//				uint8_t i, j;
	//				for (i = 0; i < 8; i++)
	//				{
	//					for (j = 0; j < 8; j++)
	//					{
	//						uint8_t lineDist = ((i <= 3 ? 3 - i : i - 4) +
	//								(j <= 3 ? 3 - j : j - 4) -
	//								line) & 7;
	//						display[i][j] = lineDist <= 3 ? lineDist : 7 - lineDist;
	//					}
	//				}
	//			}
}
