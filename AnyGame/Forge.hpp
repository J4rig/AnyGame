#pragma once
#include "any_game.hpp"

class Forge {
public:

	int id;
	Vector2 pos;
	float r;

	weak_ptr<Storage> storage;
	weak_ptr<Task> task;

	int type = 3;

	Forge(int id, Vector2 pos, float r, weak_ptr<Storage> storage);

	void draw() const;

};