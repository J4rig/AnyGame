#pragma once
#include "any_game.hpp"

class Storage {
public:
	int id;

	Vector2 pos;

	int priority;

	int capacity;

	bool can_take;

	bool remove = false;

	array<int, MAX_TYPE> is = { 0 };
	array<int, MAX_TYPE> will_be = { 0 };
	array<int, MAX_TYPE> can_be = { 0 };

	Storage(int id, Vector2 pos, int priority, int capacity, array<int, MAX_TYPE> limits, bool can_take);

	int isStored();
	int aboutToBeStored();

	bool isFull(int type);
	bool hasSpace(int type);
	bool hasSpace(vector<int> types);
	int spaceLeft(int type);
};

inline auto const storage_cmp_shared = [](shared_ptr<Storage> left, shared_ptr<Storage> right) {return left->priority > right->priority; };

inline auto const storage_cmp_weak = [](weak_ptr<Storage> left, weak_ptr<Storage> right) {return left.lock()->priority > right.lock()->priority; };