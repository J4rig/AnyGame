#pragma once
#include "any_game.hpp"

class Drawing {
public :
	DEPTH z;

	Drawing(DEPTH z);

	virtual void draw() const ;
};