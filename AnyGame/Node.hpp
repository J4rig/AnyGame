#pragma once
#include "any_game.hpp"
#include "Storage.hpp"
#include "Drawing.hpp"

class Node : public Drawing {
public:
	int id;
	
	Vector2 pos;
	shared_ptr<float> r;

	weak_ptr<Storage> storage;

	Node(DEPTH z, int id, Vector2 pos, shared_ptr<float> r, weak_ptr<Storage> storage);

	void update();

	void draw() const;
};

tuple<shared_ptr<Storage>, shared_ptr<Node>> createNode(array<int, MAX_TYPE> types, Vector2 pos);