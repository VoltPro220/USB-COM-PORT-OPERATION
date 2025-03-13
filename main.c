#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include "stducp.h"

int main()
{
	const char* portName = "COM3"; // Change this to your appropriate COM port
	HANDLE h = open_com_port(portName);
	char* msg = "";
	while (!_kbhit())
	{
		if (is_available())
		{
			msg = read_from_com_port();
			printf("%s", msg);
			free(msg);
		}
	}
	close_com_port();
	return 0;
}
