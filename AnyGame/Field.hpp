#pragma once
#include "any_game.hpp"
#include "Drawing.hpp"

class Field : public Drawing {
	Vector2 pos;
	float r;

	float time_to_generate;
	float time_passed;


	Field(int z, Vector2 pos, float r);

	void draw() const;
};
