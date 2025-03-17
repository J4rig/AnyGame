#pragma once
#include "any_game.hpp"

class Storage {
public:
	int id;

	int tribe;

	Vector2 pos;

	weak_ptr<float> r;

	int priority;

	int capacity;

	bool can_take;

	array<array<int, MAX_TYPE>, MAX_TRIBE> reserved = {};
	array<int, MAX_TYPE> is = {};
	array<int, MAX_TYPE> will_be = {};
	array<int, MAX_TYPE> can_be = {};

	Storage(int id, int tribe, Vector2 pos, weak_ptr<float> r, int priority, int capacity, array<int, MAX_TYPE> limits, bool can_take);

	int isStored();
	int aboutToBeStored();

	bool isFull(int type);
	bool isEmpty();
	bool hasSpace(int type);
	bool hasSpace(vector<int> types);
	int spaceLeft(int type);
};

inline auto const storage_cmp_shared = [](shared_ptr<Storage> left, shared_ptr<Storage> right) {return left->priority > right->priority; };