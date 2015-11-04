#include <pebble.h>
#include "game.h"

#include "balls.h"

static Window *sWindow;
static AppTimer *updateTimer;

static TextLayer *sReadyText;
static TextLayer *sOverText;
static PropertyAnimation *sPropertyAnimation;

static time_t levelsTimer;
static uint8_t level, animation;
static GColor bgColor;

static void quitGame(void *data)
{
	window_stack_pop(true);
}

static void lvl1StopCallback(void)
{
	newBall(rand() % 3, rand() % 4, 12, lvl1StopCallback);
}

static void update(void *data);

static void setLevel(uint8_t lvl)
{
	animation = 0;

	switch (level) {
		case 1:
			lvl1StopCallback();
			updateTimer = app_timer_register(1000, update, NULL);
		case 0:
			levelsTimer = time(NULL);
			break;
	}

	// Colors
	#ifdef PBL_COLOR
		switch (level) {
			case 0:
			case 1:
				bgColor = GColorChromeYellow;
				levelsTimer = time(NULL);
				break;
			case 2:
				bgColor = GColorVividCerulean;
		}
	#else
		// Invert colors on B&W screens at each level change
		if (level > 1) {
			if (bgColor == GColorWhite)
				bgColor = GColorBlack;
			else
				bgColor = GColorWhite;
		} else {
			bgColor = GColorWhite;
		}
	#endif
}

static void update(void *data)
{
	callStopHandlers = true;

	if (updateBalls()) {
		layer_add_child(window_get_root_layer(sWindow), text_layer_get_layer(sReadyText));
		layer_add_child(window_get_root_layer(sWindow), text_layer_get_layer(sOverText));
		updateTimer = app_timer_register(3000, quitGame, NULL);
		return;
	}

	layer_mark_dirty(window_get_root_layer(sWindow));
	updateTimer = app_timer_register(40, update, NULL);

	if (time(NULL) - levelsTimer > 10) {
		setLevel(++level);
		callStopHandlers = false;
		app_timer_reschedule(updateTimer, 2000);
		levelsTimer = time(NULL);
	}
}

static void drawingProcedure(struct Layer *layer, GContext *ctx)
{
	#ifdef PBL_PLATFORM_BASALT
	graphics_context_set_antialiased(ctx, false);
	#endif

	// Background
	graphics_context_set_fill_color(ctx, bgColor);
	graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
	graphics_context_set_stroke_color(ctx, COLOR_FALLBACK(GColorBlack, (bgColor == GColorBlack) ? GColorWhite : GColorBlack));
	graphics_draw_round_rect(ctx, GRect(45, 57, 55, 55), BALL_RADIUS);
	graphics_draw_line(ctx, GPoint(63, 57), GPoint(63, 111));
	graphics_draw_line(ctx, GPoint(81, 57), GPoint(81, 111));
	graphics_draw_line(ctx, GPoint(45, 75), GPoint(99, 75));
	graphics_draw_line(ctx, GPoint(45, 93), GPoint(99, 93));

	// Ennemies
	graphics_context_set_fill_color(ctx, COLOR_FALLBACK(GColorBlack, (bgColor == GColorBlack) ? GColorWhite : GColorBlack));
	static uint8_t i;
	for (i = 0; i < MAX_BALLS; i++)
		if (balls[i].visible)
			graphics_fill_circle(ctx, GPoint(balls[i].x / 5, balls[i].y / 5), BALL_RADIUS);

	// Player
	graphics_context_set_fill_color(ctx, COLOR_FALLBACK(GColorWhite, (bgColor == GColorBlack) ? GColorWhite : GColorBlack));
	graphics_fill_circle(ctx, GPoint(player.x / 5, player.y / 5), BALL_RADIUS);
}

static void upClick(ClickRecognizerRef recognizer, void *ctx)
{
	if (player.xDir == 0 && player.yDir == 0 && player.y != (84 - (BALL_RADIUS * 2 + 4)) * 5)
		player.yDir = -1;
}

static void selectClick(ClickRecognizerRef recognizer, void *ctx)
{
	if (player.xDir == 0 && player.yDir == 0 && player.x != (72 + (BALL_RADIUS * 2 + 4)) * 5)
		player.xDir = 1;
}

