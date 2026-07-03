#pragma once
#include <windows.h>
#include <atomic>
#include <thread>
#include <string>

class SerialReader {
public:
    explicit SerialReader(const std::string& port); //COM3
    ~SerialReader();
                        //        'U'     'D'      'L'       'R'     'N'
    char getDirection() const; //ARRIBA, ABAJO, IZQUIERDA, DERECHA, NEUTRO
    bool isConnected() const;

    bool joystickButtonPress();

private:
    void readLoop();

    HANDLE handle_ = INVALID_HANDLE_VALUE;
    std::atomic<char> dir_ {'N'};
    std::atomic<bool> btnEvent_ {false};
    std::atomic<bool> running_ {false};
    std::thread thread_;
};