#pragma once
#include <windows.h>

class SerialPort
{
private:
    HANDLE hSerial;
    bool isConnected;

public:
    SerialPort(const char* portName, int baudRate);
    ~SerialPort();
    bool Open();
    void Close();
    bool Write(const char* data, int length);
};
