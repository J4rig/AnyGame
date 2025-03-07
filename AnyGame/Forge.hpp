#pragma once
#include "any_game.hpp"
#include "Drawing.hpp"

class Forge : public Drawing {
public:

	int id;
	Vector2 pos;
	float r;

	array<int, MAX_TYPE> recipe = {};
	array<int, MAX_TYPE> produce = {};

	weak_ptr<Storage> storage = weak_ptr<Storage>();
	weak_ptr<Task> task = weak_ptr<Task>();
	weak_ptr<Target> target;
	weak_ptr<Construction> construction;

	

	Forge(int z, int id, Vector2 pos, float r, array<int, MAX_TYPE> recipe, array<int, MAX_TYPE> produce, weak_ptr<Construction> construction, weak_ptr<Target> target);

	void draw() const;

};