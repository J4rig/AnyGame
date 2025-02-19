#pragma once
#include "Task.hpp"

Task::Task(int id, Vector2 pos, int priority, float work_to_do, int max_workers) :
	id(id), pos(pos), priority(priority), work_to_do(work_to_do), max_workers(max_workers) {
};


bool Task::hasWorkers() const {
	return current_workers > 0;
}

bool Task::isFullyOccupied() const {
	return current_workers >= max_workers;
}

bool Task::isCompleted() const {
	return work_done >= work_to_do;
}