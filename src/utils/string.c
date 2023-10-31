#include "string.h"

int strcmp(const char *s1, const char *s2){
	register int difference;
	while(1){
		difference = *s1 - *s2;
		if(difference)
			return difference;
		if(*s1 == '\0')
			return 0;
		s1++;
		s2++;
	}
}
