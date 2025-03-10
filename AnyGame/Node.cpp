#pragma once
#include "Node.hpp"

Node::Node(DEPTH z, int id, Vector2 pos, shared_ptr<float> r, weak_ptr<Storage> storage) :
	Drawing(z), id(id), pos(pos), r(r), storage(storage) {
};

void Node::update() {
	*r = 5.0f + 2.0f * storage.lock()->isStored();
}

void Node::draw() const {
	
	DrawRing(pos, *r - 2, *r, 0, 360, 0, DARKGRAY);

	float piece = 360.0f * (1.0f / (float)storage.lock()->capacity);
	int drawn_pieces = 0;
	for (int i = 0; i < MAX_TYPE; i++) {
		DrawRing(pos, 0, *r - 1, drawn_pieces * piece, (storage.lock()->is[i] + drawn_pieces) * piece, 0, type_color[i]);
		drawn_pieces += storage.lock()->is[i];
	}
};