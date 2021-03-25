echo 'Building...'
g++ -std=c++17 main.cpp -I"include" -L"lib" -Wall -lSDL2main -lSDL2 -o sdl-example
echo 'Done!'
echo 'Running...'
./sdl-example
