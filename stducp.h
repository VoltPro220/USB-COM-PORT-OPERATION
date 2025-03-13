#pragma once

#ifndef STDUCP_H
#define STDUCP_H

#include <windows.h>

// Define for output in stderr errors
// Comment if you don`t need get an error in stderr
#define UCPERR
#define BUFFER_SIZE 256

HANDLE hCom;

#ifdef UCPERR
#include <stdio.h>
#endif


#ifdef __cplusplus
extern "C"
#else 
extern
#endif
HANDLE open_com_port(const char* portName)
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
#if defined(UCPERR)
		fprintf(stderr, "Error opening COM port: %d\n", GetLastError());
#endif
		return INVALID_HANDLE_VALUE;
	}

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
	dcb.BaudRate = CBR_9600; // Baud rate
	dcb.StopBits = ONESTOPBIT; // One stop bit
	dcb.Parity = NOPARITY; // No parity
	dcb.ByteSize = 8; // 8 data bits

	// Set the COM port settings
	if (!SetCommState(hCom, &dcb))
	{
#if defined(UCPERR)
		fprintf(stderr, "Error setting COM state: %d\n", GetLastError());
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
#if defined(UCPERR)
		fprintf(stderr, "Error setting timeouts: %d\n", GetLastError());
#endif
		CloseHandle(hCom);
		return INVALID_HANDLE_VALUE;
	}

	// Return the handle to the opened COM port
	return hCom;
}

#ifdef __cplusplus
extern "C"
#else 
extern
#endif
void close_com_port()
{
	CloseHandle(hCom);
}

#ifdef __cplusplus
extern "C"
#else 
extern
#endif
const char* read_from_com_port()
{
	char* buffer = malloc(BUFFER_SIZE);
	int bytesRead = 0;
	if (!ReadFile(hCom, buffer, BUFFER_SIZE - 1, &bytesRead, NULL))
	{
#if defined(UCPERR)
		fprintf(stderr, "Error reading from COM port: %ld\n", GetLastError());
#endif
		return NULL;
	}
	buffer[bytesRead] = '\0'; // Null-terminate the string
	return buffer;
}

#ifdef __cplusplus
extern "C"
#else 
extern
#endif
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
#if defined(UCPERR)
		fprintf(stderr, "Error checking the port status: %ld\n", GetLastError());
#endif
	}
	return res;
}

#ifdef __cplusplus
extern "C"
#else 
extern
#endif
_Bool write_to_com_port(const char* data)
{
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

