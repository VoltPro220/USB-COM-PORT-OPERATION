#include "src/stducp.h"
#include <conio.h>

int main(void)
{
	const char* portName = "COM3"; // Change this to your appropriate COM port
	open_com_port(portName);
	char* buf = "";
	char bufToSend[BUFFER_SIZE] = "";
	int data = 0;
	long time = 0;
	int st = 0;
	_Bool run = TRUE;
	while (run)
	{
		if (is_available())
		{
			buf = read_from_com_port();
			int a = sscanf(buf, "%d,%ld,%d;", &data, &time, &st);
			free(buf);
			printf("DATA: %d, TIME: %ld STATUS: %d\n", data, time, st);
			if (_kbhit())
			{
				switch (_getch())
				{
					case 'a':
						st = ~st;
						int a = sprintf(bufToSend, "%d,%ld,%d;", data, time, st);
						write_to_com_port(bufToSend);
						break;
					case 's':
						time = 0;
						int a2 = sprintf(bufToSend, "%d,%ld,%d;", data, time, st);
						write_to_com_port(bufToSend);
						break;
					case 'x':
						run = FALSE;
						break;
				}
			}
		}
	}

	return 0;
}



