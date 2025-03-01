#pragma once
#include "any_game.hpp"

class Forge {
public:

	int id;
	Vector2 pos;
	float r;

	weak_ptr<Storage> storage = weak_ptr<Storage>();
	weak_ptr<Task> task = weak_ptr<Task>();
	weak_ptr<Target> target;
	weak_ptr<Construction> construction;

	int type = 3;

	Forge(int id, Vector2 pos, float r, weak_ptr<Construction> construction, weak_ptr<Target> target);

	void draw() const;

};