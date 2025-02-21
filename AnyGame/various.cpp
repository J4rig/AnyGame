#include "any_game.hpp"

#include "Storage.hpp"
#include "Task.hpp"
#include "Resource.hpp"
#include "Target.hpp"

#include <iostream>
#include <algorithm>

shared_ptr<Resource> findClosestResource(Vector2 point, vector<shared_ptr<Resource>> resources, array<int, MAX_TYPE> valid_types) { //-1 if type is not important or number of type
	shared_ptr<Resource> result = nullptr;
	float min_dst = numeric_limits<float>::max();
	float new_distance;
	for (shared_ptr<Resource> r : resources) {
		if (!r->occupied && valid_types[r->type] > 0 && (new_distance = min(Vector2Distance(point, r->pos), min_dst)) != min_dst) {
			result = r;
			min_dst = new_distance;
		}
	}
	return result;
}

weak_ptr<Storage> findStorageToIdle(Vector2 point, vector<shared_ptr<Storage>> storages) {
	float min_distance = numeric_limits<float>::max();
	float new_distance;

	weak_ptr<Storage> result = weak_ptr<Storage>();

	for (shared_ptr<Storage> s : storages) {
		if ((new_distance = min(Vector2Distance(point, s->pos), min_distance)) != min_distance) {
			min_distance = new_distance;
			result = s;
		}
	}
	return result;
}


weak_ptr<Storage> findStorageToStore(Vector2 point, vector<shared_ptr<Storage>> storages, vector<int> types) {

	float min_distance = numeric_limits<float>::max();
	float new_distance;

	weak_ptr<Storage> result = weak_ptr<Storage>();

	for (shared_ptr<Storage> s : storages) {
		if (s->hasSpace(types) && (result.expired() || result.lock()->priority <= s->priority)) {
			if ((new_distance = min(Vector2Distance(point, s->pos), min_distance)) != min_distance || result.lock()->priority < s->priority) {
				min_distance = new_distance;
				result = s;
			}
		}
	}

	return result;
}

weak_ptr<Storage> findStorageToDeliver(Vector2 point, vector<shared_ptr<Storage>> storages) {
	vector<shared_ptr<Storage>> found_storages = vector<shared_ptr<Storage>>();

	array<array<int, MAX_TYPE>, MAX_PRIORITY> stored_resources = array<array<int, MAX_TYPE>, MAX_PRIORITY>();


	for (shared_ptr<Storage> s : storages) {
		if (s->priority >= MAX_PRIORITY) {
			cout << "WARNING: max_priority exceeded\n";
			s->priority = MAX_PRIORITY - 1;
		}

		for (int i = 0; i < MAX_TYPE; i++) {
			stored_resources[s->priority][i] += s->will_be[i];
		}
		found_storages.emplace_back(s);
	}

	sort(found_storages.begin(), found_storages.end(), storage_cmp_shared);

	for (int s = 0; s < found_storages.size(); s++) {
		bool found = false;
		for (int p = 0; p <= found_storages[s]->priority; p++) {
			for (int i = 0; i < MAX_TYPE; i++) {
				if (found_storages[s]->hasSpace(i) && stored_resources[p][i] > 0) {
					found = true;
					break;
				}
			}
			if (found) {
				break;
			}
		}
		if (found) {
			continue;
		}
		found_storages.erase(found_storages.begin() + s);
		s--;
	}

	float min_distance = numeric_limits<float>::max();
	float new_distance;

	weak_ptr<Storage> result = weak_ptr<Storage>();

	for (shared_ptr<Storage> s : found_storages) {
		if (result.expired() || result.lock()->priority <= s->priority) {
			if ((new_distance = min(Vector2Distance(point, s->pos), min_distance)) != min_distance || result.lock()->priority < s->priority) {
				min_distance = new_distance;
				result = s;
			}
		}
	}

	return result;
}

weak_ptr<Storage> findStorageToTake(Vector2 point, vector<shared_ptr<Storage>> storages, array<int, MAX_TYPE>& return_types, array<int, MAX_TYPE> wanted_types, int max_priority) {
	float min_distance = numeric_limits<float>::max();
	float new_distance;

	weak_ptr<Storage> result = weak_ptr<Storage>();

	for (shared_ptr<Storage> s : storages) {
		if (s->priority >= max_priority || !s->can_take) {
			continue;
		}

		std::array<int, MAX_TYPE> tmp = hasTypes(s->will_be, wanted_types);
		if (resourceCount(tmp) > 0 && (new_distance = min(Vector2Distance(point, s->pos), min_distance)) != min_distance) {
			return_types = tmp;
			min_distance = new_distance;
			result = s;
		}
	}

	return result;
}