static void downClick(ClickRecognizerRef recognizer, void *ctx)
{
	if (player.xDir == 0 && player.yDir == 0 && player.y != (84 + (BALL_RADIUS * 2 + 4)) * 5)
		player.yDir = 1;
}

static void backClick(ClickRecognizerRef recognizer, void *ctx)
{
	if (player.xDir == 0 && player.yDir == 0 && player.x != (72 - (BALL_RADIUS * 2 + 4)) * 5)
		player.xDir = -1;
}

static void clickConfigProvider(void *ctx)
{
	window_single_click_subscribe(BUTTON_ID_UP, upClick);
	window_single_click_subscribe(BUTTON_ID_SELECT, selectClick);
	window_single_click_subscribe(BUTTON_ID_DOWN, downClick);
	window_single_click_subscribe(BUTTON_ID_BACK, backClick);
}

static uint8_t animationStep;

static void animationNewStep(Animation *animation, bool finished, void *ctx)
{
	if (animationStep == 3) {
		#ifdef PBL_PLATFORM_APLITE
		property_animation_destroy(sPropertyAnimation);
		#endif
		layer_remove_from_parent(text_layer_get_layer(sReadyText));
		text_layer_set_text(sReadyText, "Game");
		layer_set_frame(text_layer_get_layer(sReadyText), GRect(0, 0, 144, 50));

		setLevel(++level);
	} else {
		switch (animationStep) {
			case 0:
				text_layer_set_text(sReadyText, "2");
				break;
			case 1:
				text_layer_set_text(sReadyText, "1");
				break;
			case 2:
				text_layer_set_text(sReadyText, "Go!");
		}
		animationStep++;
		animation_schedule((Animation *) sPropertyAnimation);
	}
}

static void windowLoad(Window *window) {
	initBalls();
	setLevel(level = 0);

	layer_set_update_proc(window_get_root_layer(window), drawingProcedure);

	sReadyText = text_layer_create(GRectZero);
	text_layer_set_font(sReadyText, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
	text_layer_set_text_color(sReadyText, COLOR_FALLBACK(GColorWhite, GColorBlack));
	text_layer_set_background_color(sReadyText, GColorClear);
	text_layer_set_text_alignment(sReadyText, GTextAlignmentCenter);
	text_layer_set_text(sReadyText, "3");
	layer_add_child(window_get_root_layer(sWindow), text_layer_get_layer(sReadyText));

	// Start animation
	animationStep = 0;
	GRect from = GRect(-70, 0, 70, 50);
	GRect to = GRect(168, 0, 70, 50);
	sPropertyAnimation = property_animation_create_layer_frame(text_layer_get_layer(sReadyText), &from, &to);
	animation_set_duration((Animation *) sPropertyAnimation, 1000);
	animation_set_handlers((Animation *) sPropertyAnimation, (AnimationHandlers) {.stopped = animationNewStep}, NULL);
	animation_set_curve((Animation *) sPropertyAnimation, AnimationCurveLinear);
	animation_schedule((Animation *) sPropertyAnimation);

	sOverText = text_layer_create(GRect(0, 110, 144, 50));
	text_layer_set_font(sOverText, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
	text_layer_set_text_color(sOverText, COLOR_FALLBACK(GColorWhite, GColorBlack));
	text_layer_set_background_color(sOverText, GColorClear);
	text_layer_set_text_alignment(sOverText, GTextAlignmentCenter);
	text_layer_set_text(sOverText, "Over!");
}

static void windowUnload(Window *window) {
	text_layer_destroy(sReadyText);
	text_layer_destroy(sOverText);
	app_timer_cancel(updateTimer);
}

void pushGame(void)
{
	if (sWindow == NULL) {
		sWindow = window_create();
		window_set_click_config_provider(sWindow, clickConfigProvider);
		window_set_window_handlers(sWindow, (WindowHandlers) {
			.load = windowLoad,
			.unload = windowUnload 
		});

		#ifdef PBL_PLATFORM_APLITE
		window_set_fullscreen(sWindow, true);
		#endif
	}

	window_stack_push(sWindow, false);
}

void deinitGame(void)
{
	if (sWindow != NULL)
		window_destroy(sWindow);
}
