#pragma once
#include "any_game.hpp"
#include "Drawing.hpp"

class Raider : public Drawing {
public:
	int id;
	Vector2 pos;

	weak_ptr<Target> target;

	weak_ptr<Target> prey = weak_ptr<Target>();

	Raider(DEPTH z, int id, Vector2 pos, weak_ptr<Target> combat);

	void update(vector<weak_ptr<Target>> targets);

	void draw() const;
};