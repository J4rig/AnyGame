#pragma once
#include "any_game.hpp"
#include "Drawing.hpp"

class Settlement : public Drawing {
public:
	int id;
	int tribe;
	Vector2 pos; //centre of settlement - changes when new building is added
	float r; // radius form centre on which walls are built - changes when new building is added

	vector<shared_ptr<Task>> tasks;

	vector<shared_ptr<Mine>> mines;

	vector<shared_ptr<Stockpile>> stockpiles;

	vector<shared_ptr<Worker>> workers;

	vector<shared_ptr<Construction>> constructions;

	vector<shared_ptr<Forge>> forges;

	Settlement(DEPTH z, int tribe, int id, Vector2 pos, float r);

	void update();

	void draw() const;
};