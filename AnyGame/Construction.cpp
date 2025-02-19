#pragma once
#include "Construction.hpp"

Construction::Construction(int id, Vector2 pos, weak_ptr<Storage> storage, weak_ptr<Task> task) :
	id(id), pos(pos), storage(storage), task(task) {
};