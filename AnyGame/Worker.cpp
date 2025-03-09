#pragma once
#include "Worker.hpp"
#include "Storage.hpp"
#include "Task.hpp"

#include <iostream>

Worker::Worker(int z, int id, int tribe, Vector2 pos, weak_ptr<Target> target) :
	Drawing(z), id(id), tribe(tribe), pos(pos), target(target) {
};

void Worker::forgetStorage(shared_ptr<Storage> storage) {
	if (state == WORKER_STATES::IDLE) {
		targeted_storages = {};
		return;
	}

	if (state == WORKER_STATES::OPERATING) {
		targeted_storages = {};
		return;
	}

	if (state == WORKER_STATES::TRANSPORTING) { 
		for (int s = 0; s < targeted_storages.size(); s++) {
			if (targeted_storages[s].lock() == storage) {
				if ( 2* (collected_types.size() + s) < collected_types.size() + targeted_storages.size()) { //delete an output storage
					targeted_storages[s + types_to_deliver.size() + collected_types.size()].lock()->will_be[types_to_deliver[s]]--;
					targeted_storages.erase(targeted_storages.begin() + s + types_to_deliver.size() + collected_types.size());
					targeted_storages.erase(targeted_storages.begin() + s); 
					types_to_deliver.erase(types_to_deliver.begin() + s);
					s--;
				}
				else { //delete an imput storage
					int output_index = s - types_to_deliver.size() - collected_types.size();

					if (output_index < 0) {
						//shared_ptr<Resource> new_resource = make_shared<Resource>(resource_id++, pos, collected_types[collected_types.size() + output_index]);
						//resources.emplace_back(new_resource);
						collected_types.erase(collected_types.end() + output_index);
					}
					else {
						targeted_storages[output_index].lock()->will_be[types_to_deliver[output_index]]++;
						targeted_storages.erase(targeted_storages.begin() + output_index);
						types_to_deliver.erase(types_to_deliver.begin() + output_index);
						s--;
					}
					targeted_storages.erase(targeted_storages.begin() + s);
					s--;
					
				}
			}
		}		
	}
}

vector<int> Worker::die() {

	if (state == WORKER_STATES::TRANSPORTING) {
		while (!types_to_deliver.empty()) {
			targeted_storages.front().lock()->will_be[types_to_deliver.front()]++;
			types_to_deliver.erase(types_to_deliver.begin());
			targeted_storages.erase(targeted_storages.begin());
		}
		int i = 0;
		while (!targeted_storages.empty()) {
				targeted_storages.front().lock()->will_be[collected_types[i]]--;
				i++;
				targeted_storages.erase(targeted_storages.begin());
		}
		return collected_types;
	}

	else if (state == WORKER_STATES::OPERATING) {
		targeted_task.lock()->current_workers--;

	}
}


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



bool Worker::transportResources(vector<shared_ptr<Storage>> storages) {
	vector<weak_ptr<Storage>> deliver_vector = {};

	bool first = true;
	while (!isPacked()) {
		shared_ptr<Storage> deliver_to = findStorageToDeliverTo(pos, storages).lock(); //finds storage to deliver to
		if (deliver_to == nullptr) {
			break;
		}

		array<int, MAX_TYPE> to_types = { 0 }; // creates an array of types that can be delivered to found storage
		for (int i = 0; i < MAX_TYPE; i++) {
			to_types[i] += deliver_to->can_be[i] - deliver_to->will_be[i];
		}

		array<int, MAX_TYPE> from_types = { 0 };

		shared_ptr<Storage> take_from = findStorageToTakeFrom(deliver_to->pos, tribe, storages, from_types, to_types, deliver_to->priority).lock();

		if (take_from == nullptr || take_from == deliver_to) {
			break;
		}

		queue<int> types_to_pickup = cutToCapacity(from_types, capacity - (int)types_to_deliver.size());

		if (first) {
			targeted_storages = {};
			first = false;
		}

		while (!types_to_pickup.empty()) {
			types_to_deliver.emplace_back(types_to_pickup.front());

			targeted_storages.emplace_back(take_from);
			take_from->will_be[types_to_pickup.front()]--;

			deliver_vector.emplace_back(deliver_to);
			deliver_to->will_be[types_to_pickup.front()]++;
			
			types_to_pickup.pop();
		}
	}

	targeted_storages.insert(targeted_storages.end(),deliver_vector.begin(), deliver_vector.end());

	return !types_to_deliver.empty();
}



