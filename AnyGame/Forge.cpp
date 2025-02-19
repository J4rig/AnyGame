#pragma once
#include "Forge.hpp"
#include "Storage.hpp"
#include "Task.hpp"

Forge::Forge(int id, Vector2 pos, float r, weak_ptr<Storage> storage) :
	id(id), pos(pos), r(r), storage(storage), task(weak_ptr<Task>()) {
};

void Forge::draw() const {
	DrawCircleLinesV(pos, r, type_color[type]);
	if (!task.expired()) {
		DrawRing(pos, r - 4, r, 0, 360 * (task.lock()->work_done / task.lock()->work_to_do), 0, type_color[type]);
	}

	float piece = 360.0f * (1.0f / (float)storage.lock()->capacity);
	int drawn_pieces = 0;
	DrawCircleLinesV(pos, r - 5, type_color[type]);
	for (int i = 0; i < MAX_TYPE; i++) {
		DrawRing(pos, 10, r - 5, drawn_pieces * piece, (storage.lock()->is[i] + drawn_pieces) * piece, 0, type_color[i]);
		drawn_pieces += storage.lock()->is[i];
	}
}