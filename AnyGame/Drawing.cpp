#pragma once
#include "any_game.hpp"
#include "Drawing.hpp"

Drawing::Drawing(DEPTH z) : z(z) {};

void Drawing::draw() const{
	printf("empty drawing\n");
}