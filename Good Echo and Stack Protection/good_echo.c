#include <stdio.h>
// Noble Thomas

void good_echo() {

char buffer[8];
char *ret;

while(1) { 
	ret = fgets(buffer, 8,stdin);
	if(ret == NULL) {
	break;
	}
	fputs(ret, stdout);
 }
return;
}

int main() {
good_echo();
return 0;
}
