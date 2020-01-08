#include <stdio.h>
#include <stdint.h>
#include "co-gpio/driver.h"

int main() {
	struct gpio_device* p_device = gpio_get_device("gpiochip458", 458);
	if(p_device != NULL) {	
		gpio_open(*p_device, 2, GPIO_OUT);
		gpio_close(*p_device, 2);
		free(p_device);
	}
	printf("\n");
	return 0;
}
