#include "stducp.h"

int main(void)
{
	const char* portName = "COM4"; // Change this to your appropriate COM port
	HANDLE h = open_com_port(portName);
	char message = "HELLO WORLD";

	write_to_com_port(message);

	close_com_port();

	return 0;
}



