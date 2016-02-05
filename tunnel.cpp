#include <iostream>
#include <sstream>
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_image.h"

using namespace std;

int main(int argc, char ** argv)
{
    int texWidth = 256;
    int texHeight = 256;
    int screenWidth = 640;
    int screenHeight = 480;

    bool leftMouseButtonDown = false;
    bool quit = false;
    SDL_Event event;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("Tunnel Effect",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, 0);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture *sdlTexture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, screenWidth, screenHeight);
    Uint32 *pixels = new Uint32[screenWidth * screenHeight];
    memset(pixels, 255, screenWidth * screenHeight * sizeof(Uint32));

    TTF_Init();
    TTF_Font* Sans = TTF_OpenFont("Aaargh.ttf", 40); 
    if (Sans == NULL) 
    {
        cerr << "FONT NOT LOADED" << endl;
        exit(1);
    }

    SDL_Color White = {255, 255, 255};  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color

    SDL_Rect Message_rect;
    Message_rect.w = 300;
    Message_rect.h = 40;
    Message_rect.x = screenWidth - Message_rect.w - 10; 
    Message_rect.y = screenHeight - Message_rect.h - 10; 

    int texture[texWidth][texHeight];
    int distanceTable[screenWidth][screenHeight];
    int angleTable[screenWidth][screenHeight];
    
    // create texture
    for(int x = 0; x < texWidth; x++)
        for(int y = 0; y < texHeight; y++)
            texture[x][y] = (x * 256 / texWidth) ^ (y * 256 / texHeight);

    //generate non-linear transformation table
    for(int x = 0; x < screenWidth; x++)
        for(int y = 0; y < screenHeight; y++)
        {
            int angle, distance;
            float ratio = 32.0;
            distance = int(ratio * texHeight / sqrt((x - screenWidth / 2.0) * (x - screenWidth / 2.0) + (y - screenHeight / 2.0) * (y - screenHeight / 2.0))) % texHeight;
            angle = (unsigned int)(0.5 * texWidth * atan2(y - screenHeight / 2.0, x - screenWidth / 2.0) / 3.1416);
            distanceTable[x][y] = distance;
            angleTable[x][y] = angle;
        }
    float animation;
    float depth = 0.6;
    float turnSpeed = 0.1;

    while (!quit)
    {
        SDL_UpdateTexture(sdlTexture, NULL, pixels, screenWidth * sizeof(Uint32));
        SDL_PollEvent(&event);

        switch (event.type)
        {
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_LEFT:
                turnSpeed -= 0.01;
                break;
            case SDLK_RIGHT:
                turnSpeed += 0.01;
                break;
            case SDLK_UP:
                depth += 0.01;
                break;
            case SDLK_DOWN:
                depth -= 0.01;
                break;
            }
            break;
        case SDL_QUIT:
            quit = true;
            break;
        }
    
        animation = SDL_GetTicks() / 1000.0;
        //calculate the shift values out of the animation value
        int shiftX = int(texWidth * depth * animation);
        int shiftY = int(texHeight * turnSpeed * animation);        
                        
        for(int x = 0; x < screenWidth; x++)
            for(int y = 0; y < screenHeight; y++)
            {
                //get the texel from the texture by using the tables, shifted with the animation values
                int color = texture[(unsigned int)(distanceTable[x][y] + shiftX)  % texWidth][(unsigned int)(angleTable[x][y] + shiftY) % texHeight];
                pixels[y * screenWidth + x] = color;
            }
        stringstream ss;
        ss << "depth: " << depth << ", turnSpeed: " << turnSpeed;
        SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Sans, ss.str().c_str(), White); // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first
        if (surfaceMessage == NULL) 
        {
            cerr << "surfaceMessage is NULL" << endl;
            exit(1);
        }

        SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage); //now you can convert it into a texture
        if (Message == NULL) 
        {
            cerr << "Message is NULL" << endl;
            exit(1);
        }

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, sdlTexture, NULL, NULL);
        SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
        SDL_RenderPresent(renderer);
    }

    delete[] pixels;
    TTF_CloseFont(Sans);
    SDL_DestroyTexture(sdlTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
