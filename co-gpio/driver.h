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
#include "../utils/utils.h"

#define GPIO_PATH "/sys/class/gpio/"
#define GPIO_IN 0
#define GPIO_OUT 1
#define GPIO_LOW 0
#define GPIO_HIGH 1

// Struct for an GPIO device
struct gpio_device {
	const char * name;
	int start_pin;
	int pin_amount;
};

void gpio_debug_message(const char * message);
bool gpio_check_device(struct gpio_device device);
const char * p_gpio_pin_folder_string(struct gpio_device device, int pin);
struct gpio_device* gpio_get_device(char const * name, int start_pin);
bool gpio_export(int pin_number);
bool gpio_unexport(int pin_number);
bool gpio_setmode(struct gpio_device device, int pin, int mode);
void gpio_open(struct gpio_device device, int pin, int mode);
void gpio_close(struct gpio_device device, int pin);
void gpio_write(struct gpio_device, int pin, int value);

#endif
