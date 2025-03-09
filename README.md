# USB COM PORT OPERATION

Explanation:
OpenComPort Function: This function takes a string representing the COM port name (e.g. "COM3") and attempts to open it.
CreateFile: This Windows API function opens the COM port. If it fails, it prints an error message.
DCB Structure: A DCB (Device Control Block) structure is used to define the parameters of the COM port. The baud rate, byte size, stop bits, and parity are set.
SetCommState: This function applies the settings to the COM port.
COMMTIMEOUTS: This structure sets timeouts for read and write operations.
CloseHandle: Once done with the COM port, it’s important to close the handle to release the resource.
Make sure to modify the port and parameters according to your specific needs.
