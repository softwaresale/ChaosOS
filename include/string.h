#ifndef STRING_H
#define STRING_H 1

#include <list.h>

/* Functions for dealing with strings */

// converts an integer to a string
// PARAM n   -- number to be converted
// PARAM str -- where to put converted string
void int_to_ascii(int n, char* str);

// converts a hexidecimal to a string
// PARAM h   -- hex to be converted
// PARAM str -- where to put converted hex
void hex_to_ascii(int h, char* str);

// reverses a string
void reverse(char* str);

// finds the size of a string
int strlen(char* str);

// parses out the trailing backspace of a string
void backspace(char* str);

// appends a char to a string
// PARAM str -- string to append to
// PARAM ch  -- what to append
void append(char* str, char ch);

// compared two strings. if 1 == 2, ret 0; if 1 > 2, ret 1, if 1 < 2, ret -1
int strcmp(char* str1, char* str2);

char *strncpy(char *destString, const char *sourceString,int maxLength);

int strncmp( const char* s1, const char* s2, int c );

int isspace(char c);

void itoa(char* buf, unsigned long int n, int base);

int atoi(char* string);

void strcat(void* dest, const void* src);

char * strstr(const char *in, const char *str);

char *strsep(char **stringp, const char *delim);

char* strdup(const char* src);

list_t* str_split(const char*, const char*, unsigned int*);

#endif
