#pragma once
#include "any_game.hpp"
#include "Drawing.hpp"

class Stockpile : public Drawing {
public:
	int id;
	int tribe;

	Vector2 pos;
	shared_ptr<float> r;

	weak_ptr<Construction> construction;
	weak_ptr<Storage> storage;
	weak_ptr<Target> target;

	function<void(Stockpile*)> target_expired;

	Stockpile(DEPTH z, int id, int tribe, Vector2 pos, shared_ptr<float> r, weak_ptr<Construction> construction, weak_ptr<Storage> storage, weak_ptr<Target> target);

	void die();

	void draw() const;
};

tuple<shared_ptr<Storage>, shared_ptr<Construction>, shared_ptr<Target>, shared_ptr<Stockpile>> createStockpile(Vector2 pos);