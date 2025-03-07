#pragma once
#include "any_game.hpp"
#include "Storage.hpp"
#include "Drawing.hpp"

class Node : public Drawing {
public:
	int id;
	
	Vector2 pos;
	float r = 5.0f;

	weak_ptr<Storage> storage;

	Node(int z, int id, Vector2 pos, weak_ptr<Storage> storage);

	void update();

	void draw() const;
};