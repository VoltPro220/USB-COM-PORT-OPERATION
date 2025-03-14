#pragma once

#ifndef _SERIALPORT_H_
#define _SERIALPORT_H_

// Waiting after connection so that the handle
// has time to stabilize in a given time
#define WAIT_TIME 4000 

// Number of bytes received (Can be changed)
#define INPUT_DATA_BYTES 10

// ************ Connected Port Settings **********
#define CBR_BAUD CBR_9600
#define BYTESIZE 8
#define STOPBITS ONESTOPBIT
#define PARITY NOPARITY

// **** The settings are set by default, you don't have to change them. ****

// Define OUTPUT_WCEER so that error messages are displayed in the console.
//#define OUTPUT_WCEER

// fields of the DCB structure, which sets the exchange control mode for the DTR signal. 
// Optional parameter, but if you need to change it, then uncomment the definition and 
// set your own parameter (Read the documentation)

//#define SETFDTRCONTROL
#define FDTRCONTROL DTR_CONTROL_ENABLE

// Error codes when setting the settings of the connected descriptor
#define PARAMS_COULDNT_SET 3L
#define PARAMS_COULDNT_FOUND 4L
#define COULD_NOT_WRITE 6L
#define COULD_NOT_READ 7L

#define OUTPUT_ERR

#define _CRT_SECURE_NO_WARNINGS

#ifdef OUTPUT_ERR
#include <iostream>
#endif

#include <windows.h>
#include <vector>
#include <string>

namespace usb {

	std::vector<std::string> findAvailableComPorts();

	class SerialPort
	{
	private:

		// Device Descriptor
		HANDLE serial;
		// true - connection is successful
		// false - connection is not successful
		BOOL isConnected;
		// Contains a lot of data about the status of the communication channel
		// It is useful for setting parameters.
		COMSTAT comStatus;
		// Error number, in case of an error
		DWORD err;
		// The name of port, need for return when calling method getPortName();
		const char* portName;

		void connectToComPort(const char* portName);

	public:

		SerialPort(const char* portName);
		SerialPort();
		~SerialPort();
		void connect(const char* portName);

		char* readPort() const;
		BOOL writePort(const char* data) const;
		BOOL isConneted() const;
		void disconnect();
		const char* getPortName() const;
		BOOL available() const;
	};

}

#endif // !_SERIALPORT_H