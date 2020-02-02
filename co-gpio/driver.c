/*
	This file implements all the function needed to acces the GPIO pins
	o a Raspberry PI.

	This file is written by Frank Bekema, All rights are reserved.
*/

#include "driver.h"

void gpio_debug_message(const char * message) {
	printf("co-gpio debug: %s\n", message);
}

bool gpio_check_pin(gpio_pin* pin) {
	if(pin == NULL) {
		gpio_debug_message("gpio_check_pin(): The provided gpio pin is invalid.");
		return false;
	}

	if(!pin->initialized) {
		gpio_debug_message("gpio_check_pin(): The provided gpio pin is not initialized.");
		return false;
	}
	if(!gpio_check_device(pin->device)) {
		gpio_debug_message("gpio_check_pin(): The provided device is not valid");
		return false;
	}
	return true;
}

bool gpio_check_device(gpio_device device) {
	if(!device.initialized) {
		gpio_debug_message("gpio_check_device(): The device is not initialized");
		return false;
	}
	return true;
}

const char * p_gpio_pin_folder_string(gpio_device device, int pin) {
	if(!gpio_check_device(device)) {
		gpio_debug_message("p_gpio_pin_folder_string(): GPIO device not valid");
		return NULL;
	}
	int gpio_number_string_length = digit_count(device.start_pin + pin);
	char gpio_number_string[gpio_number_string_length];
	sprintf(gpio_number_string, "%d", device.start_pin + pin);
	
	int gpio_folder_strlen = strlen(GPIO_PATH) + gpio_number_string_length + 6;
	char* gpio_folder = (char*)malloc(gpio_folder_strlen);
	strcpy(gpio_folder, GPIO_PATH);
	strcat(gpio_folder, "gpio");
	strcat(gpio_folder, gpio_number_string);
	strcat(gpio_folder, "/");
	return gpio_folder;
}

gpio_device* gpio_get_device(char const * name, int start_pin) {	
	char gpio_folder[strlen(GPIO_PATH) + strlen(name)];
	strcpy(gpio_folder, GPIO_PATH);
	strcat(gpio_folder, name);
	
	if(access(gpio_folder, F_OK) == -1) {
		gpio_debug_message("gpio_get_device(): Couldn't find GPIO folder");
		gpio_debug_message(gpio_folder);
		return NULL;
	}

	char gpio_folder_pin[strlen(gpio_folder) + 7];
	strcpy(gpio_folder_pin, gpio_folder);
	strcat(gpio_folder_pin, "/ngpio");

	FILE *p_pin_amount_file = fopen(gpio_folder_pin, "r");
	if(p_pin_amount_file == NULL) {
		gpio_debug_message("gpio_get_device(): Couldn't open the file");
		return NULL;
	}

	int length = 5;
	char pin_amount[length];
	size_t successfull_reads = fread(pin_amount, 1, length, p_pin_amount_file);
	if(successfull_reads < 1) {
		gpio_debug_message("gpio_get_device(): Couldn't read the file");
		return NULL;
	}
	
	fclose(p_pin_amount_file);

	gpio_device* device = (gpio_device*) malloc(sizeof(gpio_device));
	device->name = name;
	device->start_pin = start_pin;
	device->pin_amount = atoi(pin_amount);
	device->initialized = true;
	return device;
}

bool gpio_export(int pin_number) {
	char export_file_path[strlen(GPIO_PATH) + 5];
	strcpy(export_file_path, GPIO_PATH);
	strcat(export_file_path, "export");
	
	if(access(export_file_path, F_OK) == -1) {
		gpio_debug_message("gpio_export(): File does not exist");
		return false;
	}

	FILE* p_export_file = fopen(export_file_path, "w");
	if(p_export_file == NULL) {
		gpio_debug_message("gpio_export(): Couldn't open export file");
		return false;
	}

	int gpio_pin_count = digit_count(pin_number);
        char gpio_pin_string[gpio_pin_count];
	sprintf(gpio_pin_string, "%d", pin_number);
	size_t write_amount = fwrite(gpio_pin_string, gpio_pin_count, 1, p_export_file);
	if(write_amount < 1) {
		gpio_debug_message("gpio_export(): Couldn't write to export file");
		return false;
	}
	fclose(p_export_file);
	return true;	
}

bool gpio_unexport(int pin_number) {
	char unexport_file_path[strlen(GPIO_PATH) + 8];
	strcpy(unexport_file_path, GPIO_PATH);
	strcat(unexport_file_path, "unexport");

	if(access(unexport_file_path, F_OK) == -1) {
		gpio_debug_message("gpio_unexport(): File does not exist");
		return false;
	}

	FILE* p_unexport_file = fopen(unexport_file_path, "w");
	if(p_unexport_file == NULL) {
		gpio_debug_message("gpio_unexport(): Couldn't open unexport file");
		return false;
	}
	int gpio_pin_strlen = digit_count(pin_number);
	char gpio_pin_string[gpio_pin_strlen];
	sprintf(gpio_pin_string, "%d", pin_number);
	size_t write_amount = fwrite(gpio_pin_string, gpio_pin_strlen, 1, p_unexport_file);
	if(write_amount < 1) {
		gpio_debug_message("gpio_unexport(): Couldn't write to unexport file");
		return false;
	}
	fclose(p_unexport_file);
	return true;
}

