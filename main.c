#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "co-gpio/driver.h"

int main() {
	gpio_device* p_device = gpio_get_device("gpiochip458", 458);
	if(p_device != NULL) {
		gpio_pin* p_gpio_pin = p_gpio_open(p_device, 2, GPIO_OUT);
		//gpio_set_active_low(p_gpio_pin, 1);
		gpio_write(p_gpio_pin, GPIO_HIGH);
		sleep(1);
		gpio_write(p_gpio_pin, GPIO_LOW);
		sleep(1);
                //while(true) {
			//printf("%d\n", gpio_read(p_gpio_pin));
			//sleep(0.5);
		//}
		//gpio_close(p_gpio_pin);
		free(p_gpio_pin);
		free(p_device);
	}
	printf("\n");
	return 0;
}
