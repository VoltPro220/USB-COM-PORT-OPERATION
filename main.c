#include "src/stducp.h"
#include <conio.h>

int main(void)
{
	int num = 0;
	while ((num = find_available_com_ports()) != 0)
	{
		if (num != -1)
			printf("COM%d is available\n", num);
	}


	return 0;
}



