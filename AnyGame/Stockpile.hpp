#pragma once
#include "any_game.hpp"

class Stockpile {
public:
	int id;

	Vector2 pos;
	float r;

	weak_ptr<Construction> construction;
	weak_ptr<Storage> storage;

	Stockpile(int id, Vector2 pos, float r, weak_ptr < Construction> construction, weak_ptr <Storage> storage);

	void draw() const;
};