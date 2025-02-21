#pragma once
#include "Worker.hpp"
#include "Storage.hpp"
#include "Task.hpp"
#include "Resource.hpp"

#include <iostream>

Worker::Worker(int id, int tribe, Vector2 pos, weak_ptr<Target> target, float speed) :
	id(id), tribe(tribe), pos(pos), target(target), speed(speed) {
};


void Worker::draw() const{
	DrawRing(pos, 4 /*resource radius -1 so it overlaps and hides imperfections*/, r, 0, 360, 0, tribe_color[tribe]);

	if (!collected_types.empty()) {
		//TODO change, not to calculate all the time but only recalculate when capacity changes
		float piece = 360.0f * (1.0f / (float)capacity);

		for (int i = 0; i < collected_types.size(); i++) {
			DrawRing(pos, 0, 5/*Resource radius*/, i * piece, (i + 1) * piece, 1, type_color[collected_types[i]]);
		}
	}
	//DrawText(("w: " + to_string(id)).c_str(), pos.x - 5, pos.y - r - 2, 15, RED);
}


bool Worker::isPacked() const {
	return types_to_deliver.size() >= capacity;
}

bool Worker::collectResources(vector<shared_ptr<Resource>> resources, vector<shared_ptr<Storage>> storages) {

	array<int, MAX_TYPE> can_be_stored = canBeStored(storages);

	int found_resources = 0;

	Vector2 lastPos = {};

	shared_ptr<Resource> new_targeted_resource = nullptr;

	while (found_resources < capacity) {
		if ((new_targeted_resource = findClosestResource(pos, resources, can_be_stored)) != nullptr) {
			new_targeted_resource->occupied = true;
			targeted_resources.emplace_back(new_targeted_resource);
			lastPos = new_targeted_resource->pos;
			can_be_stored[new_targeted_resource->type]--;
			found_resources++;
			new_targeted_resource = nullptr;
		}
		else {
			break;
		}
	}


	weak_ptr<Storage> new_targeted_storage = weak_ptr<Storage>();

	bool first = true;

	vector<int> types = {};
	for (weak_ptr<Resource> r : targeted_resources) {
		types.emplace_back(r.lock()->type);
	}

	vector<weak_ptr<Resource>> tmp_resources = targeted_resources;
	targeted_resources = {};

	while (!tmp_resources.empty()) {
		new_targeted_storage = findStorageToStore(lastPos, storages, types);
		if (!new_targeted_storage.expired()) {
			for (int r = 0; r < tmp_resources.size(); r++) {
				if (new_targeted_storage.lock()->hasSpace(tmp_resources[r].lock()->type)) {
					new_targeted_storage.lock()->will_be[tmp_resources.front().lock()->type]++;
					targeted_resources.emplace_back(tmp_resources[r]);
					tmp_resources.erase(tmp_resources.begin() + r);
					types.erase(types.begin() + r);
					r--;

					if (first) {
						while (!targeted_storages.empty()) {
							targeted_storages.pop();
						}
						first = false;
					}

					if (targeted_storages.empty() || new_targeted_storage.lock() != targeted_storages.front().lock()) {
						amount_to_deliver.push(1);
						cout << "found new storage: " << new_targeted_storage.lock()->id << " <-id\n";
						targeted_storages.push(new_targeted_storage);
					}
					else {
						amount_to_deliver.front()++;
					}

				}
			}

			new_targeted_storage = weak_ptr<Storage>();

		}
		else {
			tmp_resources.front().lock()->occupied = false;
			found_resources--;
			tmp_resources.erase(tmp_resources.begin());
		}
	}

	return found_resources > 0;
}


bool Worker::transportResources(vector<shared_ptr<Storage>> storages) {
	queue<weak_ptr<Storage>> deliver_queue = {};

	bool first = true;
	while (!isPacked()) {
		shared_ptr<Storage> deliver_to = findStorageToDeliver(pos, storages).lock();
		if (deliver_to == nullptr) {
			break;
		}

		array<int, MAX_TYPE> to_types = { 0 };

		for (int i = 0; i < MAX_TYPE; i++) {
			to_types[i] += deliver_to->can_be[i] - deliver_to->will_be[i];
		}

		array<int, MAX_TYPE> from_types = { 0 };

		shared_ptr<Storage> take_from = findStorageToTake(deliver_to->pos, storages, from_types, to_types, deliver_to->priority).lock();

		if (take_from == nullptr || take_from == deliver_to) {
			break;
		}

		queue<int> types_to_pickup = cutToCapacity(from_types, capacity - (int)types_to_deliver.size());

		if (first) {
			targeted_storages = {};
			first = false;
		}

		if (targeted_storages.empty() || targeted_storages.front().lock() != take_from) {
			targeted_storages.push(take_from);
			amount_to_take.push((int)types_to_pickup.size());
		}
		else {
			amount_to_take.front() += (int)types_to_pickup.size();
		}

		if (deliver_queue.empty() || deliver_queue.front().lock() != deliver_to) {
			deliver_queue.push(deliver_to);
			amount_to_deliver.push((int)types_to_pickup.size());

		}
		else {
			amount_to_deliver.front() += (int)types_to_pickup.size();
		}

		while (!types_to_pickup.empty()) {
			types_to_deliver.emplace_back(types_to_pickup.front());
			deliver_to->will_be[types_to_pickup.front()]++;
			take_from->will_be[types_to_pickup.front()]--;
			types_to_pickup.pop();
		}

	}

	while (!deliver_queue.empty()) {
		targeted_storages.push(deliver_queue.front());
		deliver_queue.pop();
	}

	return !types_to_deliver.empty();
}

bool Worker::completeTask(vector<shared_ptr<Task>> tasks) {
	targeted_task = findTask(pos, tasks);
	return !targeted_task.expired();
}



