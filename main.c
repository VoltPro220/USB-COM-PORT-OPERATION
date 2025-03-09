#include <stdio.h>
#include <windows.h>

HANDLE OpenComPort(const char* portName)
{
    // Open the specified COM port
    HANDLE hCom = CreateFileA(portName,
                             GENERIC_READ | GENERIC_WRITE,
                             0, // No sharing
                             NULL, // Default security
                             OPEN_EXISTING,
                             0, // No attributes
                             NULL); // No template file

    // Check if the COM port was opened successfully
    if (hCom == INVALID_HANDLE_VALUE)
    {
        printf("Error opening COM port: %d\n", GetLastError());
        return INVALID_HANDLE_VALUE;
    }

    // Setup the COM port parameters
    DCB dcb;
    if (!GetCommState(hCom, &dcb))
    {
        printf("Error getting COM state: %d\n", GetLastError());
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
        printf("Error setting COM state: %d\n", GetLastError());
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
        printf("Error setting timeouts: %d\n", GetLastError());
        CloseHandle(hCom);
        return INVALID_HANDLE_VALUE;
    }

    // Return the handle to the opened COM port
    return hCom;
}

int main()
{
    const char* portName = "COM3"; // Change this to your appropriate COM port

    HANDLE hCom = OpenComPort(portName);
    if (hCom != INVALID_HANDLE_VALUE)
    {
        printf("COM port opened successfully\n");

        // Perform reading/writing operations here...

        // Close the COM port when done
        CloseHandle(hCom);
    }

    return 0;
}
