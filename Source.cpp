#include "src/SerialPort.h"
#include <conio.h>
int main(void)
{
	usb::SerialPort port("COM3");

	char* buf{};
	char bufToSend[BUFFER_SIZE] = "";
	int data = 0;
	long time = 0;
	int st = 0;
	bool run = true;
	while (run)
	{
		if (port.isAvailable())
		{
			buf = port.readPort();
			int a = sscanf(buf, "%d,%ld,%d;", &data, &time, &st);
			delete[] buf;
			printf("DATA: %d, TIME: %ld STATUS: %d\n", data, time, st);
			if (_kbhit())
			{
				switch (_getch())
				{
					case 'a':
						st = ~st;
						sprintf(bufToSend, "%d,%ld,%d;", data, time, st);
						port.writePort(bufToSend);
						break;
					case 's':
						time = 0;
						sprintf(bufToSend, "%d,%ld,%d;", data, time, st);
						port.writePort(bufToSend);
						break;
					case 'x':
						run = FALSE;
						break;
					default:
						break;
				}
			}
		}
	}

	return 0;
}