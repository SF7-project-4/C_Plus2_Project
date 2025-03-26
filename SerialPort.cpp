#include "pch.h"
#include "SerialPort.h"

SerialPort::SerialPort(const char* portName, int baudRate)
{
    hSerial = CreateFileA(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    isConnected = (hSerial != INVALID_HANDLE_VALUE);
}

SerialPort::~SerialPort()
{
    Close();
}

bool SerialPort::Open()
{
    return isConnected;
}

void SerialPort::Close()
{
    if (isConnected)
    {
        CloseHandle(hSerial);
        isConnected = false;
    }
}

bool SerialPort::Write(const char* data, int length)
{
    if (!isConnected) return false;
    DWORD bytesWritten;
    return WriteFile(hSerial, data, length, &bytesWritten, NULL);
}
