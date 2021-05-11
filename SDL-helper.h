#define STB_TRUETYPE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define SDL_MAIN_HANDLED
#include <iostream>
#include <string>
#include <thread>
#include <SDL2/SDL.h>
#include <stb/stb_image.h>
#include <stb/stb_truetype.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

static Uint8* audioPos;
static Uint32 audioLen;
std::thread audioThread;
std::thread audioUpdateThread;

void playWav(const char *audioDir, int audioTime);
void audioCallback(void* userdata, uint8_t* stream, int len);

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
    bool pKeysPressed[512];
    bool keysPressed[512];
    bool pMousesPressed[512];
    bool mousesPressed[512];
    SDL_Surface* icon;

    void start() {
        ttfBuffer = NULL;

        window = SDL_CreateWindow("Game Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, SDL_WINDOW_SHOWN); //Display window
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); //Create renderer
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        SDL_Init(SDL_INIT_EVERYTHING);
        SDL_SetWindowFullscreen(window, 1);
        SDL_SetWindowResizable(window, SDL_TRUE);
        SDL_ShowCursor(showCursor);

        SDL_AudioSpec audioSpec;
        SDL_OpenAudioDevice(NULL, 0, &audioSpec, NULL, 0);
    }

    void updateLoop() {
        while (running) {
            update();
            audioUpdateThread = std::thread(audioUpdate);
            audioUpdateThread.detach();
        }
    }

    void readEvents() {
        static bool fullScreen = true; //Is fullscreen
        static int windowWidth = 1000;  //Get window width
        static int windowHeight = 700; //Get window height
        static int screenWidth = 1920;
        static int screenHeight = 1080;

        switch(event.type)
        {
            case SDL_KEYDOWN:
            {
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
                keysPressed[event.key.keysym.scancode] = true;
            }
            break;
            case SDL_MOUSEBUTTONDOWN: mousesPressed[event.button.button] = true; break;
            case SDL_KEYUP: keysPressed[event.key.keysym.scancode] = false; break;
            case SDL_MOUSEBUTTONUP: mousesPressed[event.button.button] = false; break;
            case SDL_WINDOWEVENT:
            {
                switch (event.window.event) 
                {
                    case SDL_WINDOWEVENT_RESIZED: SDL_GetWindowSize(window, &windowWidth, &windowHeight); break;
                }
            }
            break;
            case SDL_QUIT:
            {
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);

                running = false;
            }
            break;
        }
    }

    void updatePriorities() {
        frametime = SDL_GetTicks(); //Get frame time
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); //Set background color
        SDL_RenderClear(renderer); //Clear screen

        for (int i = 0; i < 512; i++) {
            pKeysPressed[i] = keysPressed[i];
        }
        for (int i = 0; i < 512; i++) {
            pMousesPressed[i] = mousesPressed[i];
        }

        while (SDL_PollEvent(&event)) {
            readEvents();
        }
    }

    bool updateIcon() {
        if (icon != NULL) {
            SDL_SetWindowIcon(window, icon);

            return true;
        } else {
            return false;
        }
    }

    bool keyDown(SDL_Keycode key) {
        bool down = false;

        if (!pKeysPressed[SDL_GetScancodeFromKey(key)] && keysPressed[SDL_GetScancodeFromKey(key)]) {
            down = true;
        }

        return down;
    }

    bool keyUp(SDL_Keycode key) {
        bool up = false;

        if (pKeysPressed[SDL_GetScancodeFromKey(key)] && !keysPressed[SDL_GetScancodeFromKey(key)]) {
            up = true;
        }

        return up;
    }

    bool keyHeld(SDL_Keycode key) {
        const Uint8* state = SDL_GetKeyboardState(NULL); //Store key state in variable
        return state[SDL_GetScancodeFromKey(key)];
    }

    bool mouseDown(int button) {
        bool down = false;

        if (!pMousesPressed[button] && mousesPressed[button]) {
            down = true;
        }

        return down;
    }

    bool mouseUp(int button) {
        bool up = false;

        if (pMousesPressed[button] && !mousesPressed[button]) {
            up = true;
        }

        return up;
    }

    bool mouseHeld(int button) {
        SDL_PumpEvents();
        return SDL_GetMouseState(NULL, NULL)&button;
    }

    void mousePotition(int* x, int* y) {
        SDL_PumpEvents();
        SDL_GetMouseState(x, y);
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
        drawLine(x + w, y, x + w, y + h, t, r, g, b, a);
        drawLine(x + w, y + h, x, y + h, t, r, g, b, a);
        drawLine(x, y + h, x, y, t, r, g, b, a);
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

    void drawBMPImageEx(int x, int y, int w, int h, SDL_RendererFlip f, float a, int cx, int cy, const char* img, bool t, int r, int g, int b) {
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

        SDL_Point center;
        center.x = cx;
        center.y = cy;

        SDL_RenderCopyEx(renderer, texture, NULL, &rect, a, &center, f);
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
        stbi_image_free(data);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    void drawImageEx(int x, int y, int w, int h, SDL_RendererFlip f, float a, int cx, int cy, const char* img) {
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

        SDL_Point center;
        center.x = cx;
        center.y = cy;

        SDL_RenderCopyEx(renderer, texture, NULL, &rect, a, &center, f);
        stbi_image_free(data);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    SDL_Surface* loadImageToSurface(const char* img)
    {
        SDL_Surface* surface = NULL;
        int img_w, img_h, comp;
        unsigned char* data = stbi_load(img, &img_w, &img_h, &comp, STBI_rgb_alpha);

        surface = SDL_CreateRGBSurfaceFrom(data, img_w, img_h, 32, 4 * img_w, 0xFF, 0xFF00, 0xFF0000, 0xFF000000);
        stbi_image_free(data);

        return surface;
    }

    SDL_Texture* loadImage(const char* img)
    {
        SDL_Texture* texture = NULL;
        SDL_Surface* surface = NULL;
        int img_w, img_h, comp;
        unsigned char* data = stbi_load(img, &img_w, &img_h, &comp, STBI_rgb_alpha);

        surface = SDL_CreateRGBSurfaceFrom(data, img_w, img_h, 32, 4 * img_w, 0xFF, 0xFF00, 0xFF0000, 0xFF000000);
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        stbi_image_free(data);
        SDL_FreeSurface(surface);

        return texture;
    }

    SDL_Texture* loadBMPImage(const char* img, bool t, int r, int g, int b)
    {
        SDL_Texture* texture = NULL;
        SDL_Surface* surface = NULL;
        surface = SDL_LoadBMP(img);

        if (t) {
            Uint32 colorKey = SDL_MapRGB(surface->format, r, g, b);
            SDL_SetColorKey(surface, SDL_TRUE, colorKey);
        }

        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);

        return texture;
    }

    void renderImage(int x, int y, int w, int h, SDL_Texture *texture) {
        SDL_Rect rect;
        rect.x = x;
        rect.y = y;
        rect.w = w;
        rect.h = h;

        SDL_RenderCopy(renderer, texture, NULL, &rect);
    }

    void renderImageEx(int x, int y, int w, int h, SDL_RendererFlip f, float a, int cx, int cy, SDL_Texture *texture) {
        SDL_Rect rect;
        rect.x = x;
        rect.y = y;
        rect.w = w;
        rect.h = h;

        SDL_Point center;
        center.x = cx;
        center.y = cy;

        SDL_RenderCopyEx(renderer, texture, NULL, &rect, a, &center, f);
    }

    void drawText(int x, int y, int s, const char* dir, const char* txt, int r, int g, int b, int a) {
        stbtt_fontinfo font;
        unsigned char* bitmap = 0;
        if(ttfBuffer == NULL)
        {
            FILE* file = fopen(dir, "rb");

            if (!file) {
                return;
            }

            fseek(file, 0, SEEK_END);
            fSize = ftell(file);
            rewind(file);
            ttfBuffer = (unsigned char *) malloc(fSize);
            fread(ttfBuffer, 1, fSize, file);
            fclose(file);
        }

        stbtt_InitFont(&font, ttfBuffer, stbtt_GetFontOffsetForIndex(ttfBuffer, 0));
        float scale = stbtt_ScaleForPixelHeight(&font, s);
        int ascent, descent;
        stbtt_GetFontVMetrics(&font, &ascent, &descent, 0);

        int advance, x0, y0, w, h, xPos = 10, yPos = 10 + scale * ascent;

        for (int ind = 0; txt[ind]; ind++) {
            bitmap = stbtt_GetCodepointBitmap(&font, 0, stbtt_ScaleForPixelHeight(&font, s), txt[ind], &w, &h, 0, 0);
            stbtt_GetCodepointHMetrics(&font, txt[ind], &advance, 0);
            stbtt_GetCodepointBitmapBox(&font, txt[ind], scale, scale, &x0, &y0, 0, 0);

            int xp = xPos, yp = yPos;

            for (int j = 0; j < h; j++) {
                for (int i = 0; i < w; i++) {
                    if (bitmap[j * w + i]) {
                        drawPixel(xp + x0 + x, yp + y0 + y, r, g, b, a);
                    }
                    xp++;
                }
                xp = xPos;
                yp++;
            }

            stbtt_FreeBitmap(bitmap, 0);
            if (!txt[ind + 1]) break;
            xPos += scale * advance;
        }

    }

    bool isColliding(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
        bool colliding = (x1 < (x2 + w2)) && (x2 < (x1 + w1)) && (y1 < (y2 + h2)) && (y2 < (y1 + h1));

        return colliding;
    }

    void pauseAudio(int deviceId) {
        SDL_PauseAudioDevice(deviceId, 1);
    }

    void startAudio(int deviceId) {
        SDL_PauseAudioDevice(deviceId, 0);
    }

    void playAudio(const char *dir, int length) {
        audioThread = std::thread(std::ref(playWav), dir, length);
        audioThread.detach();
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
    static void audioUpdate();

private:
    int fSize;
    unsigned char* ttfBuffer;

};

void playWav(const char* audioDir, int audioTime) {
    SDL_AudioSpec wavSpec;
    Uint32 wavLength;
    Uint8* wavBuffer;

    if (SDL_LoadWAV(audioDir, &wavSpec, &wavBuffer, &wavLength) == NULL) {
        return;
    }

    audioPos = wavBuffer;
    audioLen = wavLength;

    SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);

    SDL_QueueAudio(deviceId, wavBuffer, wavLength);
    SDL_PauseAudioDevice(deviceId, 0);
 
    SDL_Delay(audioTime);

    SDL_PauseAudioDevice(deviceId, 1);
    SDL_CloseAudioDevice(deviceId);
    SDL_FreeWAV(wavBuffer);
}