#include <iostream>
#include "SDL-helper.h"

using namespace std;

SDL_helper sdlHelper;

SDL_RendererFlip flip = SDL_FLIP_NONE;
SDL_Texture* et;
SDL_Texture* pt;
float x = 100;
float y = 100;
float ex = 500;
float ey = 100;
int mx;
int my;
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
        flip = SDL_FLIP_NONE;
    } else if (sdlHelper.keyHeld(SDLK_LEFT)) {
        xs = -10;
        flip = SDL_FLIP_HORIZONTAL;
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

    sdlHelper.mousePotition(&mx, &my);

    if (y > 1080 - 160) {
        gs = 0;
        y = 1080 - 160;
    }
    if (x < 0) {
        x = 0;
    }
    if (x > 1920 - 200) {
        x = 1920 - 200;
    }
    if (ey > 1080 - 95) {
        egs = 0;
        ey = 1080 - 95;
    }

    sdlHelper.renderImageEx(x, y, 200, 200, flip, 0, 0, 0, pt);
    sdlHelper.renderImage(ex, ey, 100, 100, et);
    sdlHelper.drawText(100, 100, 100, "Roboto/Roboto-Regular.ttf", "This is an example game for SDL-HELPER!!!", 255, 100, 100);
    sdlHelper.fillRect(mx - 10, my - 10, 20, 20, 0, 0, 0, 255);

    sdlHelper.render();
    sdlHelper.correctFramerate();
}

int main() {
    sdlHelper.showCursor = false;
    sdlHelper.start();
    et = sdlHelper.loadImage("image.png");
    pt = sdlHelper.loadImage("image.bmp");
    sdlHelper.setIcon(sdlHelper.loadImageToSurface("image.png"));
    sdlHelper.updateLoop();

	return 0;
}