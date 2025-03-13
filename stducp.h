#pragma once

#ifndef STDUCP_H
#define STDUCP_H

#include <windows.h>

// Define for output in stderr errors
// Comment if you don`t need get an error in stderr
#define UCPERR


#define BUFFER_SIZE 256

#define BAUDRATE CBR_9600
#define STOPBITS ONESTOPBIT
#define PARITY NOPARITY
#define BYTESIZE 8

#define READINTERVALTIMEOUT 50
#define READTOTALTIMEOUTCONSTANT 50
#define READTOTALTIMEOUTMULTIPLIER 10
#define WRITETOTALTIMEOUTCONSTANT 50
#define WRITETOTALTIMEOUTMULTIPLIER 50

HANDLE hCom;
static _Bool isConnected = FALSE;

#ifdef UCPERR
#include <stdio.h>
#endif

#ifdef __cplusplus
#define EXTERN extern "C"
#else 
#define EXTERN
#endif

EXTERN
HANDLE open_com_port(const char* portName)
{
	// CreateFile: This Windows API function opens the COM port. 
	// If it fails, it prints an error message.
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
#if defined(UCPERR)
		fprintf(stderr, "Error opening COM port: %d\n", GetLastError());
#endif
		return INVALID_HANDLE_VALUE;
	}
	// DCB Structure: A DCB (Device Control Block) structure is used to define the parameters 
	// of the COM port. The baud rate, byte size, stop bits, and parity are set.
	// Setup the COM port parameters
	DCB dcb;
	if (!GetCommState(hCom, &dcb))
	{
#if defined(UCPERR)
		fprintf(stderr, "Error getting COM state: %d\n", GetLastError());
#endif
		CloseHandle(hCom);
		return INVALID_HANDLE_VALUE;
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
		fprintf(stderr, "Error setting COM state: %d\n", GetLastError());
#endif
		CloseHandle(hCom);
		return INVALID_HANDLE_VALUE;
	}
	// COMMTIMEOUTS: This structure sets timeouts for read and write operations.
	// Optionally, set timeouts
	COMMTIMEOUTS timeouts;
	timeouts.ReadIntervalTimeout = READINTERVALTIMEOUT;
	timeouts.ReadTotalTimeoutConstant = READTOTALTIMEOUTCONSTANT;
	timeouts.ReadTotalTimeoutMultiplier = READTOTALTIMEOUTMULTIPLIER;
	timeouts.WriteTotalTimeoutConstant = WRITETOTALTIMEOUTCONSTANT;
	timeouts.WriteTotalTimeoutMultiplier = WRITETOTALTIMEOUTMULTIPLIER;

	if (!SetCommTimeouts(hCom, &timeouts))
	{
#if defined(UCPERR)
		fprintf(stderr, "Error setting timeouts: %d\n", GetLastError());
#endif
		// CloseHandle: Once done with the COM port, it’s important to close the handle to release the resource.
		CloseHandle(hCom);
		return INVALID_HANDLE_VALUE;
	}
	isConnected = TRUE;
	// Return the handle to the opened COM port
	return hCom;
}

EXTERN
void close_com_port(void)
{
	if (isConnected)
		CloseHandle(hCom);
#ifdef UCPERR
	else fprintf(stderr, "The port is closed\n");
#endif
}

EXTERN
const char* read_from_com_port(void)
{
	if (!isConnected)
	{
#ifdef UCPERR
		fprintf(stderr, "The port is closed\n");
#endif
		return NULL;
	}
	char* buffer = calloc(BUFFER_SIZE, sizeof(char));
	int bytesRead = 0;
	if (!ReadFile(hCom, buffer, BUFFER_SIZE - 1, &bytesRead, NULL))
	{
#if defined(UCPERR)
		fprintf(stderr, "Error reading from COM port: %ld\n", GetLastError());
#endif
		return NULL;
	}
	return buffer;
}

EXTERN
long is_available(void)
{
	if (!isConnected)
	{
#ifdef UCPERR
		fprintf(stderr, "The port is closed\n");
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
		fprintf(stderr, "Error checking the port status: %ld\n", GetLastError());
#endif
		return -1;
	}
	return res;
}

EXTERN
_Bool write_to_com_port(const char* data)
{
	if (!isConnected)
	{
#ifdef UCPERR
		fprintf(stderr, "The port is closed\n");
#endif
		return FALSE;
	}
	DWORD bytesWritten;
	if (!WriteFile(hCom, data, strlen(data), &bytesWritten, NULL))
	{
#if defined(UCPERR)
		fprintf(stderr, "Error writing to COM port: %ld\n", GetLastError());
#endif
		return FALSE;
	}
	return TRUE;
}


#endif // !STDUCP_H