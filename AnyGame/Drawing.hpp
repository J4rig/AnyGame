#pragma once
#include "any_game.hpp"

class Drawing {
public :
	DEPTH z;

	Drawing(DEPTH z);

	virtual void draw() const ;
};

inline auto const drawing_cmp = [](weak_ptr<Drawing> left, weak_ptr<Drawing> right) {return left.lock()->z < right.lock()->z; };