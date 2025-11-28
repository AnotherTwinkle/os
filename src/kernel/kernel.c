#include "../drivers/screen.h"
void main() {
	SCREEN_INIT();
	char *msg = "Hello, world!";
	print(msg);
}

