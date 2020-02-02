/*
	This file defines all the function needed to access the GPIO pins
	on an Raspberry PI.

	This file is written by Frank Bekema, All rights are reserved. 
*/

#ifndef CO_GPIO_H
#define CO_GPIO_H

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <poll.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../utils/utils.h"

#define GPIO_PATH "/sys/class/gpio/"

#define GPIO_IN 0
#define GPIO_OUT 1
#define GPIO_LOW 0
#define GPIO_HIGH 1
#define GPIO_INVALID 2

#define GPIO_EDGE_NONE 0
#define GPIO_EDGE_RISING 1
#define GPIO_EDGE_FALLING 2
#define GPIO_EDGE_BOTH 3

#define GPIO_POLL_TIMEOUT 1000 * 60 * 10

// Struct for an GPIO device
typedef struct gpio_device {
	const char * name;
	int start_pin;
	int pin_amount;
	bool initialized;
} gpio_device;

// Struct for an GPIO pin
typedef struct gpio_pin {
	gpio_device device;
	int pin_number;
	int mode;
	int active_low;
	int edge;
        bool initialized;
} gpio_pin;

void gpio_debug_message(const char * message);
bool gpio_check_pin(gpio_pin* pin);
bool gpio_check_device(gpio_device device);
const char* p_gpio_pin_folder_string(gpio_device device, int pin);
gpio_device* gpio_get_device(char const * name, int start_pin);
bool gpio_export(int pin_number);
bool gpio_unexport(int pin_number);
bool gpio_setmode(gpio_pin* pin, int mode);
gpio_pin* p_gpio_open(gpio_device* device, int pin, int mode);
bool gpio_close(gpio_pin* pin);
bool gpio_file_extractor(char** result, FILE* file);
int gpio_get_direction(gpio_pin* pin);
void gpio_write(gpio_pin* pin, int value);
int gpio_read(gpio_pin* pin);
bool gpio_set_active_low(gpio_pin* pin, int active_low);
bool gpio_set_edge(gpio_pin* pin, int edge);
const char* gpio_get_edge_string(int edge);
void gpio_poll(gpio_pin* pin);

#endif
