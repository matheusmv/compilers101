#pragma once

char* str_dup(const char*);

unsigned long hash_int(int value);
unsigned long hash_float(float value);
unsigned long hash_double(double value);
unsigned long hash_char(char value);
unsigned long hash_string(char* value);