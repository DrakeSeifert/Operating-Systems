#include <stdio.h>

void func2(int *var)
{
	(*var)++;
}

void func1(int *var)
{
	func2(var);
}

int main()
{
	int var  = 5;
	printf("main() var == %d\n", var);
	func1(&var);
	printf("main() var == %d\n", var);
	return 0;
}
