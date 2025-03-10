#pragma once
#include "Forge.hpp"
#include "Storage.hpp"
#include "Task.hpp"
#include "Construction.hpp"

Forge::Forge(DEPTH z,int id, int tribe, Vector2 pos, shared_ptr<float> r, array<int, MAX_TYPE> recipe, array<int, MAX_TYPE> produce,
	weak_ptr<Construction> construction, weak_ptr<Target> target) :
	Drawing(z), id(id), pos(pos), r(r), recipe(recipe), produce(produce), construction(construction), target(target) {
};

void Forge::draw() const {

	if (construction.expired() && !storage_in.expired() && !storage_out.expired()) {
		DrawCircleLinesV(pos, *r, GRAY);
		if (!task.expired()) {
			DrawRing(pos, *r - 4, *r, 0, 360 * (task.lock()->work_done / task.lock()->work_to_do), 0, GRAY);
		}

		float piece = 180.0f * (1.0f / (float)storage_in.lock()->capacity);
		int drawn_pieces = 0;
		DrawCircleLinesV(pos, *r - 5, GRAY);
		for (int i = 0; i < MAX_TYPE; i++) {
			DrawRing(pos, 10, *r - 5, drawn_pieces * piece, (storage_in.lock()->is[i] + drawn_pieces) * piece, 0, type_color[i]);
			drawn_pieces += storage_in.lock()->is[i];
		}

		piece = 180.0f * (1.0f / (float)storage_out.lock()->capacity);
		drawn_pieces = 0;
		DrawCircleLinesV(pos, *r - 5, GRAY);
		for (int i = 0; i < MAX_TYPE; i++) {
			DrawRing(pos, 10, *r - 5, 180 + drawn_pieces * piece, 180 + (storage_out.lock()->is[i] + drawn_pieces) * piece, 0, type_color[i]);
			drawn_pieces += storage_out.lock()->is[i];
		}

	}
	else if (!construction.expired() && !construction.lock()->storage.expired()) {
		DrawRing(pos, *r - 2, *r, 0, 360, 0, RED);
		float piece = 360.0f * (1.0f / (float)construction.lock()->storage.lock()->capacity);
		int drawn_pieces = 0;
		for (int i = 0; i < MAX_TYPE; i++) {
			int pieces = construction.lock()->storage.lock()->can_be[i] - construction.lock()->storage.lock()->is[i];
			DrawRing(pos, 0, *r - 1, drawn_pieces * piece, (pieces + drawn_pieces) * piece, 0, type_color[i]);
			drawn_pieces += pieces;
		}
	}
	else {
		DrawRing(pos, *r - 2, *r, 0, 360, 0, RED);
	}
}