#include <string.h>
#include <memory.h>
#include <list.h>
#include <malloc.h>

// implementation of functions

void int_to_ascii(int n, char* str){

	int i, sign; // translation characters
	if ((sign = n) < 0) n = -n;
	i = 0;
	do {
		str[i++] = n % 10 + '0'; // convert integers
	} while ((n /= 10) > 0);
	
	if (sign < 0) str[i++] = '-';
	str[i] = '\0'; // dont forget null terminator

	reverse(str); // reverse string's order
}

void hex_to_ascii(int h, char* str){

	append(str, '0'); // append the 
	append(str, 'x'); // 0x

	char zeros = 0;   // set a zero char

	int tmp, i;
	for (i = 28; i > 0; i -= 4){
		tmp = (h >> i) & 0xF;
		if (tmp == 0 && zeros == 0) continue;
		zeros = 1;
		if (tmp > 0xA) append(str, tmp - 0xA + 'a');
		else append(str, tmp + '0');
	}
	
	tmp = h & 0xF;
	if (tmp >= 0xA) append(str, tmp - 0xA);
	else append(str, tmp + '0');

}

char *strncpy(char *destString, const char *sourceString,int maxLength)
{
    unsigned count;

    if ((destString == (char *) NULL) || (sourceString == (char *) NULL))
        {
            return (destString = NULL);
        }

    if (maxLength > 255)
        maxLength = 255;

    for (count = 0; (int)count < (int)maxLength; count ++)
        {
            destString[count] = sourceString[count];

            if (sourceString[count] == '\0')
                break;
        }

    if (count >= 255)
        {
            return (destString = NULL);
        }

    return (destString);
}

void reverse(char* str){

	int c, i, j;
	for (i = 0, j = strlen(str)-1; i < j; i++, j--){
		c = str[i];
		str[i] = str[j];
		str[j] = c;
	}

}

int strlen(char* str){
	int i = 0;
	while (str[i] != '\0') ++i;
	return i;
}

void append(char* str, char ch){

	int len = strlen(str);
	str[len] = ch;
	str[len+1] = '\0';
}

void backspace(char* str){

	int len = strlen(str);
	str[len-1] = '\0';
}

int strcmp(char* str1, char* str2){

	int i;
	for (i = 0; str1[i] == str2[i]; i++)
		if (str1[i] == '\0') return 0;

	return str1[i] - str2[i];
}

int strncmp( const char* s1, const char* s2, int c ) {
    int result = 0;

    while ( c ) {
        result = *s1 - *s2++;

        if ( ( result != 0 ) || ( *s1++ == 0 ) ) {
            break;
        }

        c--;
    }

    return result;
}

void itoa(char *buf, unsigned long int n, int base)
{
    unsigned long int tmp;
    int i, j;

    tmp = n;
    i = 0;

    do {
        tmp = n % base;
        buf[i++] = (tmp < 10) ? (tmp + '0') : (tmp + 'a' - 10);
    } while (n /= base);
    buf[i--] = 0;

    for (j = 0; j < i; j++, i--) {
        tmp = buf[j];
        buf[j] = buf[i];
        buf[i] = tmp;
    }
}

int atoi(char * string) {
    int result = 0;
    unsigned int digit;
    int sign;

    while (isspace(*string)) {
        string += 1;
    }

    /*
     * Check for a sign.
     */

    if (*string == '-') {
        sign = 1;
        string += 1;
    } else {
        sign = 0;
        if (*string == '+') {
            string += 1;
        }
    }

    for ( ; ; string += 1) {
        digit = *string - '0';
        if (digit > 9) {
            break;
        }
        result = (10*result) + digit;
    }

    if (sign) {
        return -result;
    }
    return result;
}

int isspace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r';
}

void
strcat(void* dest, const void* src)
{
    char* end = (char*) dest + strlen(dest);
    memcpy((char*) end, (char*) src, strlen((char*) src));
    end = end + strlen((char*) src);
    *end = '\0';
}

char* strdup(const char* src)
{
    int len = strlen(src) + 1;
    char* dst = malloc(len);
    memcpy(dst, src, len);
    return dst;
}


char *strsep(char **stringp, const char *delim) {
    char *s;
    const char *spanp;
    int c, sc;
    char *tok;
    if ((s = *stringp) == NULL)
        return (NULL);
    for (tok = s;;) {
        c = *s++;
        spanp = delim;
        do {
            if ((sc = *spanp++) == c) {
                if (c == 0)
                    s = NULL;
                else
                    s[-1] = 0;
                *stringp = s;
                return (tok);
            }
        } while (sc != 0);
    }
}

char * strstr(const char *in, const char *str) {
    char c;
    uint32_t len;

    c = *str++;
    if (!c)
        return (char *) in;

    len = strlen(str);
    do {
        char sc;

        do {
            sc = *in++;
            if (!sc)
                return (char *) 0;
        } while (sc != c);
    } while (strncmp(in, str, len) != 0);

    return (char *) (in - 1);
}

list_t * str_split(const char * str, const char * delim, unsigned int * numtokens) {
    list_t * ret_list = list_create();
    char *s = strdup(str);
    char *token, *rest = s;
    while ((token = strsep(&rest, delim)) != NULL) {
        if(!strcmp(token, ".")) continue;
        if(!strcmp(token, "..")) {
            if(list_size(ret_list) > 0) list_pop(ret_list);
            continue;
        }
        list_push(ret_list, strdup(token));
        if(numtokens) (*numtokens)++;
    }
    free(s);
    return ret_list;
}
/*
 * Reconstruct the string with tokens and delimiters
 * */
char * list2str(list_t * list, const char * delim) {
    char * ret = malloc(256);
    memset(ret, 0, 256);
    int len = 0, ret_len = 256;
    while(list_size(list)> 0) {
        char * temp = list_pop(list)->val;
        int len_temp = strlen(temp);
        if(len + len_temp + 1 + 1 > ret_len) {
            ret_len = ret_len * 2;
            free(ret);
            ret = malloc(ret_len);
            len = len + len_temp + 1;
        }
        strcat(ret, delim);
        strcat(ret, temp);
    }
    return ret;
}
