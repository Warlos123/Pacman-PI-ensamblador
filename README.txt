El proyecto integra el clasico juego pacman en C++ utilizando SFML, con integracion de Arduino y ensamblador. Se integra un joystick con arduino para controlar el movimiento. La integracion con ensamblador optimiza la conversion de los datos de vidas, score y jump walls para mostrarlos en pantalla. Se incorpora mapas generados aleatoriamente por medio de grafos, diferentes power-ups como jump walls y portales. Ia para los fantasmas utilizando BFS para encontrar el camino mas corto hacia el jugador. 

===========================
PACMAN
===========================

##REQUIREMENTS##
•Need to install "MSYS2" in order to run the program.
•Once MSYS2 is installed, in your programs run "MSYS2 MinGW64" (not UCRT64).
•Run the following command:
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-sfml
•This will install g++ compiler and SFML.
•Add "C:\msys64\mingw64\bin" to your Windows PATH (Environment Variables).
•If you're having issues with compiling make sure this path is first in the list of any other msys64 paths.
•Once done, close and reopen any CMD windows so the PATH updates.

##COMPILE AND RUN##
•To compile and run the program, execute "compileWindows.bat".
•If this fails, use the following commands, keep in mind you need to cd into the project root first.

g++ -std=c++17 -Wall src\Code\Graph\Graph.cpp src\Code\MazeGenerator\MazeGenerator.cpp src\Code\Path\Path.cpp src\Code\Pacman\Pacman.cpp src\Code\Ghost\Ghost.cpp src\Code\Game\Game.cpp src\GUI\GamePlay\GamePlay.cpp src\GUI\window.cpp src\GUI\SerialReader.cpp src\GUI\asm\int_to_string.o -I src -I C:\msys64\mingw64\include -L C:\msys64\mingw64\lib -o src\GUI\pacman -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

copy C:\msys64\mingw64\bin\*.dll src\GUI\

cd src\GUI
pacman.exe

•Keep in mind your paths may vary, change accordingly.

