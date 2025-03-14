#include "SerialPort.h"

void usb::SerialPort::connectToComPort(const char* portName)
{
	this->isConnected = FALSE;

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
#if defined(UCPERR)
		std::cerr << "Error opening COM port: " << GetLastError() << std::endl;
#endif
		return;
	}
	// DCB Structure: A DCB (Device Control Block) structure is used to define the parameters 
	// of the COM port. The baud rate, byte size, stop bits, and parity are set.
	// Setup the COM port parameters
	DCB dcb;
	if (!GetCommState(hCom, &dcb))
	{
#if defined(UCPERR)
		std::cerr << "Error getting COM state: " << GetLastError() << std::endl;
#endif
		CloseHandle(hCom);
		return;
	}

	// Set parameters (baud rate, byte size, stop bits, parity)
	dcb.BaudRate = BAUDRATE; // Baud rate
	dcb.StopBits = STOPBITS; // One stop bit
	dcb.Parity = PARITY; // No parity
	dcb.ByteSize = BYTESIZE; // 8 data bits

	// SetCommState: This function applies the settings to the COM port.
	// Set the COM port settings
	if (!SetCommState(hCom, &dcb))
	{
#if defined(UCPERR)
		std::cerr << "Error setting COM state: " << GetLastError() << std::endl;
#endif
		CloseHandle(hCom);
		return;
	}
	// COMMTIMEOUTS: This structure sets timeouts for read and write operations.
	// Optionally, set timeouts
	COMMTIMEOUTS timeouts{};
	timeouts.ReadIntervalTimeout = READINTERVALTIMEOUT;
	timeouts.ReadTotalTimeoutConstant = READTOTALTIMEOUTCONSTANT;
	timeouts.ReadTotalTimeoutMultiplier = READTOTALTIMEOUTMULTIPLIER;
	timeouts.WriteTotalTimeoutConstant = WRITETOTALTIMEOUTCONSTANT;
	timeouts.WriteTotalTimeoutMultiplier = WRITETOTALTIMEOUTMULTIPLIER;

	if (!SetCommTimeouts(hCom, &timeouts))
	{
#if defined(UCPERR)
		std::cerr << "Error setting timeouts: " << GetLastError() << std::endl;
#endif
		// CloseHandle: Once done with the COM port, it’s important to close the handle to release the resource.
		CloseHandle(hCom);
		return;
	}
	isConnected = TRUE;
}

usb::SerialPort::SerialPort(const char* portName)
{
	this->connectToComPort(portName);
}

usb::SerialPort::SerialPort()
{
	this->isConnected = FALSE;
	this->hCom = NULL;
	portName = NULL;
}

usb::SerialPort::~SerialPort()
{
	this->disconnect();
}

void usb::SerialPort::connect(const char* portName)
{
	if (this->isConnected == TRUE)
		return;
	this->connectToComPort(portName);
}

char* usb::SerialPort::readPort() const
{
	// Reads the requested data ("for reading") bytes to the "buffer"
	// and returns the number of bytes actually read
	if (!this->isConnected)
	{
#ifdef UCPERR
		std::cerr << "The port is closed" << std::endl;
#endif
		return NULL;
	}
	char* buffer = static_cast<char*>(calloc(BUFFER_SIZE, sizeof(char)));
	unsigned long bytesRead = 0;
	if (!ReadFile(hCom, buffer, BUFFER_SIZE - 1, &bytesRead, NULL))
	{
#if defined(UCPERR)
		std::cerr << "Error reading from COM port: \"\\." << portName << "\" " << GetLastError() << std::endl;
#endif
		free(buffer);
		return NULL;
	}
	return buffer;
}

BOOL usb::SerialPort::writePort(const char* data) const
{
	if (!this->isConnected)
	{
#ifdef UCPERR
		std::cerr << "The port is closed" << std::endl;
#endif
		return NULL;
	}
	DWORD bytesWritten;
	if (!WriteFile(hCom, data, strlen(data), &bytesWritten, NULL))
	{
#ifdef UCPERR
		std::cerr << "Error writing to COM port: " << GetLastError() << "\n";
#endif
		return FALSE;
	}
	return TRUE;
}

BOOL usb::SerialPort::isConneted() const
{
	return this->isConnected;
}

void usb::SerialPort::disconnect()
{
	if (this->isConnected)
	{
		this->isConnected = FALSE;
		this->portName = "";
		CloseHandle(hCom);
	}
}

const char* usb::SerialPort::getPortName() const
{
	return this->portName;
}

std::vector<std::string> usb::findAvailableComPorts()
{
	std::vector<std::string> comPorts;
	for (int i = 1; i <= 256; i++)
	{
		std::string portName = "COM" + i;

		HANDLE hPort = CreateFileA(portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

		if (hPort != INVALID_HANDLE_VALUE)
		{
			comPorts.push_back(portName);
			CloseHandle(hPort);
		}
	}
	return comPorts;
}

BOOL usb::SerialPort::isAvailable() const
{
	if (!isConnected)
	{
#ifdef UCPERR
		std::cerr << "The port is closed\n";
#endif
		return -1;
	}
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
#if defined(UCPERR)
		std::cerr << "ERROR: \"\\." << portName << "\" Error checking the port status\n";
#endif
		return -1;
	}
	return res;
}