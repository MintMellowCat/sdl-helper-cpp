#define SDL_MAIN_HANDLED
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <stb/stb_image.h>
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
    SDL_Event event;
    int poll;
    bool running = true;
    bool showCursor = true;

    void start() {
        window = SDL_CreateWindow("Game Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, SDL_WINDOW_SHOWN); //Display window
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); //Create renderer

        SDL_Init(SDL_INIT_EVERYTHING);
        SDL_SetWindowFullscreen(window, 1);
        SDL_SetWindowResizable(window, SDL_TRUE);
        SDL_ShowCursor(showCursor);
    }

    void updateLoop() {
        while (running) {
            update();
        }
    }

    void updatePriorities() {
        poll = SDL_PollEvent(&event);
        frametime = SDL_GetTicks(); //Get frame time
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); //Set background color
        SDL_RenderClear(renderer); //Clear screen

        static bool fullScreen = true; //Is fullscreen
        static int windowWidth = 1000;  //Get window width
        static int windowHeight = 700; //Get window height
        static int screenWidth = 1920;
        static int screenHeight = 1080;

        if (poll)
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
        if (poll)
        {
            if (event.type == SDL_KEYDOWN) {
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
        if (poll)
        {
            if (event.type == SDL_KEYUP) {
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

    bool mouseDown(int button) {
        if (poll)
        {
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == button) {
                    return true;
                } else {
                    return false;
                }
            }
        }

        return 0;
    }

    bool mouseUp(int button) {
        if (poll)
        {
            if (event.type == SDL_MOUSEBUTTONUP) {
                if (event.button.button == button) {
                    return true;
                } else {
                    return false;
                }
            }
        }

        return 0;
    }

    bool mouseHeld(int button) {
        SDL_PumpEvents();
        return SDL_GetMouseState(NULL, NULL)&button;
    }

    int mousePositionX() {
        int x;
        SDL_PumpEvents();
        SDL_GetMouseState(&x, NULL);

        return x;
    }

    int mousePositionY() {
        int y;
        SDL_PumpEvents();
        SDL_GetMouseState(NULL, &y);

        return y;
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

    void drawPixel(int x, int y, int r, int g, int b, int a) {
        SDL_SetRenderDrawColor(renderer, r, g, b, a);
        SDL_RenderDrawPoint(renderer, x, y);
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

    void drawPolygon(int length, int x[], int y[], int t, int r, int g, int b, int a) {
        for (int i = 0; i < length; i++) {
            drawLine(x[i], y[i], x[(i + 1)%length], y[(i + 1)%length], t, r, g, b, a);
        }
    }

    void drawBMPImage(int x, int y, int w, int h, const char* img, bool t, int r, int g, int b) {
        SDL_Texture* texture = NULL;
        SDL_Surface* surface = NULL;
        surface = SDL_LoadBMP(img);

        if (t) {
            Uint32 colorKey = SDL_MapRGB(surface->format, r, g, b);
            SDL_SetColorKey(surface, SDL_TRUE, colorKey);
        }

        texture = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_Rect rect;
        rect.x = x;
        rect.y = y;
        rect.w = w;
        rect.h = h;

        SDL_RenderCopy(renderer, texture, NULL, &rect);
    }

    void drawImage(int x, int y, int w, int h, const char* img) {
        SDL_Texture* texture = NULL;
        SDL_Surface* surface = NULL;
        int img_w, img_h, comp;
        unsigned char* data = stbi_load(img, &img_w, &img_h, &comp, STBI_rgb_alpha);

        surface = SDL_CreateRGBSurfaceFrom(data, img_w, img_h, 32, 4 * img_w, 0xFF, 0xFF00, 0xFF0000, 0xFF000000);
        texture = SDL_CreateTextureFromSurface(renderer, surface);

        stbi_image_free(data);
        SDL_FreeSurface(surface);

        SDL_Rect rect;
        rect.x = x;
        rect.y = y;
        rect.w = w;
        rect.h = h;

        SDL_RenderCopy(renderer, texture, NULL, &rect);
        SDL_DestroyTexture(texture);

    }

    void loadImage(SDL_Texture *texture, const char* img)
    {
        SDL_Surface* surface = NULL;
        int img_w, img_h, comp;
        unsigned char* data = stbi_load(img, &img_w, &img_h, &comp, STBI_rgb_alpha);

        surface = SDL_CreateRGBSurfaceFrom(data, img_w, img_h, 32, 4 * img_w, 0xFF, 0xFF00, 0xFF0000, 0xFF000000);
        texture = SDL_CreateTextureFromSurface(renderer, surface);

        stbi_image_free(data);
        SDL_FreeSurface(surface);
    }

    void renderImage(int x, int y, int w, int h, SDL_Texture *texture) {
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