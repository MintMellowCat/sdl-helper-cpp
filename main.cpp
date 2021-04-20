#include <iostream>
#include "SDL-helper.h"

using namespace std;

SDL_helper sdlHelper;

float x = 100;
float y = 100;
float ex = 500;
float ey = 100;
float mx;
float my;
float xs = 0;
float ys = 0;
float exs = 0;
float eys = 0;
float r = 0;
float g = 0.5;
float gs = 0;
float egs = 0;

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
    }
    if (sdlHelper.mouseDown(SDL_BUTTON_LEFT)) {
        gs = 0;
        x = sdlHelper.mousePositionX() - 100;
        y = sdlHelper.mousePositionY() - 100;
    }
    if (x + 100 <= ex + 50) {
        exs = -5;
    } else if (x + 100 >= ex + 50) {
        exs = 5;
    }

    x += xs;
    gs += g;
    y += gs + ys;
    xs = 0;
    ys = 0;

    ex += exs;
    egs += g;
    ey += egs + eys;
    exs = 0;
    eys = 0;

    mx = sdlHelper.mousePositionX();
    my = sdlHelper.mousePositionY();

    if (y > 1080 - 160) {
        gs = 0;
        y = 1080 - 160;
    }
    if (ey > 1080 - 95) {
        egs = 0;
        ey = 1080 - 95;
    }

    sdlHelper.drawBMPImage(x, y, 200, 200, "image.bmp", true, 255, 255, 255);
    sdlHelper.drawImage(ex, ey, 100, 100, "image.png");
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