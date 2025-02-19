#pragma once
#include "any_game.hpp"

class Raider {
public:
	int id;
	Vector2 pos;

	weak_ptr<Combat> combat;

	weak_ptr<Combat> target = weak_ptr<Combat>();

	Raider(int id, Vector2 pos, weak_ptr<Combat> combat);

	void update(vector<shared_ptr<Combat>> targets);

	void draw() const;
};