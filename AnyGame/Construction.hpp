#pragma once
#include "any_game.hpp"

class Construction {
public:
	int id;
	Vector2 pos;

	bool is_all_delivered = false;
	weak_ptr<Storage> storage;

	weak_ptr<Task> task;

	Construction(int id, Vector2 pos, weak_ptr<Storage> storage, weak_ptr<Task> task);
};