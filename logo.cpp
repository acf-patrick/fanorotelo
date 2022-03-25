#include "logo.h"
#include <SDL/SDL_image.h>
#include <SDL/SDL_rotozoom.h>
#include <iostream>

Logo::Logo() {
    image = IMG_Load("logo.png");
    if (!image)
        std::cerr << "logo.png not found" << std::endl;
    else {
        const int width = SDL_GetVideoSurface()->w;
        const int padding = 15;

        image = rotozoomSurface(image, 0.0f, 0.75, 1);
        x = width - image->w;
        x -= padding;
        y += padding;
    }
}