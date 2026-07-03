#include "SerialReader.hpp"

SerialReader::SerialReader(const std::string& port){
    std::string fullPort = "\\\\.\\" + port; 
    
    for (int intento = 0; intento < 10; ++intento){
        handle_ = CreateFileA(fullPort.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr);
        if (handle_ != INVALID_HANDLE_VALUE) break;
        if (GetLastError() != ERROR_ACCESS_DENIED) break;
        Sleep(50);
    }

    if (handle_ == INVALID_HANDLE_VALUE)
        return;

    DCB dcb = {};
    dcb.DCBlength = sizeof(dcb);
    GetCommState(handle_, &dcb);
    dcb.BaudRate = CBR_9600;
    dcb.ByteSize = 8;
    dcb.Parity   = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    SetCommState(handle_, &dcb);

    COMMTIMEOUTS timeouts = {};
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    SetCommTimeouts(handle_, &timeouts);

    running_ = true;
    thread_  = std::thread(&SerialReader::readLoop, this);
}

SerialReader::~SerialReader() {
    running_ = false;
    if(thread_.joinable())
        thread_.join();
    if(handle_ != INVALID_HANDLE_VALUE)
        CloseHandle(handle_);
}

char SerialReader::getDirection() const {
    return dir_.load();
}

bool SerialReader::isConnected() const {
    return handle_ != INVALID_HANDLE_VALUE;
}

bool SerialReader::joystickButtonPress() {
    return btnEvent_.exchange(false);
}

void SerialReader::readLoop() {
    char buf;
    DWORD read;
    bool btnDown = false;
    while (running_){
        if (ReadFile(handle_, &buf, 1, &read, nullptr) && read == 1) {
            if (buf == 'U' || buf == 'D' || buf == 'L' || buf == 'R' || buf == 'N')
                dir_.store(buf);

            else if(buf == 'B'){
                if(!btnDown){
                    btnEvent_.store(true);
                }

                btnDown = true;
            }

            else if(buf == 'b'){
                btnDown = false;
            }
        }
    }
}