#ifndef UTILS_H
#define UTILS_H

#define uint unsigned int

#include <HAPI_lib.h>

//
// Enumerator
//

enum Input
{
	null = 0,
	left,
	right,
	up,
	down,
	jump,
	jumpleft,
	jumpright,
	shoot,
	select,
	backspace,
	inputs
};

enum States
{
	play = 0,
	gameover,
	menu,
	states
};

//
// Structures
//

struct Rect{
	
	public:
		int x, y; 
		int width, height;
	public:
		Rect() : x(0), y(0), width(0), height(0) {}
		Rect(int X, int Y, int W, int H) : x(X), y(Y), width(W), height(H){}
		Rect percent(const float param) const { return Rect( (int)(x + (width - x)*(param/100.0)), (int)(y - (y - height)*(param/100.0)), (int)(width - (width - x)*(param/100.0)), (int)(height + (y - height)*(param/100.0)));}

};

struct Coord2D{

	public:
		int x, y;
	public:
		Coord2D() : x(0), y(0) {}
		Coord2D(int X, int Y) : x(X), y(Y){}
		Coord2D operator+(const Coord2D &param) const { return Coord2D( x + param.x, y + param.y);}
		Coord2D operator-(const Coord2D &param) const { return Coord2D( x - param.x, y - param.y);} 
		Coord2D operator*(const float param) const { return Coord2D((int)(x * param), (int)(y * param));}
		Coord2D operator/(const float param) const { return Coord2D((int)(x / param), (int)(y / param));}
		bool operator==(const Coord2D &param) const { return (x == param.x && y == param.y);}
		bool operator!=(const Coord2D &param) const { return (x != param.x && y != param.y);}
};

#endif //UTILS_H