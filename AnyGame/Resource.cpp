#pragma once
#include "Resource.hpp"

Resource::Resource(int id, Vector2 pos, int type) :
	id(id), pos(pos), type(type) {
};

void Resource::draw() const {
	DrawCircleV(pos, r, type_color[type]);
	//DrawText(("r: " + to_string(id)).c_str(), pos.x - 5, pos.y - r - 2, 15, RED);
}