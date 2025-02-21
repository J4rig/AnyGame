#pragma once
#include "any_game.hpp"

class Raider {
public:
	int id;
	Vector2 pos;

	weak_ptr<Target> combat;

	weak_ptr<Target> target = weak_ptr<Target>();

	Raider(int id, Vector2 pos, weak_ptr<Target> combat);

	void update(vector<weak_ptr<Target>> targets);

	void draw() const;
};