#include <stdio.h>

int main(int argc, char**  argv)
{
	int i = 1;

	if ( argc != 2 )
	{
		printf("error!\n");
		return -1;
	}
	printf("i = %d\n", i);
	printf("input %s\n", argv[0]);
	return 0;
}