bool Worker::completeTask(vector<shared_ptr<Task>> tasks) {
	targeted_task = findTask(pos, tasks);
	return !targeted_task.expired();
}



void Worker::update( vector<shared_ptr<Storage>> storages, vector<shared_ptr<Task>> tasks) {

	WORKER_STATES original = state;
	if (state == WORKER_STATES::IDLE) {
		if (!tasks.empty() && completeTask(tasks)) {
			targeted_task.lock()->current_workers++;
			state = WORKER_STATES::OPERATING;
		}

		else if (!targeted_storages.empty() && !targeted_storages.front().expired()) {

			if (transportResources(storages)) {
				state = WORKER_STATES::TRANSPORTING;
			}

			//todo make prettier
			else if (Vector2Distance(pos, targeted_storages.front().lock()->pos) <= 1.0f) {
				pos = Vector2Add(targeted_storages.front().lock()->pos, { 50.0f,0.0f });
			}

			else if (Vector2Distance(pos, targeted_storages.front().lock()->pos) <= *(targeted_storages.front().lock()->r.lock()) + r) {

				pos = rotateAroundPoint(pos, targeted_storages.front().lock()->pos, 0.5f * GetFrameTime());
			}

			else {
				pos = Vector2MoveTowards(pos, targeted_storages.front().lock()->pos, SPEED_MOD * speed * GetFrameTime());
			}

		}
		else {
			if (!targeted_storages.empty() && targeted_storages.front().expired()) {
				targeted_storages.erase(targeted_storages.begin());
			}

			weak_ptr<Storage> tmp = findStorageToIdle(pos, storages);
			if (!tmp.expired()) {
				targeted_storages.emplace_back(tmp);
			}
		}
	}

	else if (state == WORKER_STATES::TRANSPORTING) {
		if (!types_to_deliver.empty()) {
			if (Vector2Distance(pos, targeted_storages.front().lock()->pos) <= *(targeted_storages.front().lock()->r.lock()) + r) {
				if (targeted_storages.front().lock()->is[types_to_deliver.front()] > 0) {
					targeted_storages.front().lock()->is[types_to_deliver.front()]--;

					collected_types.emplace_back(types_to_deliver.front());
					types_to_deliver.erase(types_to_deliver.begin());

					targeted_storages.erase(targeted_storages.begin());
				}
				else { // waiting for resorce to be delivered
					//pos = rotateAroundPoint(pos, targeted_storages.front().lock()->pos, 0.5f * GetFrameTime());
				}
			}
			else {
				pos = Vector2MoveTowards(pos, targeted_storages.front().lock()->pos, SPEED_MOD * speed * GetFrameTime());
			}
		}
		else if (!targeted_storages.empty()){
			if (Vector2Distance(pos, targeted_storages.front().lock()->pos) <= *targeted_storages.front().lock()->r.lock() + r) { // TODO fix distance parameter

				targeted_storages.front().lock()->is[collected_types.front()]++;
				collected_types.erase(collected_types.begin());

				targeted_storages.erase(targeted_storages.begin());

				if (collected_types.empty()) {
					state = WORKER_STATES::IDLE;
				}

			}
			else {
				pos = Vector2MoveTowards(pos, targeted_storages.front().lock()->pos, SPEED_MOD * speed * GetFrameTime());
			}
		}
		else {
			//state = WORKER_STATES::IDLE;
		}
	}

	else if (state == WORKER_STATES::OPERATING) {
		if (targeted_task.expired()) {
			state = WORKER_STATES::IDLE;
		}
		else if (!targeted_task.lock()->isCompleted()) {
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