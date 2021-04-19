#include <iostream>
#include "SDL-helper.h"

using namespace std;

SDL_helper sdlHelper;

float x = 100;
float y = 100;
float mx;
float my;
float xs = 0;
float ys = 0;
float r = 0;
float g = 0.5;
float gs = 0;

void SDL_helper::update() {
    sdlHelper.updatePriorities();

    if (sdlHelper.keyHeld(SDLK_RIGHT)) {
        xs = 10;
    } else if (sdlHelper.keyHeld(SDLK_LEFT)) {
        xs = -10;
    }
    if (gs == 0 && y >= 1080 - 160) {
        if (sdlHelper.keyDown(SDLK_SPACE)) {
            gs = -20;
        }
        if (sdlHelper.mouseHeld(SDL_BUTTON_LEFT)) {
            gs = -10;
        }
    }

    x += xs;
    gs += g;
    y += gs + ys;
    xs = 0;
    ys = 0;

    mx = sdlHelper.mousePositionX();
    my = sdlHelper.mousePositionY();

    if (y > 1080 - 160) {
        gs = 0;
        y = 1080 - 160;
    }

    sdlHelper.drawBMPImage(x, y, 200, 200, "image.bmp", true, 255, 255, 255);
    sdlHelper.fillRect(mx - 10, my - 10, 20, 20, 0, 0, 0, 255);

    sdlHelper.render();
    sdlHelper.correctFramerate();
}

int main() {
    sdlHelper.showCursor = false;
    sdlHelper.start();
    sdlHelper.updateLoop();

	return 0;
}