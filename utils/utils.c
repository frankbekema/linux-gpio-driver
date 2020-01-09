/*
	This file implements all utility functions for this project.

	This file is wrriten by Frank Bekema, All rights are reserved.
*/

#include "utils.h"

const char* p_string_append(const char* string1, const char * string2) {
	char* string_result = (char*)malloc(strlen(string1) + strlen(string2));
	strcpy(string_result, string1);
	strcat(string_result, string2);
	return string_result;
}

void string_buffer_append(char** buffer, const char * string) {
	char* string_result; 
	string_result = (char*)malloc(strlen(*buffer) + strlen(string));
	strcpy(string_result, *buffer);
	strcat(string_result, string);	
	free(*buffer);
	*buffer = string_result;
}

int digit_count(int number) {
	int count = 0;
	while(number != 0) {
		number /= 10;
		count++;
	}
	return count;
}

char* p_inttstr(int number) {
	int length = digit_count(number);
	char* p_result = (char*)malloc(length);
	sprintf(p_result, "%d", number);
	return p_result;
}
