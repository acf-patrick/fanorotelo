#include "pawn.h"
#include "const.h"
#include "base/func_tool.h"
#include <SDL_gfxPrimitives.h>

SDL_Rect Piece::board_top_left;
int Piece::diameter(32);

Piece::Piece(int _x, int _y, bool color) : r_x(_x), r_y(_y)
{
    image = SDL_CreateRGBSurface(SDL_HWSURFACE, diameter, diameter, 32, RMASK, GMASK, BMASK, AMASK);
    shadow = SDL_CreateRGBSurface(SDL_HWSURFACE, diameter, diameter, 32, RMASK, GMASK, BMASK, AMASK);
    if (!image or !shadow)
    {
		std::cerr << "Erreur lors de la création d'un pion!";
		exit(EXIT_FAILURE);
    }

    filledCircleColor(shadow, 0.5*diameter, 0.5*diameter, 0.5*diameter, PAWN_SHADOW);
    filledCircleColor(image, 0.5*diameter, 0.5*diameter, 0.5*diameter, color?WHITE_PAWN:BLACK_PAWN);
    circleColor(image, 0.5*diameter, 0.5*diameter, 0.5*diameter, 0xeeeeee55);
}

Piece::~Piece()
{
	SDL_FreeSurface(shadow);
	shadow = NULL;
}

void Piece::update()
{
	x = r_x*0.5*board_top_left.w - 0.5*diameter + board_top_left.x;
	y = r_y*0.5*board_top_left.h - 0.5*diameter + board_top_left.y;
}

void Piece::draw(SDL_Surface* screen)
{
    SDL_Rect shadow_pos = { Sint16(x+5), Sint16(y+5) };
    SDL_BlitSurface(shadow, NULL, screen, &shadow_pos);
    GameObject::draw(screen);
}