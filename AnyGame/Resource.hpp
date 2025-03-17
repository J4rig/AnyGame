#pragma once
#include "any_game.hpp"

class Resource {
public:
	int type;
	array<bool, MAX_TRIBE> occupied;
	bool ready = false;

	Resource(int type);
};