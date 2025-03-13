# USB COM PORT OPERATION Utility

Make sure to modify the port and parameters according to your specific needs. It will be written below how to do this.

*If you want a detailed explanation of how the code works, then there will be a link to README_DETAILED.md with detailed explanation and examples.*
*So far, basic information on working with the utility and configuring it is listed here.*

Function and Explanation:
- ```HANDLE open_com_port(const char*)```: This function takes a string representing the COM port name (e.g. "COM3") and attempts to open it. Specify the COM port name as an argument (COMN, where N is the com port number).
  It will return one of the values different from the descriptor itself if it was not possible to access the distribution. Read the description of each error below.
- ```void close_com_port(void)```: Closes the handle if you opened the com port. Necessarily **call him** after open com port.
- ```const char* read_from_com_port(void)```: Reading various data from the COM port that the descriptor sends. Open it first, or the function will just return NULL.
- ```long is_available(void)```: Checks how many bytes are available in the COM port of the buffer and return. If it returns -1, there was an error.
- ```_Bool write_to_com_port(const char* data)```: Sends data to the COM port and returns true if successful and vice versa.

### Defines and Conditional compilation constructs
Define for output in stderr errors. It will output all errors when working with the buffer, for example, you have not opened the buffer and are trying to close it, or the handle itself is not working properly.
Comment or remove the line with this define if you don`t need get an error in stderr
```
#define UCPERR
```

You can change the value, since so many bytes will be allocated when reading data from the buffer.
This define - Necessarily
```
#define BUFFER_SIZE 256
```

The utility can also be used in C++ code. Below will be examples.
```
#ifdef __cplusplus
#define EXTERN extern "C"
#else 
#define EXTERN
#endif
```

The standart setting com port. If you need you can change them
```
#define BAUDRATE CBR_9600
#define STOPBITS ONESTOPBIT
#define PARITY NOPARITY
#define BYTESIZE 8

#define READINTERVALTIMEOUT 50
#define READTOTALTIMEOUTCONSTANT 50
#define READTOTALTIMEOUTMULTIPLIER 10
#define WRITETOTALTIMEOUTCONSTANT 50
#define WRITETOTALTIMEOUTMULTIPLIER 50
```

### Errors:
- ```Error opening COM port```  COM port connection error, check if you entered the port number correctly. You can check through the device manager

![device manager](img/ports.png)
- ```Error getting COM state```  We tried to get the status of the COM port to configure it to work, but an error came out. Check the integrity of the descriptor and try again.
- ```Error setting COM state```  We managed to get the port data, but we couldn't configure it.
- ```Error setting timeouts```  It was not possible to set timeouts for read and write operations.
- ```The port is closed```  You tried to work with the COM port, but you didn't open it. Open via the desired function
- ```Error reading from COM port``` Data from the COM port was unsuccessfully read
- ```Error checking the port status``` Couldn't get the port status
- ```Error writing to COM port``` Error when sending data to the COM port
  
### Examples:
1. Put the stducp.h in one directory with main file
```
// Includes
#include <stdio.h>
#include <conio.h>
#include "stducp.h"

int main(void)
{
	const char* portName = "COM3"; // Change this to your appropriate COM port
	HANDLE h = open_com_port(portName); // Open com port
	char* msg = ""; // In this pointer we will save  data from buffer
	while (!_kbhit()) // If you press the key then the cycle ends 
	{
		if (is_available()) // Check buffer
		{
			msg = read_from_com_port(); // Attention, the function uses malloc(), and do not forget about freeing up memory.
			printf("%s", msg); // Output
			free(msg); // Freeing up memory
		}
	}
	close_com_port(); // close


	return 0;
}

```
2. Sending data to a COM port
```
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
```
