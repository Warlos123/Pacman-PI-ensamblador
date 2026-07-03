@echo off
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
    -o src\GUI\pacman ^
    -lsfml-graphics -lsfml-window -lsfml-system

if %errorlevel% neq 0 (
    pause
    exit /b %errorlevel%
)

cd src\GUI
pacman.exe