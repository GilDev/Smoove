#include <pebble.h>

static Window *sWindow;
static ScrollLayer *sScroll;
#ifdef PBL_PLATFORM_BASALT
static StatusBarLayer *sStatusBar;
#endif

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

	sScroll = scroll_layer_create(frame);
	layer_add_child(windowLayer, scroll_layer_get_layer(sScroll));
}

static void windowUnload(Window *window)
{

}

void pushHelp(void)
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

void deinitHelp(void)
{
	if (sWindow != NULL) {
		window_destroy(sWindow);
	}
}
