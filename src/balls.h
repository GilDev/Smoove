#ifndef BALLS_H
#define BALLS_H

#define BALL_RADIUS 7
#define BALL_DIAMETER (BALL_RADIUS * 2 + 1)
#define MAX_BALLS 8

enum BallId {BALL_FIRST = 0, BALL_SECOND, BALL_THIRD};
enum BallDirection {BALL_UP = 0, BALL_RIGHT, BALL_DOWN, BALL_LEFT};

struct Ball {
	int16_t x; // Times 5 to have more "speeds" available. A speed of 1 would be 1/5 pixel/frame. Divison by 5 to get the real coordinates occurs on rendering
	int16_t y;
	uint8_t speed;
	int8_t xDir: 2;
	int8_t yDir: 2;
	uint8_t visible: 1;
	void (*stopCallback)();
};

void newBall(enum BallId id, enum BallDirection direction, uint8_t speed, void (*stopCallback)());
uint8_t updateBalls(void); // Return 1 if collision
void resetBalls(void);
void initBalls(void);

extern struct Ball balls[MAX_BALLS];
extern struct Ball player;
extern bool callStopHandlers;

#endif