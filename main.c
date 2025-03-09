#include <stdio.h>
#include <windows.h>
#include <conio.h>

#define BUFFER_SIZE 256

// Define for output in stderr errors
// Comment if you don`t need get an error in stderr
#define OUTERR

HANDLE hCom;
HANDLE OpenComPort(const char* portName)
{
	// Open the specified COM port
	hCom = CreateFileA(portName,
					   GENERIC_READ | GENERIC_WRITE,
					   0, // No sharing
					   NULL, // Default security
					   OPEN_EXISTING,
					   0, // No attributes
					   NULL); // No template file

	// Check if the COM port was opened successfully
	if (hCom == INVALID_HANDLE_VALUE)
	{
#if defined(OUTERR)
		printf("Error opening COM port: %d\n", GetLastError());
#endif
		return INVALID_HANDLE_VALUE;
	}

	// Setup the COM port parameters
	DCB dcb;
	if (!GetCommState(hCom, &dcb))
	{
#if defined(OUTERR)
		printf("Error getting COM state: %d\n", GetLastError());
#endif
		CloseHandle(hCom);
		return INVALID_HANDLE_VALUE;
	}

	// Set parameters (baud rate, byte size, stop bits, parity)
	dcb.BaudRate = CBR_9600; // Baud rate
	dcb.StopBits = ONESTOPBIT; // One stop bit
	dcb.Parity = NOPARITY; // No parity
	dcb.ByteSize = 8; // 8 data bits

	// Set the COM port settings
	if (!SetCommState(hCom, &dcb))
	{
#if defined(OUTERR)
		printf("Error setting COM state: %d\n", GetLastError());
#endif
		CloseHandle(hCom);
		return INVALID_HANDLE_VALUE;
	}

	// Optionally, set timeouts
	COMMTIMEOUTS timeouts;
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;

	if (!SetCommTimeouts(hCom, &timeouts))
	{
#if defined(OUTERR)
		printf("Error setting timeouts: %d\n", GetLastError());
#endif
		CloseHandle(hCom);
		return INVALID_HANDLE_VALUE;
	}

	// Return the handle to the opened COM port
	return hCom;
}

void read_from_com_port()
{
	char buffer[BUFFER_SIZE];
	DWORD bytesRead;
	if (!ReadFile(hCom, buffer, BUFFER_SIZE - 1, &bytesRead, NULL))
	{
#if defined(OUTERR)
		fprintf(stderr, "Error reading from COM port: %ld\n", GetLastError());
#endif
		return 0;
	}

	buffer[bytesRead] = '\0'; // Null-terminate the string
	printf("%s", buffer);
}

unsigned long is_available()
{
	DWORD errors;
	COMSTAT cs;
	unsigned long res = 0;
	if (ClearCommError(hCom, &errors, &cs))
	{
		if (cs.cbInQue > 0)
		{
			res = cs.cbInQue;
		}
	}
	else
	{
#if defined(OUTERR)
		fprintf(stderr, "Error checking the port status: %ld\n", GetLastError());
#endif
	}
	return res;
}

short write_to_com_port(const char* data)
{
	DWORD bytesWritten;
	if (!WriteFile(hCom, data, strlen(data), &bytesWritten, NULL))
	{
#if defined(OUTERR)
		fprintf(stderr, "Error writing to COM port: %ld\n", GetLastError());
#endif
		return 0;
	}
	return 1;
}

int main()
{
	const char* portName = "COM3"; // Change this to your appropriate COM port

	HANDLE hCom = OpenComPort(portName);
	if (hCom != INVALID_HANDLE_VALUE)
	{
		printf("COM port opened successfully\n");

		// Perform reading/writing operations here...

		while (TRUE)
		{
			if(is_available())
				read_from_com_port();
			if (_kbhit())
			{
				char t = _getch();
				if(t != 120)
					write_to_com_port("Test");
				if (t == 120)
					break;
			}
		}

		// Close the COM port when done
		CloseHandle(hCom);
	}

	return 0;
}
