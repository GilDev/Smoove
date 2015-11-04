#include <pebble.h>
#include "menu.h"

#include "game.h"
#include "help.h"

static Window *sWindow;
static SimpleMenuLayer *sMenu;

#ifdef PBL_PLATFORM_BASALT
static StatusBarLayer *sStatusBar;
#endif

void selectClick(int index, void *ctx);
static SimpleMenuItem items[] = {{
	.title = "Play!",
	.subtitle = NULL,
	.icon = NULL,
	.callback = selectClick
}, {
	.title = "Help",
	.subtitle = NULL,
	.icon = NULL,
	.callback = selectClick
}, {
	.title = "About",
	.subtitle = NULL,
	.icon = NULL,
	.callback = selectClick
}};

static SimpleMenuSection section = {
	.title = NULL,
	.items = items,
	.num_items = 3
};

void selectClick(int index, void *ctx)
{
	switch (index) {
		case 0:
			pushGame();
			break;
		case 1:
			pushHelp();
	}
}

static void windowLoad(Window *window)
{
	Layer *windowLayer = window_get_root_layer(window);
	GRect frame = layer_get_frame(windowLayer);

	#ifdef PBL_PLATFORM_BASALT
	sStatusBar = status_bar_layer_create();
	layer_add_child(windowLayer, status_bar_layer_get_layer(sStatusBar));

	frame.size.h -= STATUS_BAR_LAYER_HEIGHT;
	frame.origin.y += STATUS_BAR_LAYER_HEIGHT;
	#endif

	sMenu = simple_menu_layer_create(frame, sWindow, &section, 1, NULL);

	layer_add_child(windowLayer, simple_menu_layer_get_layer(sMenu));
}

static void windowUnload(Window *window)
{
	simple_menu_layer_destroy(sMenu);

	#ifdef PBL_PLATFORM_BASALT
	status_bar_layer_destroy(sStatusBar);
	#endif
}

void pushMenu(void)
{
	if (sWindow == NULL) {
		sWindow = window_create();
		window_set_window_handlers(sWindow, (WindowHandlers) {
			.load = windowLoad,
			.unload = windowUnload 
		});
	}

	window_stack_push(sWindow, true);
}

void deinitMenu(void)
{
	deinitGame();
	if (sWindow != NULL)
		window_destroy(sWindow);
}
