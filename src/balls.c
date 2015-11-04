#include <pebble.h>
#include "balls.h"

struct Ball balls[MAX_BALLS];
struct Ball player;
bool callStopHandlers;

static uint8_t i;

void newBall(enum BallId id, enum BallDirection direction, uint8_t speed, void (*stopCallback)())
{
	for (i = 0; i < MAX_BALLS; i++) {
		if (!balls[i].visible) {
			balls[i].visible = true;
			balls[i].speed = speed;
			balls[i].stopCallback = stopCallback;

			if (direction == BALL_UP || direction == BALL_DOWN) {
				balls[i].x = (54 + id * 18) * 5;
				balls[i].xDir = 0;
				if (direction == BALL_DOWN) {
					balls[i].y = (-(BALL_RADIUS + 1)) * 5;
					balls[i].yDir = 1;
				} else {
					balls[i].y = (168 + BALL_RADIUS) * 5;
					balls[i].yDir = -1;
				}
			} else {
				balls[i].y = (66 + id * 18) * 5;
				balls[i].yDir = 0;
				if (direction == BALL_RIGHT) {
					balls[i].x = (-(12 + BALL_RADIUS + 1)) * 5;
					balls[i].xDir = 1;
				} else {
					balls[i].x = (144 + 12 + BALL_RADIUS) * 5;
					balls[i].xDir = -1;
				}
			}

			break;
		}
	}
}

uint8_t updateBalls(void)
{
	for (i = 0; i < MAX_BALLS; i++) {
		if (balls[i].visible) {
			// Collision check
			if ((balls[i].x - player.x) * (balls[i].x - player.x) + (balls[i].y - player.y) * (balls[i].y - player.y) <= (BALL_DIAMETER * 5 * BALL_DIAMETER * 5))
				return true;

			// Position update
			if (balls[i].xDir != 0) {
				balls[i].x += balls[i].xDir * balls[i].speed;

				if ((balls[i].xDir == 1 && balls[i].x >= (144 + 12 + BALL_RADIUS) * 5) || (balls[i].xDir == -1 && balls[i].x <= (-(BALL_RADIUS + 12 + 1)) * 5)) {
					balls[i].visible = false;
					if (callStopHandlers)
						(*(balls[i].stopCallback))();
				}
			} else {
				balls[i].y += balls[i].yDir * balls[i].speed;

				if ((balls[i].yDir == 1 && balls[i].y >= (168 + BALL_RADIUS) * 5) || (balls[i].yDir == -1 && balls[i].y <= (-(BALL_RADIUS + 1)) * 5)) {
					balls[i].visible = false;
					if (callStopHandlers)
						(*(balls[i].stopCallback))();
				}
			}
		}
	}

	// Player movements
	if (player.xDir != 0) {
		player.x += (player.xDir) * 15;
		if (player.x == (72 - (BALL_DIAMETER + 3)) * 5 || player.x == 72 * 5 || player.x == (72 + (BALL_DIAMETER + 3)) * 5)
			player.xDir = 0;
	} else if (player.yDir != 0) {
		player.y += (player.yDir) * 15;
		if (player.y == (84 - (BALL_DIAMETER + 3)) * 5 || player.y == 84 * 5 || player.y == (84 + (BALL_DIAMETER + 3)) * 5)
			player.yDir = 0;
	}

	printf("%d,%d - %d,%d\n%d,%d", player.x, player.y, player.xDir, player.yDir, player.x / 5, player.y / 5);

	return false;
}

void resetBalls(void)
{
	for (i = 0; i < MAX_BALLS; i++)
		balls[i].visible = false;
	callStopHandlers = true;
}

void initBalls(void)
{
	resetBalls();

	player.x = (144 / 2) * 5;
	player.y = (168 / 2) * 5;
	player.xDir = 0;
	player.yDir = 0;
}
