#pragma once
#include "any_game.hpp"

class Generator {
public:
	int id;
	Vector2 pos;
	float r;

	int type;

	int max;
	int remaining;

	weak_ptr<Task> task;

	float dispense_radius;

	Generator(int id, Vector2 pos, float r, int type, int max, float dispense_radius);

	void draw() const;

	bool isEmpty() const;
};
