#pragma once

char* str_dup(const char*);
void remove_quotes(char** str);
char* escape_special_chars(const char* str);

unsigned long hash_int(int value);
unsigned long hash_float(float value);
unsigned long hash_double(double value);
unsigned long hash_char(char value);
unsigned long hash_string(const char* value);
