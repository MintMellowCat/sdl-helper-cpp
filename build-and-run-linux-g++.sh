echo 'Building...'
g++ -std=c++17 sdl-example.cpp -I"include" -L"lib" -Wall -lSDL2main -lSDL2 -o build
echo 'Done!'
echo 'Running...'
./build
