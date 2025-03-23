#pragma once
#include "any_game.hpp"
#include "Drawing.hpp"

class Forge : public Drawing {
public:

	int id;
	int tribe;
	Vector2 pos;
	shared_ptr<float> r;

	array<int, MAX_TYPE> recipe = {};
	array<int, MAX_TYPE> produce = {};

	weak_ptr<Storage> storage_in = weak_ptr<Storage>();
	weak_ptr<Storage> storage_out = weak_ptr<Storage>();
	weak_ptr<Task> task = weak_ptr<Task>();
	weak_ptr<Target> target;
	weak_ptr<Construction> construction;

	

	Forge(DEPTH z, int id, int tribe, Vector2 pos, shared_ptr<float> r, array<int, MAX_TYPE> recipe, array<int, MAX_TYPE> produce, weak_ptr<Construction> construction, weak_ptr<Target> target);

	void draw() const;

};

tuple<shared_ptr<Storage>, shared_ptr<Construction>, shared_ptr<Target>, shared_ptr<Forge>> createForge(array<int, MAX_TYPE> recipe, array<int, MAX_TYPE> produce, Vector2 pos);