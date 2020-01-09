/*
	This file defines all utility functions for this project.

	This file is written by Frank Bekema, All rights are reserved
*/

#ifndef CO_UTILS_H
#define CO_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

const char* p_string_append(const char* string1, const char* string2);
void string_buffer_append(char** buffer, const char* string);
int digit_count(int number);
char* p_inttstr(int number);

#endif
