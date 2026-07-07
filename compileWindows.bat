@echo off
cd /d "%~dp0"

set SFML_INC=C:\msys64\mingw64\include
set SFML_LIB=C:\msys64\mingw64\lib
set SFML_BIN=C:\msys64\mingw64\bin

g++ -std=c++17 -Wall ^
    src\Code\Graph\Graph.cpp ^
    src\Code\MazeGenerator\MazeGenerator.cpp ^
    src\Code\Path\Path.cpp ^
    src\Code\Pacman\Pacman.cpp ^
    src\Code\Ghost\Ghost.cpp ^
    src\Code\Game\Game.cpp ^
    src\GUI\GamePlay\GamePlay.cpp ^
    src\GUI\window.cpp ^
    src\GUI\SerialReader.cpp ^
    src\GUI\asm\int_to_string.o ^
    -I src ^
    -I "%SFML_INC%" ^
    -L "%SFML_LIB%" ^
    -o src\GUI\pacman ^
    -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

if %errorlevel% neq 0 (
    pause
    exit /b %errorlevel%
)

copy /Y "%SFML_BIN%\*.dll" src\GUI\ >nul

cd src\GUI
pacman.exe
