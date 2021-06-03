#include <iostream>
#include "SDL-helper.h"

SDL_helper sdlHelper;

SDL_RendererFlip flip = SDL_FLIP_NONE;
SDL_Texture* et;
SDL_Texture* stickman[4];
int mx;
int my;
int n = 0;
int t = 0;
int f = 0;
int x = 100;
int y = 100;
bool tc = false;
bool m = false;
float ex = 500;
float ey = 100;
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

    m = false;

    if (sdlHelper.keyHeld(SDLK_RIGHT)) {
        xs = 10;
        flip = SDL_FLIP_NONE;
        m = true;
    }
    if (sdlHelper.keyHeld(SDLK_LEFT)) {
        xs = -10;
        flip = SDL_FLIP_HORIZONTAL;
        m = true;
    }
    if (gs == 0 && y >= 1080 - 180) {
        if (sdlHelper.keyDown(SDLK_SPACE)) {
            gs = -20;
            sdlHelper.playAudio("assets/jump.wav", 100);
        }
    }
    if (sdlHelper.mouseDown(SDL_BUTTON_LEFT)) {
        sdlHelper.playAudio("assets/click.wav", 100);
    }
    if (sdlHelper.mouseHeld(SDL_BUTTON_LEFT)) {
        if (isColliding(mx - 10, my - 10, 20, 20, 100, 110, 1640, 100)) {
            tc = true;
        }
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

    if (y > 1080 - 180) {
        gs = 0;
        y = 1080 - 179;
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

    std::cout << elapsedTime << "\n";

    if (m && gs == 0 && y >= 1080 - 180) {
        if (sdlHelper.elapsedFrames - t >= 5) {
            t = sdlHelper.elapsedFrames;
            f += 1;
            if (f > 3) {
                f = 0;
            }
        }

        if (f == 0) {
            sdlHelper.renderImageEx(x, y, 200, 200, flip, 0, 0, 0, stickman[1]);
        } else if (f == 1) {
            sdlHelper.renderImageEx(x, y, 200, 200, flip, 0, 0, 0, stickman[2]);
        } else if (f == 2) {
            sdlHelper.renderImageEx(x, y, 200, 200, flip, 0, 0, 0, stickman[3]);
        } else if (f == 3) {
            sdlHelper.renderImageEx(x, y, 200, 200, flip, 0, 0, 0, stickman[2]);
        }
    } else {
        t = elapsedFrames;
        sdlHelper.renderImageEx(x, y, 200, 200, flip, 0, 0, 0, stickman[0]);
    }

    sdlHelper.renderImage(ex, ey, 100, 100, et);
    sdlHelper.drawText(95, 95, 100, "assets/Roboto/Roboto-Regular.ttf", "This is an example game for SDL-HELPER!!!", 0, 0, 0, 30);
    sdlHelper.drawText(100, 100, 100, "assets/Roboto/Roboto-Regular.ttf", "This is an example game for SDL-HELPER!!!", tc ? 0 : 255, 100, tc ? 255 : 100, 255);
    sdlHelper.fillRect(mx - 10, my - 10, 20, 20, 0, 0, 0, 255);
    sdlHelper.render();
}

void SDL_helper::audioUpdate() {
    if (n == 0) {
        sdlHelper.playAudio("assets/music.wav", 3700);
        n = 228;
    }
    n--;
    sdlHelper.correctFramerate();
}

int main() {
    sdlHelper.showCursor = false;
    sdlHelper.start();
    sdlHelper.icon = sdlHelper.loadImageToSurface("assets/image.png");
    et = sdlHelper.loadImage("assets/image.png");
    stickman[0] = sdlHelper.loadImage("assets/stickman/frame-1.png");
    stickman[1] = sdlHelper.loadImage("assets/stickman/frame-2.png");
    stickman[2] = sdlHelper.loadImage("assets/stickman/frame-3.png");
    stickman[3] = sdlHelper.loadImage("assets/stickman/frame-4.png");
    sdlHelper.updateIcon();
    sdlHelper.updateLoop();

	return 0;
}