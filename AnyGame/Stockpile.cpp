#pragma once
#include "Stockpile.hpp"
#include "Storage.hpp"
#include "Construction.hpp"
#include "Target.hpp"

#include <string>

Stockpile::Stockpile(DEPTH z, int id, int tribe, Vector2 pos, shared_ptr<float> r, weak_ptr <Construction> construction, weak_ptr <Storage> storage, weak_ptr<Target> target) :
	Drawing(z), id(id), tribe(tribe), pos(pos), r(r), construction(construction), storage(storage), target(target) {
};

void Stockpile::draw() const {
	if (construction.expired()) {
		DrawRing(pos, *r - 2, *r, 0, 360, 0, tribe_color[tribe]);

		//TODO change, not to calculate all the time but only recalculate when capacity changes
		float piece = 360.0f * (1.0f / (float)storage.lock()->capacity);
		int drawn_pieces = 0;
		for (int i = 0; i < MAX_TYPE; i++) {
			DrawRing(pos, 0, *r - 1, drawn_pieces * piece, (storage.lock()->is[i] + drawn_pieces) * piece, 0, type_color[i]);
			drawn_pieces += storage.lock()->is[i];
		}

		DrawText((to_string(storage.lock()->isStored()) + "/" + to_string(storage.lock()->capacity)).c_str(), (int)pos.x - 10, (int)pos.y, 10, WHITE);
		//DrawText(("s: " + to_string(id)).c_str(),pos.x-5,pos.y-r-2,15,RED);
		//DrawText(("r: " + to_string(storage.lock()->id) + " p: " + to_string(storage.lock()->priority)).c_str(), pos.x + 5, pos.y + *r - 2, 15, GREEN);
	}
	else if (!construction.lock()->storage.expired()) {
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

tuple<shared_ptr<Storage>, shared_ptr<Construction>, shared_ptr<Target>, shared_ptr<Stockpile>>
createStockpile(Vector2 pos) {
	shared_ptr<float> r = make_shared<float>(STOCKPILE_R);
	//construction storage
	std::array<int, MAX_TYPE> limits = { 0 };
	limits[0] = 2;
	shared_ptr<Storage> new_storage = make_shared<Storage>(storage_id++, selected_tribe, pos, r, 3, 2, limits, false);

	//construction
	shared_ptr<Construction> new_construction = make_shared<Construction>(construction_id++, pos, new_storage, weak_ptr<Task>());

	//target
	shared_ptr<Target> new_target = make_shared<Target>(target_id++, selected_tribe, nullptr, 10.0f, 30, 0, 0);

	//stockpile
	shared_ptr<Stockpile> new_stockpile = make_shared<Stockpile>(DEPTH::STOCKPILE, stockpile_id++, selected_tribe, pos, r, new_construction, weak_ptr<Storage>(), new_target);
	new_stockpile->target.lock()->pos = &new_stockpile->pos;
	return make_tuple(new_storage, new_construction, new_target, new_stockpile);
}