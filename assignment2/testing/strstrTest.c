#include <stdio.h>
#include <string.h>

int main() {

	char haystack[] = "this is a test\napples and oranges\n";
	char needle[] = "oranges";
	//char needle[] = "nope\n";

	char *newString;

	newString = strstr(haystack, needle);

	if(newString)
		printf("It's a match!\n");
	else
		printf("No match\n");
	
	printf("The substring is: %s\n", newString);

	return 0;
}
