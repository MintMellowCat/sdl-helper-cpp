#define PI 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117
#define SDL_MAIN_HANDLED
#include <iostream>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <math.h>

using namespace std;

class SDL_helper
{
public:
    SDL_Window* window;
    SDL_Renderer* renderer;
    Uint32 fps = 60; //Set speed in fps (frames per second)
    Uint32 minframetime = 1000 / fps;
    Uint32 frametime;
    bool running = true;

    void start() {
        window = SDL_CreateWindow("Game Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, SDL_WINDOW_SHOWN); //Display window
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); //Create renderer

        SDL_Init(SDL_INIT_EVERYTHING);
        SDL_SetWindowFullscreen(window, 1);
        SDL_SetWindowResizable(window, SDL_TRUE);
    }

    void updateLoop() {
        while (running) {
            update();
        }
    }

    void updatePriorities() {

        frametime = SDL_GetTicks(); //Get frame time

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); //Set background color

        SDL_RenderClear(renderer); //Clear screen

        static bool fullScreen = true; //Is fullscreen
        static int windowWidth = 1000;  //Get window width
        static int windowHeight = 700; //Get window height
        static int screenWidth = 1920;
        static int screenHeight = 1080;

        SDL_Event event;

        if (SDL_PollEvent(&event))
        {
            switch (event.type)
            {

            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_F11) { //Detect if F11 is pressed
                    if (fullScreen) {
                        SDL_SetWindowFullscreen(window, 0);
                        SDL_RestoreWindow(window);
                        SDL_SetWindowSize(window, windowWidth, windowHeight);
                        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED); //Toggle fullscreen off

                        fullScreen = false;
                    }
                    else {
                        SDL_SetWindowSize(window, 1920, 1080);
                        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN); //Toggle fullscreen on

                        fullScreen = true;

                        SDL_GetWindowSize(window, &screenWidth, &screenHeight);
                    }
                }
            }

            if (event.type == SDL_WINDOWEVENT) {
                switch (event.window.event) {
                case SDL_WINDOWEVENT_RESIZED:
                    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
                }
            }

            if (SDL_QUIT == event.type) //Detect if user presses the x button
            {
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);

                running = false;
            }
        }
    }

    bool keyDown(SDL_Keycode key) {
        SDL_Event event;

        if (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == key) {
                    return true;
                } else {
                    return false;
                }
            }
        }

        return 0;
    }

    bool keyUp(SDL_Keycode key) {
        SDL_Event event;

        if (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_KEYUP:
                if (event.key.keysym.sym == key) {
                    return true;
                } else {
                    return false;
                }
            }
        }

        return 0;
    }

    bool keyHeld(SDL_Keycode key) {
        const Uint8* state = SDL_GetKeyboardState(NULL); //Store key state in variable

        if (state[SDL_GetScancodeFromKey(key)]) {
            return true;
        } else {
            return false;
        }

        return 0;
    }

    void drawRect(int x, int y, int w, int h, int t, int r, int g, int b, int a) {
        drawLine(x, y, x + w, y, t, r, g, b, a);
        drawLine(x + w, y, x + w, y + w, t, r, g, b, a);
        drawLine(x + w, y + w, x, y + w, t, r, g, b, a);
        drawLine(x, y + w, x, y, t, r, g, b, a);
    }

    void fillRect(int x, int y, int w, int h, int r, int g, int b, int a) {
        SDL_Rect rect;
        rect.x = x;
        rect.y = y;
        rect.w = w;
        rect.h = h;

        SDL_SetRenderDrawColor(renderer, r, g, b, a);
        SDL_RenderFillRect(renderer, &rect);
    }

    void drawPoint(int x, int y, int t, int r, int g, int b, int a) {
        fillRect(x - t / 2, y - t / 2, t, t, r, g, b, a);
    }

    void drawLine(int x1, int y1, int x2, int y2, int t, int r, int g, int b, int a) {
        int dx, dy;
        float quality;

        dx = x2 - x1;
        dy = y2 - y1;
        quality = (abs(dx) + abs(dy)) * t;

        for (int i = 0; i < quality + 1; i++) {
            drawPoint(((dx / quality) * i) + x1, ((dy / quality) * i) + y1, t, r, g, b, a);
        }
    }

    void drawImage(int x, int y, int w, int h, const char* img) {
        SDL_Texture* texture = NULL;
        SDL_Surface* surface = NULL;
        surface = SDL_LoadBMP(img);
        texture = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_Rect rect;
        rect.x = x;
        rect.y = y;
        rect.w = w;
        rect.h = h;

        SDL_RenderCopy(renderer, texture, NULL, &rect);
    }

    void render() {
        SDL_RenderPresent(renderer);
    }

    void correctFramerate() {
        if (SDL_GetTicks() - frametime < minframetime) { //Check if it is running at the right speed
            SDL_Delay(minframetime - (SDL_GetTicks() - frametime));
        }
    }

    void update();
    
};