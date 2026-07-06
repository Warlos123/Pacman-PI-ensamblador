El proyecto integra el clasico juego pacman en C++ utilizando SFML, con integracion de Arduino y ensamblador. Se integra un joystick con arduino para controlar el movimiento. La integracion con ensamblador optimiza la conversion de los datos de vidas, score y jump walls para mostrarlos en pantalla. Se incorpora mapas generados aleatoriamente por medio de grafos, diferentes power-ups como jump walls y portales. Ia para los fantasmas utilizando BFS para encontrar el camino mas corto hacia el jugador. 

===========================
PACMAN
===========================

##REQUIREMENTS##

•Need to install "MSYS2" in order to run the program.
•Once MSYS2 is installed, in your programs run "MSYS2 UCRT64".
•Run the following command:
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-sfml

•This will install g++ compiler and SFML.
•Once the installation is complete you can close the window.



##COMPILE AND RUN##
•To compile and run the program, execute "run.bat".

•If this fails, use the following commands:
-To compile run the following commands inside CMD, keep in mind you need to cd "project_path".
C:\msys64\ucrt64\bin\g++.exe -g src/main.cpp -o pacman -IC:\msys64\ucrt64\include -LC:\msys64\ucrt64\lib -lsfml-graphics -lsfml-window -lsfml-system

cd src/GUI
pacman.exe


•Keep in mind your paths may vary, change accordingly.