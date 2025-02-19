#pragma once
#include "any_game.hpp"

class Resource {
public:
	int id;
	Vector2 pos;
	float r = 5.0f;

	int type;

	bool occupied = false;
	bool taken = false;

	Resource(int id, Vector2 pos, int type);

	void draw() const;
};