#include <iostream>
#include <SDL2/SDL.h>

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

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture *sdlTexture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, screenWidth, screenHeight);
    Uint32 *pixels = new Uint32[screenWidth * screenHeight];
    memset(pixels, 255, screenWidth * screenHeight * sizeof(Uint32));

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

    while (!quit)
    {
        SDL_UpdateTexture(sdlTexture, NULL, pixels, screenWidth * sizeof(Uint32));
        SDL_PollEvent(&event);

        switch (event.type)
        {
        case SDL_QUIT:
            quit = true;
            break;
        }
    
        animation = SDL_GetTicks() / 1000.0;
        //calculate the shift values out of the animation value
        int shiftX = int(texWidth * 1.0 * animation);
        int shiftY = int(texHeight * 0.25 * animation);        
                        
        for(int x = 0; x < screenWidth; x++)
            for(int y = 0; y < screenHeight; y++)
            {
                //get the texel from the texture by using the tables, shifted with the animation values
                int color = texture[(unsigned int)(distanceTable[x][y] + shiftX)  % texWidth][(unsigned int)(angleTable[x][y] + shiftY) % texHeight];
                pixels[y * screenWidth + x] = color;
            }


        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, sdlTexture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    delete[] pixels;
    SDL_DestroyTexture(sdlTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
