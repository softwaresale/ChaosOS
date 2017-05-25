#include <string.h>

// implementation of functions

void int_to_ascii(int n, char str[]){

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

void hex_to_ascii(int h, char str[]){

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

void reverse(char str[]){

	int c, i, j;
	for (i = 0, j = strlen(str)-1; i < j; i++, j--){
		c = str[i];
		str[i] = str[j];
		str[j] = c;
	}

}

int strlen(char str[]){
	int i = 0;
	while (str[i] != '\0') ++i;
	return i;
}

void append(char str[], char ch){

	int len = strlen(str);
	str[len] = ch;
	str[len+1] = '\0';
}

void backspace(char str[]){

	int len = strlen(str);
	str[len-1] = '\0';
}

int strcmp(char str1[], char str2[]){

	int i;
	for (i = 0; str1[i] == str2[i]; i++)
		if (str1[i] == '\0') return 0;

	return str1[i] - str2[i];
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

