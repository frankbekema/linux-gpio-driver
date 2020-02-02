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

char* p_gpio_pin_folder_string(gpio_device device, int pin) {
	int gpio_number_string_length;
	int gpio_folder_strlen;
	char* gpio_folder;

	if(!gpio_check_device(device)) {
		gpio_debug_message("p_gpio_pin_folder_string(): GPIO device not valid");
		return NULL;
	}
	gpio_number_string_length = digit_count(device.start_pin + pin);
	char gpio_number_string[gpio_number_string_length];
	sprintf(gpio_number_string, "%d", device.start_pin + pin);
	
	gpio_folder_strlen = strlen(GPIO_PATH) + gpio_number_string_length + 6;
	gpio_folder = (char*)malloc(gpio_folder_strlen);
	strcpy(gpio_folder, GPIO_PATH);
	strcat(gpio_folder, "gpio");
	strcat(gpio_folder, gpio_number_string);
	strcat(gpio_folder, "/");
	return gpio_folder;
}

gpio_device* gpio_get_device(char const * name, int start_pin) {	
	char gpio_folder[strlen(GPIO_PATH) + strlen(name)];
	FILE* p_pin_amount_file;	
	int length;
	char pin_amount[length];	
	size_t successfull_reads;

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

	p_pin_amount_file = fopen(gpio_folder_pin, "r");
	if(p_pin_amount_file == NULL) {
		gpio_debug_message("gpio_get_device(): Couldn't open the file");
		return NULL;
	}

	length = 5;
	successfull_reads = fread(pin_amount, 1, length, p_pin_amount_file);
	if(successfull_reads < 1) {
		gpio_debug_message("gpio_get_device(): Couldn't read the file");
		fclose(p_pin_amount_file);
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
	FILE* p_export_file;
	int gpio_pin_count;
	size_t write_amount;	

	strcpy(export_file_path, GPIO_PATH);
	strcat(export_file_path, "export");
	
	if(access(export_file_path, F_OK) == -1) {
		gpio_debug_message("gpio_export(): File does not exist");
		return false;
	}

	p_export_file = fopen(export_file_path, "w");
	if(p_export_file == NULL) {
		gpio_debug_message("gpio_export(): Couldn't open export file");
		return false;
	}

	gpio_pin_count = digit_count(pin_number);
        char gpio_pin_string[gpio_pin_count];
	sprintf(gpio_pin_string, "%d", pin_number);
	write_amount = fwrite(gpio_pin_string, gpio_pin_count, 1, p_export_file);
	if(write_amount < 1) {
		gpio_debug_message("gpio_export(): Couldn't write to export file");
		fclose(p_export_file);
		return false;
	}
	fclose(p_export_file);
	return true;	
}

bool gpio_unexport(int pin_number) {
	char unexport_file_path[strlen(GPIO_PATH) + 8];
	FILE* p_unexport_file;
	int gpio_pin_strlen;
	size_t write_amount;	

	strcpy(unexport_file_path, GPIO_PATH);
	strcat(unexport_file_path, "unexport");

	if(access(unexport_file_path, F_OK) == -1) {
		gpio_debug_message("gpio_unexport(): File does not exist");
		return false;
	}

	p_unexport_file = fopen(unexport_file_path, "w");
	if(p_unexport_file == NULL) {
		gpio_debug_message("gpio_unexport(): Couldn't open unexport file");
		return false;
	}
	gpio_pin_strlen = digit_count(pin_number);
	char gpio_pin_string[gpio_pin_strlen];
	sprintf(gpio_pin_string, "%d", pin_number);
	write_amount = fwrite(gpio_pin_string, gpio_pin_strlen, 1, p_unexport_file);
	if(write_amount < 1) {
		gpio_debug_message("gpio_unexport(): Couldn't write to unexport file");
		fclose(p_unexport_file);
		return false;
	}
	fclose(p_unexport_file);
	return true;
}

bool gpio_setmode(gpio_pin* pin, int mode) {	
	char* p_mode_text;
	char* p_folder_string;
	char* p_direct_folder;
	FILE* p_direction_file;
	size_t write_amount;

	if(!gpio_check_pin(pin)) {
		gpio_debug_message("gpio_setmode(): Pin is invalid");
		return false;
	}
	
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

	p_folder_string = p_gpio_pin_folder_string(pin->device, pin->pin_number);
	p_direct_folder = p_string_append(p_folder_string, "direction");	
	
	if(access(p_direct_folder, F_OK) == -1) {
		gpio_debug_message("gpio_setmode(): Direction file was not found");
		free(p_mode_text);
		free(p_folder_string);
		free(p_direct_folder);
		return false;
	}
	
	p_direction_file = fopen(p_direct_folder, "w");
	if(p_direction_file == NULL) {
		gpio_debug_message("gpio_setmode(): Couldn't open direction file");
		free(p_mode_text);
		free(p_folder_string);
		free(p_direct_folder);
		return false;
	}
	write_amount = fwrite(p_mode_text, strlen(p_mode_text), 1, p_direction_file);
	if(write_amount < 0) {
		gpio_debug_message("gpio_setmode(): Couldn't write to export file");
		free(p_mode_text);
		free(p_folder_string);
		free(p_direct_folder);
		fclose(p_direction_file);
		return false;
	}

	pin->mode = mode; 

	free(p_mode_text);
	free(p_folder_string);
	free(p_direct_folder);
	fclose(p_direction_file);
	return true;
}

gpio_pin* p_gpio_open(gpio_device* device, int pin, int mode) {
	gpio_pin* p_pin;

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
	p_pin = (gpio_pin*) malloc(sizeof(gpio_pin));
	p_pin->device = *device;
	p_pin->pin_number = pin;
	p_pin->edge = 0;	
	p_pin->initialized = true;
	if(gpio_has_edge(p_pin)) {
		if(!gpio_set_edge(p_pin, GPIO_EDGE_NONE)) {
			gpio_debug_message("p_gpio_open(): Couldn't set the edge");
			free(p_pin);
			return NULL;
		}
	}
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
		free(pin);
		return false;
	}
	if(!gpio_unexport(pin->device.start_pin + pin->pin_number)) {
		gpio_debug_message("gpio_close(): Couldn't unexport pin");
		free(pin);
		return false;
	}
	free(pin);
	return true;
}

