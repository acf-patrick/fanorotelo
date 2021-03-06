#include "pawn.h"
#include "const.h"
#include "base/func_tool.h"
#include <SDL/SDL_gfxPrimitives.h>
#include <cmath>

Piece* Piece::selected(NULL);
Piece* Piece::moving(NULL);
SDL_Rect Piece::board_top_left;
int** Piece::board(NULL);
int* Piece::game_turn(NULL);
int Piece::diameter(32);
int Piece::instance(0);
int Piece::move_cnt[2] = { 0 };

Piece::Piece(int _x, int _y, bool color) : r_x(_x), r_y(_y), state(IDLE), x_vel(0), y_vel(0), acceleration(-0.015), moved(false) {
	instance++;
	rect.w = rect.h = diameter;
    image =  createSurface(diameter, diameter);
    shadow = createSurface(diameter, diameter);
    light = createSurface(diameter, diameter);
    
	if (!(image and shadow and light)) {
		std::cerr << "Erreur lors de la création d'un pion!";
		exit(EXIT_FAILURE);
    }

    filledCircleColor(light, 0.5*diameter, 0.5*diameter, 0.5*diameter,0x5a74a946);
    filledCircleColor(shadow, 0.5*diameter, 0.5*diameter, 0.5*diameter, PAWN_SHADOW);
    filledCircleColor(image, 0.5*diameter, 0.5*diameter, 0.5*diameter, color?WHITE_PAWN:BLACK_PAWN);
    zoom = createSurface(diameter+2, diameter+2);
    filledCircleColor(zoom, 0.5*zoom->w, 0.5*zoom->w, 0.5*zoom->w, color?WHITE_PAWN:BLACK_PAWN);
    circleColor(image, 0.5*diameter, 0.5*diameter, 0.5*diameter, 0xeeeeee55);
}

Piece::~Piece() {
	instance--;
	move_cnt[0] = move_cnt[1] = 0;
	SDL_FreeSurface(shadow);
	SDL_FreeSurface(light);
	SDL_FreeSurface(zoom);
	zoom = shadow = light = NULL;
}

float dist(int x1, int y1, int x2, int y2)
{ return std::sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1)); }

void Piece::update() {
	if (state == MOVING) {
		SDL_Rect r(get_SDL_coord(d_x, d_y));
		/* can't get higher precision than this */
		if (dist(std::ceil(x), std::ceil(y), r.x, r.y) > 2) {
            x += x_vel;
            y += y_vel;
            x_vel += acceleration*sgn(x_vel);
            y_vel += acceleration*sgn(y_vel);
		} else {
			x_vel = y_vel = 0;
			r_x = d_x;
			r_y = d_y;
			state = IDLE;
			moving = NULL;
		}
	} else {
		SDL_Rect r(get_SDL_coord(r_x, r_y));
		x = r.x;
		y = r.y;
	}
}

SDL_Rect Piece::get_SDL_coord(int rx, int ry) {
	SDL_Rect ret = { Sint16(ry*0.5*board_top_left.w - 0.5*diameter + board_top_left.x),
					Sint16(rx*0.5*board_top_left.h - 0.5*diameter + board_top_left.y) };
	return ret;
}

void Piece::draw(SDL_Surface* screen) {
    SDL_Rect pos = { Sint16(x+5), Sint16(y+5) };
    SDL_BlitSurface(shadow, NULL, screen, &pos);
	SDL_Surface* tmp(image);
	if (state == MOVING)
		image = zoom;
    GameObject::draw(screen);
    image = tmp;
    
	if (state == SELECTED) {
		pos.x = Sint16(x);
		pos.y = Sint16(y);
		SDL_BlitSurface(light, NULL, screen, &pos);
	}
}

void Piece::bump(const std::string& flag) {
	if (flag == "unselect")
		state = IDLE;
	else {
		state = SELECTED;
		selected = this;
	}
}

void Piece::unselect() {
	selected = NULL;
}

void Piece::move(int xDest, int yDest) {
	if (state != MOVING) {
		if (valid(r_x, r_y, xDest, yDest)) {
			(*game_turn) = !(*game_turn);
			if (!moved) {
				moved = true;
				move_cnt[board[r_x][r_y]]++;
			}
			state = MOVING;
			moving = this;
			std::swap(board[r_x][r_y], board[xDest][yDest]);
			d_x = xDest;
			d_y = yDest;
			x_vel = 2*sgn(d_y - r_y);
			y_vel = 2*sgn(d_x - r_x);
		}
		else
			state = IDLE;
	}
}

bool Piece::valid(int x0, int y0, int xDest, int yDest) {
    if (xDest < 0 or yDest < 0 or xDest >= 3 or yDest >= 3 or (x0 == xDest and y0 == yDest))
		return false;
	if (board[xDest][yDest] == black or board[xDest][yDest] == white)
		return false;
	float d(dist(x0, y0, xDest, yDest));
    if (1>d or d>std::sqrt(2))
		return false;
	/* diagonal move */
	if (sgn(xDest - x0) and sgn(yDest - y0))
        return x0%2 == y0%2;
	return true;
}

int Piece::get_x() const
{ return r_x; }
int Piece::get_y() const
{ return r_y; }

bool Piece::ready()
{ return instance >= 15; }

bool Piece::allMoved(int color)
{ return ((color>=2)?false:(move_cnt[color] == 3)); }
