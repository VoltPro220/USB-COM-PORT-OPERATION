#include <stdio.h>
#include <windows.h>

HANDLE open_com_port(const char* port_name)
{
    HANDLE hSerial = CreateFile(port_name,
                                GENERIC_READ | GENERIC_WRITE,
                                0, // No sharing
                                NULL, // No security
                                OPEN_EXISTING,
                                0, // No attributes
                                NULL); // No template

    if (hSerial == INVALID_HANDLE_VALUE)
    {
        // ������ �������� �����
        printf("������ �������� ����� %s: %d\n", port_name, GetLastError());
        return INVALID_HANDLE_VALUE;
    }

    // ��������� ���������� �����
    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams))
    {
        // ������ ��������� ���������
        printf("������ ��������� ��������� �����: %d\n", GetLastError());
        CloseHandle(hSerial);
        return INVALID_HANDLE_VALUE;
    }

    // ��������� ����������
    dcbSerialParams.BaudRate = CBR_9600; // ����� �� ������ ���������� ����������� ���������
    dcbSerialParams.ByteSize = 8;         // 8 ��� ������
    dcbSerialParams.StopBits = ONESTOPBIT; // 1 ����-���
    dcbSerialParams.Parity = NOPARITY;  // ��� ��������

    if (!SetCommState(hSerial, &dcbSerialParams))
    {
        // ������ ��������� ���������
        printf("������ ��������� ��������� �����: %d\n", GetLastError());
        CloseHandle(hSerial);
        return INVALID_HANDLE_VALUE;
    }

    return hSerial;
}

int main()
{
    const char* port_name = "COM3"; // ������� ���� COM ���� �����
    HANDLE hSerial = open_com_port(port_name);

    if (hSerial != INVALID_HANDLE_VALUE)
    {
        printf("COM ���� %s ������� ������.\n", port_name);
        // ����� ����� ����������������� � �������� ������

        // �������� �����
        CloseHandle(hSerial);
    }

    return 0;
}
