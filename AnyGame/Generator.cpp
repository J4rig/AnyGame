#pragma once
#include "Generator.hpp"
#include "Task.hpp"

#include <string>

Generator::Generator(int id, Vector2 pos, float r, int type, int max, float dispense_radius) :
	id(id), pos(pos), r(r), type(type), max(max), remaining(max), task(task), dispense_radius(dispense_radius) {
};

void Generator::draw() const {
	DrawRing(pos, 10, r, 0, 360, 0, DARKGRAY);
	DrawRing(pos, 10, r, 0, (float)remaining / (float)max * 360.0f, 0, type_color[type]);
	/*DrawText(("g: " + to_string(id)).c_str(), pos.x - 5, pos.y - r - 2, 15, RED);
	DrawText(("t: " + to_string(task.lock()->id) + " p: " + to_string(task.lock()->priority)).c_str(), pos.x + 5, pos.y + r - 2, 15, GREEN);
	DrawRectangleLines(pos.x - dispense_radius, pos.y - dispense_radius, 2 * dispense_radius, 2 * dispense_radius, RED);*/
}

bool Generator::isEmpty() const {
	return remaining <= 0;
}