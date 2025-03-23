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

tuple<shared_ptr<Storage>, shared_ptr<Node>>
createNode(array<int, MAX_TYPE> types, Vector2 pos) {
	shared_ptr<float> r = make_shared<float>(NODE_R);

	shared_ptr<Storage> new_storage = make_shared<Storage>(storage_id++, -1, pos, r, 0, resourceCount(types), types, true);

	new_storage->is = types;
	new_storage->will_be = types;

	shared_ptr<Node> new_node = make_shared<Node>(DEPTH::NODE, node_id++, pos, r, new_storage);

	return make_tuple(new_storage, new_node);
}