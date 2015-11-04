#include <pebble.h>

#include "menu.h"

static void init(void) {
	srand(time(NULL));

	pushMenu();
}

static void deinit(void) {
	deinitMenu();
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}