bool gpio_file_extractor(char** result, FILE* file) {	
	size_t size;
	char* new_result; 
	char current_char[1];	
	size_t current_size;
	int successfull_reads;
	char* p_new_val;
	
	size = 8;
	new_result = (char*)malloc(sizeof(char) *size);
	current_size = 0;

	while(true) {
		successfull_reads = fread(current_char, 1, 1, file);
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
			p_new_val = (char*)malloc(size);
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
	char* p_pin_string;
	char* p_direction_location;
	FILE* p_direction_file;
	char* p_direction;
	int result;
	
	if(!gpio_check_pin(pin)) {
		gpio_debug_message("gpio_get_direction(): Pin is invalid");
		return GPIO_INVALID;
	}
 	p_pin_string = p_inttstr(pin->device.start_pin + pin->pin_number);
	p_direction_location = (char*)malloc(0);
	p_direction_location[0] = '\0';
	
	string_buffer_append(&p_direction_location, GPIO_PATH);
	string_buffer_append(&p_direction_location, "gpio");
	string_buffer_append(&p_direction_location, p_pin_string);
	string_buffer_append(&p_direction_location, "/direction");

	if(access(p_direction_location, F_OK) == -1) {
		gpio_debug_message("gpio_get_direction(): Direction file not found");
		free(p_direction_location);
		free(p_pin_string);	
		return GPIO_INVALID;
	}

	p_direction_file = fopen(p_direction_location, "r");
	if(p_direction_file == NULL) {
		gpio_debug_message("gpio_get_direction(): Direction file could not be opened");
		
		free(p_direction_location);
		free(p_pin_string);
		return GPIO_INVALID;
	}
		
	gpio_file_extractor(&p_direction, p_direction_file); 
		
	if(strcmp(p_direction, "in") == 0) {
		result = 1;
	} else if(strcmp(p_direction, "out") == 0) {
		result = 2;
	} else {
		result = 0;
	}

	fclose(p_direction_file);
	free(p_direction_location);
	free(p_pin_string);
	free(p_direction);

	if(result == 1) {
		return GPIO_IN; 
	} else if (result == 2) {
		return GPIO_OUT;
	} else {
		return GPIO_INVALID;
	}
}

void gpio_write(gpio_pin* pin, int value) {	
	char* p_pin_string;
	char* p_value_location;
	FILE* value_file;
	char* p_value_string;
	int successfull_writes;
	
	if(!gpio_check_pin(pin)) {
		gpio_debug_message("gpio_write(): Pin is invalid");
		return;
	}
	p_pin_string = p_inttstr(pin->device.start_pin + pin->pin_number);	
	p_value_location = (char*)malloc(0);
	p_value_location[0] = '\0';	

	string_buffer_append(&p_value_location, GPIO_PATH);
	string_buffer_append(&p_value_location, "gpio");
	string_buffer_append(&p_value_location, p_pin_string);
	string_buffer_append(&p_value_location, "/value");
	
	if(gpio_get_direction(pin) != GPIO_OUT) {
		gpio_debug_message("gpio_write(): You can't write on an non OUTPUT pin");
		free(p_pin_string); 
		free(p_value_location);	
		return;
	}
	
	if(access(p_value_location, F_OK) == -1) {
		gpio_debug_message("gpio_write(): Couldn't find the value file");
		free(p_pin_string); 
		free(p_value_location);
		return;
	}
	
	value_file = fopen(p_value_location, "w");
	if(value_file == NULL) {
		gpio_debug_message("gpio_write(): Couldn't open value file.");
		free(p_pin_string); 
		free(p_value_location);
		return;
	}
	
	p_value_string = p_inttstr(value);
	successfull_writes = fwrite(p_value_string, strlen(p_value_string), 1, value_file);
	if(successfull_writes < 1) {
		gpio_debug_message("gpio_write(): Couldn't write to value file.");
		fclose(value_file);
		free(p_pin_string); 
		free(p_value_location);
		free(p_value_string);
		return;
	}

	fclose(value_file);
	free(p_pin_string); 
	free(p_value_location);
	free(p_value_string);
}

int gpio_read(gpio_pin* pin) {	
	char* p_pin_string;
	char* p_value_location;
	FILE* p_value_file;
	char* p_result;
	int int_result;
	
	if(!gpio_check_pin(pin)) {
		gpio_debug_message("gpio_read(): Pin is invalid");
		return -1;
	}
	p_pin_string = p_inttstr(pin->device.start_pin + pin->pin_number);
	p_value_location = (char*)malloc(0);
	p_value_location[0] = '\0';
	string_buffer_append(&p_value_location, GPIO_PATH);
	string_buffer_append(&p_value_location, "gpio");
	string_buffer_append(&p_value_location, p_pin_string);
	string_buffer_append(&p_value_location, "/value");
	
	if(gpio_get_direction(pin) != GPIO_IN) {
		gpio_debug_message("gpio_write(): You can't read from a non INPUT pin");	
		free(p_value_location);
		free(p_pin_string);
		return -1;
	}

	if(access(p_value_location, F_OK) == -1) {
		gpio_debug_message("gpio_read(): File could not be found");	
		free(p_value_location);
		free(p_pin_string);
		return -1;
	}

	p_value_file = fopen(p_value_location, "r");
	if(p_value_file == NULL) {
		gpio_debug_message("gpio_read(): File could not be opened");
		
		free(p_value_location);
		free(p_pin_string);
		return -1;
	} 

	if(!gpio_file_extractor(&p_result, p_value_file)) {
		gpio_debug_message("gpio_read(): Something went wrong at reading the file");
		
		fclose(p_value_file);
		free(p_value_location);
		free(p_pin_string);
		return -1;
	}
	
	int_result = atoi(p_result);

	fclose(p_value_file);
	free(p_result);
	free(p_value_location);
	free(p_pin_string);
	return int_result;
}

bool gpio_set_active_low(gpio_pin* pin, int active_low) {
	char* p_pin_string;
	char* p_value_location;
	FILE* p_value_file;
	char* p_value_string;
	int successfull_writes;

	if(!gpio_check_pin(pin)) {
		gpio_debug_message("gpio_set_active_low(): GPIO pin is invalid");
		return false;
	}
	p_pin_string = p_inttstr(pin->device.start_pin + pin->pin_number);
	p_value_location = (char*)malloc(0);
	p_value_location[0] = '\0';
	string_buffer_append(&p_value_location, GPIO_PATH);
	string_buffer_append(&p_value_location, "gpio");
	string_buffer_append(&p_value_location, p_pin_string);
	string_buffer_append(&p_value_location, "/active_low");

	if(access(p_value_location, F_OK) == -1) {
		gpio_debug_message("gpio_set_active_low(): File could not be found");
		free(p_value_location);
		free(p_pin_string);
		return false;
	}

	p_value_file = fopen(p_value_location, "w");
	if(p_value_file == NULL) {
		gpio_debug_message("gpio_set_active_low(): File could not be opened")	;
		free(p_value_location);
		free(p_pin_string);
		return false;
	}
	
	p_value_string = p_inttstr(active_low); 
	successfull_writes = fwrite(p_value_string, strlen(p_value_string), 1, p_value_file);
	if(successfull_writes < 1) {
		gpio_debug_message("gpio_set_active_low(): Failed to write to file");
		fclose(p_value_file);
		free(p_value_string);
		free(p_value_location);
		free(p_pin_string);
		return false;
	}
	
	pin->active_low = active_low;

	fclose(p_value_file);
	free(p_value_string);
	free(p_value_location);
	free(p_pin_string);

	return true;
}

bool gpio_has_edge(gpio_pin* pin) {
	char* p_pin_string;
	char* p_edge_location;
	p_pin_string = p_inttstr(pin->device.start_pin + pin->pin_number);

	p_edge_location = (char*)malloc(0);
	p_edge_location[0] = '\0';
	string_buffer_append(&p_edge_location, GPIO_PATH);
	string_buffer_append(&p_edge_location, "gpio");
	string_buffer_append(&p_edge_location, p_pin_string);
	string_buffer_append(&p_edge_location, "/edge");
	
	if(access(p_edge_location, F_OK) == 0) {
		free(p_pin_string);
		free(p_edge_location);
		return true;
	} else {
		free(p_pin_string);
		free(p_edge_location);
		return false;
	}
}

bool gpio_set_edge(gpio_pin* pin, int edge) {	
	char* p_pin_string;
	char* p_edge_location;	
	const char* edge_string;
	FILE* p_edge_file;
	int successfull_writes;
	
	if(!gpio_has_edge) {
		gpio_debug_message("gpio_set_edge(): This pin doesn't have support to 					      be an interrupt generating input pin");
		free(p_pin_string);
		free(p_edge_location);
		return false;
	}
	if(!gpio_check_pin(pin)) {
		gpio_debug_message("gpio_set_edge(): GPIO pin is invalid");
		free(p_pin_string);
		free(p_edge_location);
		return false;
	}
	if(edge != GPIO_EDGE_NONE && 
	   edge != GPIO_EDGE_RISING && 
	   edge != GPIO_EDGE_FALLING &&
	   edge != GPIO_EDGE_BOTH) 
	{
		gpio_debug_message("gpio_set_edge(): Edge is not valid");
		free(p_pin_string);
		free(p_edge_location);
		return false;
	}	
	p_pin_string = p_inttstr(pin->device.start_pin + pin->pin_number);
	p_edge_location = (char*)malloc(0);
	p_edge_location[0] = '\0';
	string_buffer_append(&p_edge_location, GPIO_PATH);
	string_buffer_append(&p_edge_location, "gpio");
	string_buffer_append(&p_edge_location, p_pin_string);
	string_buffer_append(&p_edge_location, "/edge");
	
	if(access(p_edge_location, F_OK) == -1)
	{
		gpio_debug_message("gpio_set_edge(): File could not be found");
		free(p_pin_string);
		free(p_edge_location);
		return false;
	}

	edge_string = gpio_get_edge_string(edge);
	if(edge_string == NULL) {
		gpio_debug_message("gpio_set_edge(): Failed to retrive edge string");
		free(p_pin_string);
		free(p_edge_location);
		return false;
	} 
	
	p_edge_file = fopen(p_edge_location, "w");
	if(p_edge_file == NULL) {
		gpio_debug_message("gpio_set_edge(): File could not be opened");
		free(p_pin_string);
		free(p_edge_location);
		return false;
	}
	 
	successfull_writes = fwrite(edge_string, strlen(edge_string), 1, p_edge_file);
	
	if(successfull_writes < 1) {
		gpio_debug_message("gpio_set_edge(): Failed to write to file");
		fclose(p_edge_file);	
		free(p_pin_string);
		free(p_edge_location);
		return false;
	}

	pin->edge = edge;

	fclose(p_edge_file);	
	free(p_pin_string);
	free(p_edge_location);
}

const char* gpio_get_edge_string(int edge) {
	switch(edge) {
		case GPIO_EDGE_NONE:
			return "none";
			break;
		case GPIO_EDGE_RISING:
			return "rising";
			break;
		case GPIO_EDGE_FALLING:
			return "falling";
			break;
		case GPIO_EDGE_BOTH:
			return "both";
			break;
		default:
			gpio_debug_message("get_edge_string(): Provided edge is invalid");
			return NULL;
			break;
	}
}

void gpio_poll(gpio_pin* pin) {
	char* p_pin_string;
	char *p_value_location;
	struct pollfd fds[1];
	int poll_res;

	if(!gpio_check_pin(pin)) {
		gpio_debug_message("gpio_poll(): GPIO pin is invalid");
		return;
	}
	p_pin_string = p_inttstr(pin->device.start_pin + pin->pin_number);
	p_value_location = (char*)malloc(0);
	p_value_location[0] = '\0';
	string_buffer_append(&p_value_location, GPIO_PATH);
	string_buffer_append(&p_value_location, "gpio");
	string_buffer_append(&p_value_location, p_pin_string);
	string_buffer_append(&p_value_location, "/value");

	if(access(p_value_location, F_OK) == -1)
	{
		gpio_debug_message("gpio_poll(): File could not be found");
		free(p_pin_string);
		free(p_value_location);
		return;
	}
	 	
	fds[0].fd = open(p_value_location, O_RDONLY);
	if(fds[0].fd == -1) {
		gpio_debug_message("gpio_poll(): Failed opening file");
		free(p_pin_string);
		free(p_value_location);
		return;
	}
	fds[0].events = POLLPRI | POLLERR;
	
	//Bug fix for the event
	read(fds[0].fd, NULL, 0);
	
	poll_res = poll(fds, 1, -1);	
	
	if(close(fds[0].fd) != 0) {
		gpio_debug_message("gpio_poll(): Failed closing file");
		free(p_pin_string);
		free(p_value_location);
		return;
	}
	free(p_pin_string);
	free(p_value_location);	
}
