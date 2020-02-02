#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "co-gpio/driver.h"

void enable_server(gpio_device* p_device) {	
	gpio_pin* p_gpio_pin = p_gpio_open(p_device, 2, GPIO_OUT);
	gpio_write(p_gpio_pin, GPIO_HIGH);
	sleep(1);
	gpio_write(p_gpio_pin, GPIO_LOW);
	sleep(1);
	free(p_gpio_pin);
}

void edge_test(gpio_device* device) {
	gpio_pin* p_gpio_pin = p_gpio_open(device, 3, GPIO_IN);
	gpio_set_active_low(p_gpio_pin, 1);
        gpio_set_edge(p_gpio_pin, GPIO_EDGE_BOTH);
	gpio_poll(p_gpio_pin);

	printf("%d\n", gpio_read(p_gpio_pin));
	/*while(true) {i

	}*/
	free(p_gpio_pin);	
}

void read_pin(gpio_device* device) {
	
}

int main() {
	gpio_device* p_device = gpio_get_device("gpiochip458", 458);
	if(p_device != NULL) {
		//enable_server(p_device);
		edge_test(p_device);	
	}
	free(p_device);
	printf("\n");
	return 0;
}
