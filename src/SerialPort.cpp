#include "SerialPort.h"

void usb::SerialPort::connectToComPort(const char* portName)
{
	this->err = 0;
	this->comStatus = { 0 };
	this->isConnected = FALSE;

	// Creating and opening an input/output descriptor.
	this->serial = CreateFileA(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	// ***************** HANDLE errors *******************
	// Use the Win32 API function to get the latest error.
	// It returns an integer error code. There
	// it contains about 16,000 error codes, we are only interested in two
	// (is the device connected? and is it already being used by another application? ).

	DWORD errMsg = GetLastError();

	// the win32 error code for ERROR_FILE_NOT_FOUND is DWORD 2L
	// Indicates that the device is not connected or has not been found.
	if (errMsg == ERROR_FILE_NOT_FOUND)
	{
#ifdef OUTPUT_ERR
		std::cerr << "ERROR #2L: \"\\." << portName << "\" I/O device NOT found\n";
#else 
		return;
#endif
	}

	// the win32 error code for ERROR_ACCESS_DENIED is DWORD 5L
	// This means that access is denied because it is being used.
	// in another program.
	else if (errMsg == ERROR_ACCESS_DENIED)
	{
#ifdef OUTPUT_ERR
		std::cerr << "ERROR #5L: can`t open: \"\\." << portName << "\"";
#else 
		return;
#endif
	}

	// ****************** COM Port Configuration ****************************
	// If everything is fine, it configures the serial COM port.
	else if (errMsg == 0)
	{
		// DCB is a "Device Control Block", a structure that stores settings
		// distribution kit. Setting the COM port parameters. (You can change them)
		// In the SerialPort.h header file in the definitions
		DCB dcbSerialParameters = { 0 };
		if (!GetCommState(serial, &dcbSerialParameters))
		{
#ifdef OUTPUT_ERR
			std::cerr << "ERROR #4L: \"\\." << portName << "\" Distribution data could not be obtained\n";
#else 
			return;
#endif
		}
		else
		{
			dcbSerialParameters.BaudRate = CBR_BAUD;
			dcbSerialParameters.ByteSize = BYTESIZE;
			dcbSerialParameters.StopBits = STOPBITS;
			dcbSerialParameters.Parity = PARITY;
#ifdef SETFDTRCONTROL
			dcbSerialParameters.fDtrControl = FDTRCONTROL;
#endif

			// Set the parameters, if this fails, an error appears.
			if (!GetCommState(serial, &dcbSerialParameters))
			{
#ifdef OUTPUT_ERR
				std::cerr << "ERROR #3L: \"\\." << portName << "\" Couldn't set parameters\n";
#else 
				CloseHandle(serial);
				return;
#endif
				CloseHandle(serial);
			}
			else
			{
				this->isConnected = TRUE;
				this->portName = portName;
				PurgeComm(serial, PURGE_RXCLEAR | PURGE_TXCLEAR);
				Sleep(WAIT_TIME);
			}
		}

	}
}

usb::SerialPort::SerialPort(const char* portName)
{
	this->connectToComPort(portName);
}

usb::SerialPort::SerialPort()
{
	this->err = 0;
	this->comStatus = { 0 };
	this->isConnected = FALSE;
	this->serial = NULL;
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
	char buffer[256]{}; // Используйте необходимый размер буфера
	DWORD bytesRead;
	if (!ReadFile(serial, buffer, sizeof(buffer) - 1, &bytesRead, NULL))
	{
#if defined(OUTPUT_ERR)
		std::cerr << "ERROR #7L: \"\\." << portName << "\" Couldn't read\n";
#endif
		delete[] buffer;
		return NULL;
	}

	buffer[bytesRead] = '\0';

	char* result = new char[bytesRead + 1];
	strncpy(result, buffer, static_cast<size_t>(bytesRead) + 1);

	return result;
}

BOOL usb::SerialPort::writePort(const char* data) const
{
	DWORD bytesWritten;
	if (!WriteFile(serial, data, strlen(data), &bytesWritten, NULL))
	{
#ifdef OUTPUT_ERR
		std::cerr << "ERROR #6L: \"\\." << this->portName << "\" Couldn't transmit data\n";
#endif
		return FALSE;
	}
	else
	{
		return TRUE;
	}
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
		CloseHandle(serial);
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

BOOL usb::SerialPort::available() const
{
	if (!isConnected)
	{
#ifdef OUTPUT_ERR
		std::cerr << "The port is closed\n";
#endif
		return -1;
	}
	DWORD errors;
	COMSTAT cs;
	unsigned long res = 0;
	if (ClearCommError(serial, &errors, &cs))
	{
		if (cs.cbInQue > 0)
		{
			res = cs.cbInQue;
		}
	}
	else
	{
#if defined(OUTPUT_ERR)
		std::cerr << "ERROR: \"\\." << portName << "\" Error checking the port status\n";
#endif
		return -1;
	}
	return res;
}