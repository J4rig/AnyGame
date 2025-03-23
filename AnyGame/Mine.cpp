#pragma once
#include "Mine.hpp"
#include "Storage.hpp"
#include "Task.hpp"
#include "Construction.hpp"

#include <string>

Mine::Mine(DEPTH z, int id, int tribe, Vector2 pos, shared_ptr<float> r, weak_ptr<Construction> construction, weak_ptr<Target> target) :
	Drawing(z), id(id), tribe(tribe), pos(pos), r(r), construction(construction), target(target) {
};

void Mine::draw() const {
	if (construction.expired() && !storage.expired() && !generated.expired()) {
		DrawCircleLinesV(pos, *r, GRAY);
		if (!task.expired()) {
			DrawRing(pos, *r - 4, *r, 0, 360 * (task.lock()->work_done / task.lock()->work_to_do), 0, GRAY);
		}

		float piece = 180.0f * (1.0f / (float)storage.lock()->capacity);
		int drawn_pieces = 0;
		DrawCircleLinesV(pos, *r - 5, GRAY);
		for (int i = 0; i < MAX_TYPE; i++) {
			DrawRing(pos, 10, *r - 5, drawn_pieces * piece, (storage.lock()->is[i] + drawn_pieces) * piece, 0, type_color[i]);
			drawn_pieces += storage.lock()->is[i];
		}

		piece = 180.0f * (1.0f / (float)generated.lock()->capacity);
		drawn_pieces = 0;
		DrawCircleLinesV(pos, *r - 5, GRAY);
		for (int i = 0; i < MAX_TYPE; i++) {
			DrawRing(pos, 10, *r - 5, 180 + drawn_pieces * piece, 180 + (generated.lock()->is[i] + drawn_pieces) * piece, 0, type_color[i]);
			drawn_pieces += generated.lock()->is[i];
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