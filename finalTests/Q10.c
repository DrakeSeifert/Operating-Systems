#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(){
	char threeStr[3] = "ab";
	strcpy(threeStr, "abc");
	printf(threeStr);

	return 0;
}