void Worker::update(vector<shared_ptr<Resource>> resources,
	vector<shared_ptr<Storage>> storages,
	vector<shared_ptr<Task>> tasks
) {
	WORKER_STATES original = state;
	if (state == WORKER_STATES::IDLE) {
		if (!tasks.empty() && completeTask(tasks)) {
			targeted_task.lock()->current_workers++;
			state = WORKER_STATES::OPERATING;
		}

		else if (!targeted_storages.empty() && !targeted_storages.front().expired()) {
			if (collectResources(resources, storages)) {
				state = WORKER_STATES::COLLECTING;

			}

			else if (transportResources(storages)) {
				state = WORKER_STATES::TRANSPORTING;
			}

			//todo make prettier
			else if (Vector2Distance(pos, targeted_storages.front().lock()->pos) <= 1.0f) {
				pos = Vector2Add(targeted_storages.front().lock()->pos, { 50.0f,0.0f });
			}

			else if (Vector2Distance(pos, targeted_storages.front().lock()->pos) <= 40 + 10) {
				pos = rotateAroundPoint(pos, targeted_storages.front().lock()->pos, 0.5f * GetFrameTime());
			}

			else {
				pos = Vector2MoveTowards(pos, targeted_storages.front().lock()->pos, SPEED_MOD * speed * GetFrameTime());
			}

		}
		else {
			if (!targeted_storages.empty() && targeted_storages.front().expired()) {
				targeted_storages.pop();
			}

			weak_ptr<Storage> tmp = findStorageToIdle(pos, storages);
			if (!tmp.expired()) {
				targeted_storages.push(tmp);
			}
		}
	}

	else if (state == WORKER_STATES::COLLECTING) {

		if (!targeted_storages.empty()) {
			queue<weak_ptr<Storage>> s = targeted_storages;
			cout << "tageted storages:\n";
			while (!s.empty()) {
				cout << s.front().lock()->id << " <-id ";
				s.pop();
			}
			cout << "\n";
		}

		if (!targeted_resources.empty()) {
			if (Vector2Distance(pos, targeted_resources.front().lock()->pos) <= 0.5f) {
				targeted_resources.front().lock()->taken = true;
				collected_types.emplace_back(targeted_resources.front().lock()->type);
				targeted_resources.erase(targeted_resources.begin());
			}
			else
			{
				pos = Vector2MoveTowards(pos, targeted_resources.front().lock()->pos, SPEED_MOD * speed * GetFrameTime());
			}
		}
		else {
			if (Vector2Distance(pos, targeted_storages.front().lock()->pos) <= 40 + 10) {

				cout << "delivering type: " << collected_types.back() << "\n";

				for (int i = 0; i < amount_to_deliver.front(); i++) {
					cout << "delivered to storage " << targeted_storages.front().lock()->id << " <-id\n";
					targeted_storages.front().lock()->is[collected_types.front()]++;
					collected_types.erase(collected_types.begin());
				}

				amount_to_deliver.pop();

				if (targeted_storages.size() > 1) {
					targeted_storages.pop();
				}
				else {
					state = WORKER_STATES::IDLE;
				}

			}
			else {
				pos = Vector2MoveTowards(pos, targeted_storages.front().lock()->pos, SPEED_MOD * speed * GetFrameTime());
			}
		}
	}

	else if (state == WORKER_STATES::TRANSPORTING) {
		if (!types_to_deliver.empty()) {
			if (Vector2Distance(pos, targeted_storages.front().lock()->pos) <= 40 + 10) {
				if (targeted_storages.front().lock()->is[types_to_deliver.front()] > 0) {
					targeted_storages.front().lock()->is[types_to_deliver.front()]--;

					collected_types.emplace_back(types_to_deliver.front());
					types_to_deliver.erase(types_to_deliver.begin());
					amount_to_take.front()--;
					if (amount_to_take.front() <= 0) {
						targeted_storages.pop();
						amount_to_take.pop();
					}
				}
				else { // waiting for resorce to be delivered
					pos = rotateAroundPoint(pos, targeted_storages.front().lock()->pos, 0.5f * GetFrameTime());
				}
			}
			else {
				pos = Vector2MoveTowards(pos, targeted_storages.front().lock()->pos, SPEED_MOD * speed * GetFrameTime());
			}
		}
		else {
			if (Vector2Distance(pos, targeted_storages.front().lock()->pos) <= 40 + 10) { // TODO fix distance parameter

				while (amount_to_deliver.front() > 0) {
					targeted_storages.front().lock()->is[collected_types.front()]++;
					collected_types.erase(collected_types.begin());
					amount_to_deliver.front()--;
				}

				amount_to_deliver.pop();
				targeted_storages.pop();

				if (collected_types.empty()) {
					state = WORKER_STATES::IDLE;
				}

			}
			else {
				pos = Vector2MoveTowards(pos, targeted_storages.front().lock()->pos, SPEED_MOD * speed * GetFrameTime());
			}
		}
	}

	else if (state == WORKER_STATES::OPERATING) {
		if (!targeted_task.lock()->isCompleted()) {
			if (Vector2Distance(pos, targeted_task.lock()->pos) <= 0.5f) { // TODO fix distance parameter

				targeted_task.lock()->work_done += GetFrameTime();
				//rotateAroundPoint(pos, targeted_constructions.front()->pos, 0.5f * GetFrameTime());

			}
			else {
				pos = Vector2MoveTowards(pos, targeted_task.lock()->pos, SPEED_MOD * speed * GetFrameTime());
			}
		}
		else {
			targeted_task.lock()->current_workers--;
			state = WORKER_STATES::IDLE;
		}

	}
}