bool gpio_setmode(gpio_pin* pin, int mode) {	
	if(!gpio_check_pin(pin)) {
		gpio_debug_message("gpio_setmode(): Pin is invalid");
		return false;
	}
	char* p_mode_text;
	if(mode == GPIO_IN) {
		p_mode_text = (char*)malloc(3);
		strcpy(p_mode_text, "in");
	} else if(mode == GPIO_OUT) {
		p_mode_text = (char*)malloc(4);
		strcpy(p_mode_text, "out");
	} else {
		gpio_debug_message("gpio_setmode(): No valid GPIO mode was passed");
		return false;
	}

	const char* p_folder_string = p_gpio_pin_folder_string(pin->device, pin->pin_number);	
	const char* p_direct_folder = p_string_append(p_folder_string, "direction");	
	
	if(access(p_direct_folder, F_OK) == -1) {
		gpio_debug_message("gpio_setmode(): Direction file was not found");
		return false;
	}
	
	FILE* p_direction_file = fopen(p_direct_folder, "w");
	if(p_direction_file == NULL) {
		gpio_debug_message("gpio_setmode(): Couldn't open direction file");
		return false;
	}
	size_t write_amount = fwrite(p_mode_text, strlen(p_mode_text), 1, p_direction_file);
	if(write_amount < 0) {
		gpio_debug_message("gpio_setmode(): Couldn't write to export file");
		return false;
	}

	pin->mode = mode; 

	free(p_mode_text);
	free((char*)p_folder_string);
	free((char*)p_direct_folder);
	fclose(p_direction_file);
	return true;
}

gpio_pin* p_gpio_open(gpio_device* device, int pin, int mode) {
	if(!gpio_check_device(*device)) {
		gpio_debug_message("p_gpio_open(): Device is not valid");
		return NULL;
	}
	if(mode != GPIO_IN && mode != GPIO_OUT) {
		mode = GPIO_IN;
	}
	if(!gpio_export(device->start_pin + pin)) {
		gpio_debug_message("p_gpio_open(): Couldn't export pin");
		return NULL;
	}
	gpio_pin* p_pin = (gpio_pin*) malloc(sizeof(gpio_pin));
	p_pin->device = *device;
	p_pin->pin_number = pin;	
	p_pin->initialized = true;
	if(mode == GPIO_IN || mode == GPIO_OUT) {
		if(!gpio_setmode(p_pin, mode)) {
			gpio_unexport(device->start_pin + pin);
			free(p_pin);
			gpio_debug_message("p_gpio_open(): Couldn't set gpio mode");
			return NULL;
		}	
	}
	if(mode == GPIO_OUT) {
		gpio_write(p_pin, GPIO_LOW);	
	}
	return p_pin;
}

bool gpio_close(gpio_pin* pin) {	
	if(!gpio_check_pin(pin)) {
		gpio_debug_message("gpio_close(): Check if the pin is valid");
		return false;
	}
	if(!gpio_unexport(pin->device.start_pin + pin->pin_number)) {
		gpio_debug_message("gpio_close(): Couldn't unexport pin");
		return false;
	}
	free(pin);
	return true;
}

bool gpio_file_extractor(char** result, FILE* file) {	
	size_t size = 8;
	char* new_result = (char*)malloc(sizeof(char) * size); 
	char current_char[1];	
	size_t current_size = 0;
	
	while(true) {
		int successfull_reads = fread(current_char, 1, 1, file);
		if(successfull_reads != 1) {
			break;
		}
		if(isspace(current_char[0]) || current_char[0] == '\0') {
			new_result[current_size] = '\0';
			break;
		}
		new_result[current_size] = current_char[0];
		current_size++;
		if(current_size > size) {
			size = size * 2;
			char* p_new_val = (char*)malloc(size);
			strcat(p_new_val, *result);
			free(*result);
			new_result = p_new_val;
		}
	}
	*result = new_result;
	rewind(file);
	return true;
}

int gpio_get_direction(gpio_pin* pin) {	
	if(!gpio_check_pin(pin)) {
		gpio_debug_message("gpio_get_direction(): Pin is invalid");
		return GPIO_INVALID;
	}
 	char* p_pin_string = p_inttstr(pin->device.start_pin + pin->pin_number);
	char* p_direction_location = (char*)malloc(0);
	p_direction_location[0] = '\0';
	
	string_buffer_append(&p_direction_location, GPIO_PATH);
	string_buffer_append(&p_direction_location, "gpio");
	string_buffer_append(&p_direction_location, p_pin_string);
	string_buffer_append(&p_direction_location, "/direction");

	if(access(p_direction_location, F_OK) == -1) {
		gpio_debug_message("gpio_get_direction(): Direction file not found");
		return GPIO_INVALID;
	}

	FILE* p_direction_file = fopen(p_direction_location, "r");
	if(p_direction_file == NULL) {
		gpio_debug_message("gpio_get_direction(): Direction file could not be opened");
		return GPIO_INVALID;
	}
		
	char* p_direction;
	gpio_file_extractor(&p_direction, p_direction_file); 

	int res = 0;		
	if(strcmp(p_direction, "in") == 0) {
		res = 1;
	} else if(strcmp(p_direction, "out") == 0) {
		res = 2;
	} else {
		res = 0;
	}

	fclose(p_direction_file);
	free(p_direction_location);
	free(p_pin_string);
	free(p_direction);

	if(res == 1) {
		return GPIO_IN; 
	} else if (res == 2) {
		return GPIO_OUT;
	} else {
		return GPIO_INVALID;
	}
}

