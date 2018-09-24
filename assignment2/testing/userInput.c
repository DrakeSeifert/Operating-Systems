#include <stdio.h>
#include <string.h>

int main() {
	
	char myInput[64];
	printf("Enter something: ");
	if(fgets(myInput, sizeof(myInput), stdin))
		printf("\nYou wrote:\n%s", myInput);

	return 0;
}
