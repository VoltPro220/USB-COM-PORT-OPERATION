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
        // Ошибка открытия порта
        printf("Ошибка открытия порта %s: %d\n", port_name, GetLastError());
        return INVALID_HANDLE_VALUE;
    }

    // Настройка параметров порта
    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams))
    {
        // Ошибка получения состояния
        printf("Ошибка получения состояния порта: %d\n", GetLastError());
        CloseHandle(hSerial);
        return INVALID_HANDLE_VALUE;
    }

    // Установка параметров
    dcbSerialParams.BaudRate = CBR_9600; // Здесь вы можете установить необходимые параметры
    dcbSerialParams.ByteSize = 8;         // 8 бит данных
    dcbSerialParams.StopBits = ONESTOPBIT; // 1 стоп-бит
    dcbSerialParams.Parity = NOPARITY;  // Без четности

    if (!SetCommState(hSerial, &dcbSerialParams))
    {
        // Ошибка установки состояния
        printf("Ошибка установки состояния порта: %d\n", GetLastError());
        CloseHandle(hSerial);
        return INVALID_HANDLE_VALUE;
    }

    return hSerial;
}

int main()
{
    const char* port_name = "COM3"; // Укажите свой COM порт здесь
    HANDLE hSerial = open_com_port(port_name);

    if (hSerial != INVALID_HANDLE_VALUE)
    {
        printf("COM порт %s успешно открыт.\n", port_name);
        // Здесь можно взаимодействовать с открытым портом

        // Закрытие порта
        CloseHandle(hSerial);
    }

    return 0;
}