void gpio_write(gpio_pin* pin, int value) {	
	if(!gpio_check_pin(pin)) {
		gpio_debug_message("gpio_write(): Pin is invalid");
		return;
	}
	char* p_pin_string = p_inttstr(pin->device.start_pin + pin->pin_number);	
	char* p_value_location = (char*)malloc(0);
	p_value_location[0] = '\0';	

	string_buffer_append(&p_value_location, GPIO_PATH);
	string_buffer_append(&p_value_location, "gpio");
	string_buffer_append(&p_value_location, p_pin_string);
	string_buffer_append(&p_value_location, "/value");
	
	if(gpio_get_direction(pin) != GPIO_OUT) {
		gpio_debug_message("gpio_write(): You can't write on an non OUTPUT pin");
		return;
	}
	
	if(access(p_value_location, F_OK) == -1) {
		gpio_debug_message("gpio_write(): Couldn't find the value file");
		return;
	}
	
	FILE* value_file = fopen(p_value_location, "w");
	if(value_file == NULL) {
		gpio_debug_message("gpio_write(): Couldn't open value file.");
		return;
	}
	
	char* p_value_string = p_inttstr(value);
	int successfull_writes = fwrite(p_value_string, strlen(p_value_string), 1, value_file);
	if(successfull_writes < 1) {
		gpio_debug_message("gpio_write(): Couldn't write to value file.");
		return;
	}

	fclose(value_file);
	free(p_pin_string); 
	free(p_value_location);
	free(p_value_string);
}

int gpio_read(gpio_pin* pin) {	
	if(!gpio_check_pin(pin)) {
		gpio_debug_message("gpio_read(): Pin is invalid");
		return -1;
	}
	char* p_pin_string = p_inttstr(pin->device.start_pin + pin->pin_number);
	char* p_value_location = (char*)malloc(0);
	p_value_location[0] = '\0';
	string_buffer_append(&p_value_location, GPIO_PATH);
	string_buffer_append(&p_value_location, "gpio");
	string_buffer_append(&p_value_location, p_pin_string);
	string_buffer_append(&p_value_location, "/value");
	
	if(gpio_get_direction(pin) != GPIO_IN) {
		gpio_debug_message("gpio_write(): You can't read from a non INPUT pin");
		return -1;
	}

	if(access(p_value_location, F_OK) == -1) {
		gpio_debug_message("gpio_read(): File could not be found");
		return -1;
	}

	FILE* p_value_file = fopen(p_value_location, "r");
	if(p_value_file == NULL) {
		gpio_debug_message("gpio_read(): File could not be opened");
		return -1;
	} 

	char* p_result;
	if(!gpio_file_extractor(&p_result, p_value_file)) {
		gpio_debug_message("gpio_read(): Something went wrong at reading the file");
		return -1;
	}
	
	int int_result = atoi(p_result);

	fclose(p_value_file);
	free(p_result);
	free(p_value_location);
	free(p_pin_string);
	return int_result;
}

bool gpio_set_active_low(gpio_pin* pin, int active_low) {
	if(!gpio_check_pin(pin)) {
		gpio_debug_message("gpio_set_active_low(): GPIO pin is invalid");
		return false;
	}
	char* p_pin_string = p_inttstr(pin->device.start_pin + pin->pin_number);
	char* p_value_location = (char*)malloc(0);
	p_value_location[0] = '\0';
	string_buffer_append(&p_value_location, GPIO_PATH);
	string_buffer_append(&p_value_location, "gpio");
	string_buffer_append(&p_value_location, p_pin_string);
	string_buffer_append(&p_value_location, "/active_low");

	if(access(p_value_location, F_OK) == -1) {
		gpio_debug_message("gpio_set_active_low(): File could not be found");
		return false;
	}

	FILE* p_value_file = fopen(p_value_location, "w");
	if(p_value_file == NULL) {
		gpio_debug_message("gpio_set_active_low(): File could not be opened");
		return false;
	}
	
	char* p_value_string = p_inttstr(active_low); 
	int successfull_writes = fwrite(p_value_string, strlen(p_value_string), 1, p_value_file);
	if(successfull_writes < 1) {
		gpio_debug_message("gpio_set_active_low(): Failed to write to file");
		return false;
	}
	
	pin->active_low = active_low;

	fclose(p_value_file);
	free(p_value_string);
	free(p_value_location);
	free(p_pin_string);

	return true;
}

bool set_edge(gpio_pin* pin, int edge) {
	
}
