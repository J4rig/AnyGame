#pragma once
#include "any_game.hpp"

class Drawing {
public :
	int z;

	Drawing(int z);

	virtual void draw() const ;
};