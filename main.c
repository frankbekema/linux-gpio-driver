#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "co-gpio/driver.h"

int main() {
	struct gpio_device* p_device = gpio_get_device("gpiochip458", 458);
	if(p_device != NULL) {	
		gpio_open(*p_device, 2, GPIO_IN);
		gpio_write(*p_device, 2, GPIO_HIGH);
		sleep(2);
		gpio_write(*p_device, 2, GPIO_LOW);
		sleep(2);
		gpio_close(*p_device, 2);
		free(p_device);
	}
	printf("\n");
	return 0;
}
