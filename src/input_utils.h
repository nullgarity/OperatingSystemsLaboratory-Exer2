#ifndef INPUT_UTILS_H
#define INPUT_UTILS_H

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "memory_manager.h"

char* get_input_line(char* buffer, int size);
int is_valid_integer(const char* str);
int get_validated_integer(const char* prompt, int min, int max, int default_value);
void edit_parameters(MemoryManager *mm);

#endif
