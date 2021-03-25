#define SDL_MAIN_HANDLED
#include <iostream>
#include "SDL-helper.h"

using namespace std;

SDL_helper sdlHelper;

int x = 100;
int y = 100;

void SDL_helper::update() {
    sdlHelper.updatePriorities();

    if (sdlHelper.keyHeld(SDLK_RIGHT)) {
        x += 5;
    }
    if (sdlHelper.keyHeld(SDLK_LEFT)) {
        x -= 5;
    }
    if (sdlHelper.keyHeld(SDLK_DOWN)) {
        y += 5;
    }
    if (sdlHelper.keyHeld(SDLK_UP)) {
        y -= 5;
    }

    sdlHelper.drawLine(x, y, 20, 40, 3, 0, 0, 0, 255);
    sdlHelper.drawImage(x, y, 200, 200, "image.bmp");
    sdlHelper.drawRect(100, 100, 100, 100, 10, 0, 0, 0, 255);

    sdlHelper.render();
    sdlHelper.correctFramerate();
}

int main() {
    sdlHelper.start();
    sdlHelper.updateLoop();

	return 0;
}