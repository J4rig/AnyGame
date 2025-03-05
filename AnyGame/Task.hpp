#pragma once
#include "any_game.hpp"

class Task {
public:
	int id;

	Vector2 pos;

	int priority;

	float work_to_do;
	float work_done = 0.0f;

	int max_workers;
	int current_workers = 0;

	bool destroy = false;

	bool finished = false;

	Task(int id, Vector2 pos, int priority, float work_to_do, int max_workers);

	bool hasWorkers() const;
	bool isFullyOccupied() const;
	bool isCompleted() const;
};

inline auto const task_cmp_shared = [](shared_ptr<Task > left, shared_ptr<Task> right) {return left->priority > right->priority; };

inline auto const task_cmp_weak = [](weak_ptr<Task > left, weak_ptr<Task> right) {return left.lock()->priority > right.lock()->priority; };