weak_ptr<Task> findTask(Vector2 point, vector<shared_ptr<Task>> tasks) {
	float min_distance = numeric_limits<float>::max();
	float new_distance;

	weak_ptr<Task> result = weak_ptr<Task>();

	for (shared_ptr<Task> t : tasks) {

		if (!t->isFullyOccupied() && !t->isCompleted() && (result.expired() || result.lock()->priority <= t->priority)) {
			if ((new_distance = min(Vector2Distance(point, t->pos), min_distance)) != min_distance) {
				min_distance = new_distance;
				result = t;
			}
		}
	}
	return result;
}


weak_ptr<Target> findTarget(Vector2 point, weak_ptr<Target> attacker, vector<weak_ptr<Target>> targets) {
	float min_distance = numeric_limits<float>::max();
	float new_distance;

	weak_ptr<Target> result = weak_ptr<Target>();

	for (weak_ptr<Target> t : targets) {
		if (t.expired() || attacker.lock()->tribe == t.lock()->tribe) {
			continue;
		}
		if ((new_distance = min(Vector2Distance(point, (*t.lock()->pos)), min_distance)) != min_distance) {
			min_distance = new_distance;
			result = t;
		}
	}
	return result;
}



array<int, MAX_TYPE> hasTypes(array<int, MAX_TYPE> stored_types, array<int, MAX_TYPE> types) {
	array<int, MAX_TYPE> result = { 0 };
	for (int i = 0; i < MAX_TYPE; i++) {
		if (types[i] > 0 && stored_types[i] > 0) {
			result[i] = min(types[i], stored_types[i]);
		}
	}
	return result;
}



// rotates point position around center point by specified angle
Vector2 rotateAroundPoint(Vector2 point, Vector2 center, float angleInRads) {
	float s = sin(angleInRads);
	float c = cos(angleInRads);

	point = Vector2Subtract(point, center);

	Vector2 new_point = { point.x * c - point.y * s, point.x * s + point.y * c };

	return Vector2Add(new_point, center);
}



bool hasType(array<int, MAX_TYPE> stored_types, int type) {
	return stored_types[type] > 0;
}



array<int, MAX_TYPE> arangeTypes(vector<int> types) {
	array<int, MAX_TYPE> result = { 0 };
	for (int i : types) {
		result[i]++;
	}
	return result;
}



int resourceCount(array<int, MAX_TYPE> stored_types) {
	int result = 0;
	for (int i = 0; i < MAX_TYPE; i++) {
		result += stored_types[i];
	}
	return result;
}



queue<int> cutToCapacity(array<int, MAX_TYPE> stored_types, int capacity) {
	queue<int> result;
	for (int i = 0; i < MAX_TYPE; i++) {
		for (int j = 0; j < stored_types[i]; j++) {
			if (result.size() < capacity) {
				result.push(i);
			}
			else break;
		}
	}
	return result;
}

array<int, MAX_TYPE> canBeStored(vector<shared_ptr<Storage>> storages) {
	int amount = 0;
	array<int, MAX_TYPE> result = { 0 };
	for (shared_ptr<Storage> s : storages) {
		for (int i = 0; i < MAX_TYPE; i++) {
			if ((amount = s->spaceLeft(i)) > 0) {
				result[i] += amount;
			}
		}
	}
	return result;
}

void insertStorageShared(vector<shared_ptr<Storage>>& storages, shared_ptr<Storage> storage) {
	auto pos = lower_bound(storages.begin(), storages.end(), storage, storage_cmp_shared);
	storages.insert(pos, storage);
}

void insertStorageWeak(vector<weak_ptr<Storage>>& storages, shared_ptr<Storage> storage) {
	auto pos = lower_bound(storages.begin(), storages.end(), storage, storage_cmp_weak);
	storages.insert(pos, storage);
}

void insertTask(vector<shared_ptr<Task>>& tasks, shared_ptr<Task> task) {
	auto pos = lower_bound(tasks.begin(), tasks.end(), task, task_cmp);
	tasks.insert(pos, task);
}