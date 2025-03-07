#pragma once
#include "any_game.hpp"

class Settlement {
public:
	int id;
	Vector2 pos; //centre of settlement - changes when new building is added
	float r; // radius form centre on which walls are built - changes when new building is added

	vector<shared_ptr<Storage>> storages;

	vector<shared_ptr<Task>> tasks;

	vector<shared_ptr<Generator>> generators;

	vector<shared_ptr<Stockpile>> stockpiles;

	vector<shared_ptr<Worker>> workers;

	vector<shared_ptr<Construction>> constructions;

	vector<shared_ptr<Forge>> forges;

	Settlement(int id, Vector2 pos, float r);
};