#pragma once
#include "any_game.hpp"
#include "Drawing.hpp"

class Mine : public Drawing {
public:
	int id;
	int tribe;
	Vector2 pos;
	shared_ptr<float> r;

	weak_ptr<Construction> construction;
	weak_ptr<Target> target;
	weak_ptr<Storage> storage;
	weak_ptr<Storage> generated;
	weak_ptr<Task> task = weak_ptr<Task>();

	Mine(DEPTH z, int id, int tribe, Vector2 pos, shared_ptr<float> r, weak_ptr<Construction> construction, weak_ptr<Target> target);

	void draw() const;
};

tuple<shared_ptr<Storage>, shared_ptr<Construction>, shared_ptr<Target>, shared_ptr<Mine>> createMine(Vector2 pos);