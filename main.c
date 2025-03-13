#include <stdio.h>
#include <windows.h>
#include "stducp.h"

int main()
{
	const char* portName = "COM3"; // Change this to your appropriate COM port
	HANDLE h = open_com_port(portName);
	if (is_available())
	{
		read_from_com_port();
	}
	return 0;
}
