#include "any_game.hpp"

#include <algorithm>
#include <string>
#include <ctime>
#include <iostream>
#include <limits>

Storage::Storage(int id, Vector2 pos, int priority, int capacity, array<int, MAX_TYPE> limits, bool can_take) :
	id(id), pos(pos), priority(priority), capacity(capacity), can_be(limits), can_take(can_take) {};

int Storage::isStored() {
	int result = 0;
	for (int i = 0; i < MAX_TYPE; i++) {
		result += is[i];
	}
	return result;
}

int Storage::aboutToBeStored() {
	int result = 0;
	for (int i = 0; i < MAX_TYPE; i++) {
		result += will_be[i];
	}
	return result;
}

bool Storage::isFull(int type) {
	if (type >= 0 && type < MAX_TYPE) {
		return isStored() >= capacity || is[type] >= can_be[type];
	}
	return isStored() >= capacity;
}

bool Storage::hasSpace(int type) {
	if (type >= 0 && type < MAX_TYPE) {
		return aboutToBeStored() < capacity && will_be[type] < can_be[type];
	}
	return aboutToBeStored() < capacity;
}

bool Storage::hasSpace(vector<int> types) {
	for (int type : types) {
		if (aboutToBeStored() < capacity && will_be[type] < can_be[type]) {
			return true;
		}
	}
	return false;
}

int Storage::spaceLeft(int type) {
	return min(capacity - aboutToBeStored(), can_be[type] - will_be[type]);
}



Task::Task(int id, Vector2 pos, int priority, float work_to_do, int max_workers) :
	id(id), pos(pos), priority(priority), work_to_do(work_to_do), max_workers(max_workers) {};


bool Task::hasWorkers() const {
	return current_workers > 0;
}

bool Task::isFullyOccupied() const {
	return current_workers >= max_workers;
}

bool Task::isCompleted() const {
	return work_done >= work_to_do;
}


Combat::Combat(int id, Vector2 pos, float r, int max_health, int defense, int damage) :
	id(id), pos(pos), r(r), max_health(max_health), health(max_health), defense(defense), damage(damage) { };

void Combat::attack(weak_ptr<Combat> &target) {
	target.lock()->health -= damage - target.lock()->defense;
}

bool Combat::canAttack() const {
	return attack_cooldown <= time_to_attack;
}

bool Combat::isDead() const {
	return health <= 0;
}


Raider::Raider(int id, Vector2 pos, weak_ptr<Combat> combat) :
	id(id), pos(pos), combat(combat) {};

void Raider::draw() const {
	float r = combat.lock()->r;
	float angle = ((float)combat.lock()->health / combat.lock()->max_health) * 360;
	DrawCircleLinesV(pos, 15, PURPLE);
	DrawRing(pos,0,r,0.0f,angle,0,RED);
}

void Raider::update(vector<shared_ptr<Combat>> targets) {
	if (target.expired()) {
		target = findTarget(pos, combat, targets);
	}
	else {
		if (Vector2Distance(pos, target.lock()->pos) < combat.lock()->r + target.lock()->r) {
			if (!combat.lock()->canAttack()) {
				combat.lock()->time_to_attack += GetFrameTime();
			}
			else {
				combat.lock()->attack(target);
				combat.lock()->time_to_attack = 0.0f;
			}
		}
		else {
			pos = Vector2MoveTowards(pos, target.lock()->pos, SPEED_MOD * 5 * GetFrameTime());
			combat.lock()->pos = pos;
			combat.lock()->time_to_attack = 0.0f;
		}
	}
}

Construction::Construction(int id, Vector2 pos, weak_ptr<Storage> storage, weak_ptr<Task> task) :
	id(id),pos(pos), storage(storage), task(task) {};



Stockpile::Stockpile(int id, Vector2 pos, float r, weak_ptr <Construction> construction, weak_ptr <Storage> storage) :
	id(id), pos(pos), r(r), construction(construction), storage(storage) {};



void Stockpile::draw() const {
	if (construction.expired()) {
		DrawRing(pos, r - 2, r, 0, 360, 0, DARKGRAY);

		//TODO change, not to calculate all the time but only recalculate when capacity changes
		float piece = 360.0f * (1.0f / (float)storage.lock()->capacity);
		int drawn_pieces = 0;
		for (int i = 0; i < MAX_TYPE; i++) {
			DrawRing(pos, 0, r - 1, drawn_pieces * piece, (storage.lock()->is[i] + drawn_pieces) * piece, 0, type_color[i]);
			drawn_pieces += storage.lock()->is[i];
		}

		DrawText((to_string(storage.lock()->isStored()) + "/" + to_string(storage.lock()->capacity)).c_str(), (int)pos.x - 10, (int)pos.y, 10, WHITE);
		//DrawText(("s: " + to_string(id)).c_str(),pos.x-5,pos.y-r-2,15,RED);
		//DrawText(("r: " + to_string(storage.lock()->id) + " p: " + to_string(storage.lock()->priority)).c_str(), pos.x + 5, pos.y + r - 2, 15, GREEN);
	}
	else if (!construction.lock()->storage.expired()) {
		DrawRing(pos, r - 2, r, 0, 360, 0, RED);
		float piece = 360.0f * (1.0f / (float)construction.lock()->storage.lock()->capacity);
		int drawn_pieces = 0;
		for (int i = 0; i < MAX_TYPE; i++) {		 
			int pieces = construction.lock()->storage.lock()->can_be[i] - construction.lock()->storage.lock()->is[i];
			DrawRing(pos, 0, r - 1, drawn_pieces * piece, (pieces + drawn_pieces) * piece, 0, type_color[i]);
			drawn_pieces += pieces;
		}	
	}
	else {
		DrawRing(pos, r - 2, r, 0, 360, 0, RED);
	}
}



Resource::Resource(int id, Vector2 pos, int type) :
	id(id), pos(pos), type(type) {};


void Resource::draw() const {
	DrawCircleV(pos, r, type_color[type]);
	//DrawText(("r: " + to_string(id)).c_str(), pos.x - 5, pos.y - r - 2, 15, RED);
}



Generator::Generator(int id, Vector2 pos, float r, int type, int max, float dispense_radius) :
	id(id), pos(pos), r(r), type(type), max(max), remaining(max), task(task), dispense_radius(dispense_radius) {};



void Generator::draw() const {
	DrawRing(pos, 10, r, 0, 360, 0, DARKGRAY);
	DrawRing(pos, 10, r, 0, (float)remaining / (float)max * 360.0f, 0, type_color[type]);
	//DrawText(("g: " + to_string(id)).c_str(), pos.x - 5, pos.y - r - 2, 15, RED);
	//DrawText(("t: " + to_string(task.lock()->id) + " p: " + to_string(task.lock()->priority)).c_str(), pos.x + 5, pos.y + r - 2, 15, GREEN);
	//DrawRectangleLines(pos.x - dispense_radius, pos.y - dispense_radius, 2 * dispense_radius, 2 * dispense_radius, RED);
}



bool Generator::isEmpty() const {
	return remaining <= 0;
}

Forge::Forge(int id, Vector2 pos, float r, weak_ptr<Storage> storage) :
	id(id), pos(pos), r(r), storage(storage), task(weak_ptr<Task>()) {};

void Forge::draw() const {
	DrawCircleLinesV(pos, r, type_color[type]);
	if (!task.expired()) {
		DrawRing(pos, r - 4, r, 0, 360 * (task.lock()->work_done / task.lock()->work_to_do), 0, type_color[type]);
	}

	float piece = 360.0f * (1.0f / (float)storage.lock()->capacity);
	int drawn_pieces = 0;
	DrawCircleLinesV(pos, r-5, type_color[type]);
	for (int i = 0; i < MAX_TYPE; i++) {
		DrawRing(pos, 10, r-5, drawn_pieces * piece, (storage.lock()->is[i] + drawn_pieces) * piece, 0, type_color[i]);
		drawn_pieces += storage.lock()->is[i];
	}
}



Worker::Worker(int id, Vector2 pos, float speed) :
	id(id), pos(pos), speed(speed) {};


void Worker::draw() {
	DrawRing(pos, 4 /*resource radius -1 so it overlaps and hides imperfections*/, r, 0, 360, 0, DARKGRAY);

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



//returns closest resource
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

	sort(found_storages.begin(), found_storages.end(), storage_cmp);

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


weak_ptr<Combat> findTarget(Vector2 point, weak_ptr<Combat> attacker, vector<shared_ptr<Combat>> targets) {
	float min_distance = numeric_limits<float>::max();
	float new_distance;

	weak_ptr<Combat> result = weak_ptr<Combat>();

	for (shared_ptr<Combat> t : targets) {
		if (attacker.lock() == t) {
			continue;
		}
		if ((new_distance = min(Vector2Distance(point, t->pos), min_distance)) != min_distance) {
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


void insertStorage(vector<shared_ptr<Storage>>& storages, shared_ptr<Storage> storage) {
	auto pos = lower_bound(storages.begin(), storages.end(), storage, storage_cmp);
	storages.insert(pos, storage);
}

void insertTask(vector<shared_ptr<Task>>& tasks, shared_ptr<Task> task) {
	auto pos = lower_bound(tasks.begin(), tasks.end(), task, task_cmp);
	tasks.insert(pos, task);
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
				pos = Vector2Add(targeted_storages.front().lock()->pos, {50.0f,0.0f});
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
			if (Vector2Distance(pos, targeted_storages.front().lock()->pos) <= 40+10) {

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
				else{ // waiting for resorce to be delivered
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



int main() {

	bool pause = false;

	srand((unsigned int)time(nullptr)); // time based seed for RNG

	vector<shared_ptr<Storage>> storages;

	vector<shared_ptr<Task>> tasks;

	vector<shared_ptr<Combat>> targets;


	vector<shared_ptr<Generator>> generators;

	vector<shared_ptr<Stockpile>> stockpiles;

	vector<shared_ptr<Resource>> resources;

	vector<shared_ptr<Worker>> workers;

	vector<shared_ptr<Construction>> constructions;

	vector<shared_ptr<Forge>> forges;

	vector<shared_ptr<Raider>> raiders;

	

	InitWindow(1600, 800, "AnyGame");
	SetTargetFPS(30);
	while (!WindowShouldClose()) {

		if (IsKeyReleased(KEY_SPACE)) {
			pause = !pause;
		}

		if (IsKeyReleased(KEY_BACKSPACE)) {
			raider_id = 0;
			construction_id = 0;
			stockpile_id = 0;
			resource_id = 0;
			worker_id = 0;
			generator_id = 0;
			forge_id = 0;

			storage_id = 0;
			task_id = 0;
			combat_id = 0;

			storages.erase(storages.begin(), storages.begin() + storages.size());
			tasks.erase(tasks.begin(), tasks.begin() + tasks.size());
			targets.erase(targets.begin(), targets.begin() + targets.size());

			resources.erase(resources.begin(), resources.begin() + resources.size());
			generators.erase(generators.begin(), generators.begin() + generators.size());
			stockpiles.erase(stockpiles.begin(), stockpiles.begin() + stockpiles.size());
			workers.erase(workers.begin(), workers.begin() + workers.size());
			forges.erase(forges.begin(), forges.begin() + forges.size());
			raiders.erase(raiders.begin(), raiders.begin() + raiders.size());


		}

		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
			Vector2 mouse_pos = GetMousePosition();
			int collide = false;
			for (shared_ptr<Resource> res : resources) {
				if (CheckCollisionPointCircle(mouse_pos, res->pos, res->r*2)) {
					collide = true;
					res->type = ++res->type % MAX_TYPE;
				}
			}
			if (!collide) {
				resources.emplace_back(make_shared<Resource>(resource_id++, mouse_pos, rand() % NATURAL_TYPE));
			}
		}

		if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
			Vector2 mouse_pos = GetMousePosition();
			workers.emplace_back(new Worker(worker_id++, mouse_pos, 1.0f));
		}

		if (IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE)) {
			Vector2 mouse_pos = GetMousePosition();
			array<int, MAX_TYPE> limits = { 0 };
			limits.fill(STOCKPILE_CAPACITY);

			shared_ptr<Storage> new_storage = make_shared<Storage>(storage_id++, mouse_pos, 0, STOCKPILE_CAPACITY, limits, true);
			insertStorage(storages, new_storage);
		
			stockpiles.emplace_back(new Stockpile(stockpile_id++, mouse_pos, 40.0f, weak_ptr<Construction>(), new_storage));
		}

		if (IsKeyReleased(KEY_G)) {
			Vector2 mouse_pos = GetMousePosition();
			generators.emplace_back(make_shared<Generator>(generator_id++, mouse_pos, 20, rand() % NATURAL_TYPE, 3, 30));
		}

		if (IsKeyReleased(KEY_C)) {
			Vector2 mouse_pos = GetMousePosition();

			std::array<int, MAX_TYPE> tmp = { 0 };
			tmp[0] = 1;
			tmp[1] = 1;
			tmp[2] = 1;
			shared_ptr<Storage> new_storage = make_shared<Storage>(storage_id++, mouse_pos, 2, 3, tmp, false);
			insertStorage(storages, new_storage);
			shared_ptr<Construction> c = make_shared<Construction>(construction_id++,mouse_pos, new_storage,weak_ptr<Task>());
			constructions.emplace_back(c);
			stockpiles.emplace_back(make_shared<Stockpile>(stockpile_id++, mouse_pos, 40.0f, c, weak_ptr<Storage>()));
		}

		if (IsKeyReleased(KEY_F)) {
			Vector2 mouse_pos = GetMousePosition();
			std::array<int, MAX_TYPE> limits = { 0 };
			limits[1] = 1;
			limits[2] = 1;
			shared_ptr<Storage> new_storage = make_shared<Storage>(stockpile_id++, mouse_pos, 1, 2, limits, true);
			insertStorage(storages, new_storage);
			shared_ptr<Forge> forge = make_shared<Forge>(forge_id++, mouse_pos, 40, new_storage);
			forges.emplace_back(forge);
		}

		if (IsKeyReleased(KEY_R)) {
			Vector2 mouse_pos = GetMousePosition();
			shared_ptr<Combat> new_target = make_shared<Combat>(combat_id++, mouse_pos, 15, 10, 2, 3);
			targets.emplace_back(new_target);
			shared_ptr<Raider> new_raider = make_shared<Raider>(raider_id++, mouse_pos, new_target);
			raiders.emplace_back(new_raider);
		}

		BeginDrawing();
		ClearBackground(BLACK);

		for (shared_ptr<Stockpile> s : stockpiles) {
			if (s->construction.expired() && s->storage.expired()) {
				array<int, MAX_TYPE> limits = { 0 };
				limits.fill(STOCKPILE_CAPACITY);

				shared_ptr<Storage> new_storage = make_shared<Storage>(storage_id++, s->pos, 0, 10, limits, true);
				insertStorage(storages, new_storage);
				s->storage = new_storage;
			}
			s->draw();
		}

		for (int g = 0; g < generators.size(); g++) {

			if (generators[g]->task.expired() && !generators[g]->isEmpty()) {
				shared_ptr<Task> new_task = make_shared<Task>(task_id++, generators[g]->pos, 0, 5.0f, 1);
				insertTask(tasks, new_task);
				generators[g]->task = new_task;
			}

			if (generators[g]->task.lock()->isCompleted()) {
				generators[g]->remaining--;
				resources.emplace_back(make_shared<Resource>(resource_id++,Vector2Add(generators[g]->pos, {(float)(rand() % 60 -30),(float)(rand() % 60 -30)} ), generators[g]->type));
				generators[g]->task.lock()->finished = true;
			}

			if (generators[g]->isEmpty()) {
				generators.erase(generators.begin() + g);
				g--;
			}
			else {
				generators[g]->draw();
			}
		}

		for (int res = 0; res < resources.size(); res++) {
			if (resources[res]->taken) {
				resources.erase(resources.begin() + res);
				res--;
			}
			else {
				resources[res]->draw();
			}
		}

		for (int f = 0; f < forges.size(); f++) {
			if (!forges[f]->task.expired() && forges[f]->task.lock()->isCompleted() && !forges[f]->task.lock()->finished) {
				forges[f]->storage.lock()->is = { 0 };
				forges[f]->storage.lock()->will_be = { 0 };
				resources.emplace_back(make_shared<Resource>(resource_id++, Vector2Add(forges[f]->pos, {(float)(rand() % 60 - 30),(float)(rand() % 60 - 30)}), 3));
				forges[f]->task.lock()->finished = true;
			}

			if ((!forges[f]->task.expired() && forges[f]->storage.lock()->hasSpace(-1))) {
				forges[f]->task.lock()->finished = true;
			}

			if (forges[f]->task.expired() && forges[f]->storage.lock()->isFull(-1)) {
				shared_ptr<Task> new_task = make_shared<Task>(task_id++, forges[f]->pos, 2, 3.0f, 1);
				insertTask(tasks, new_task);
				forges[f]->task = new_task;
			}
			forges[f]->draw();
		}

		for (int c = 0; c < constructions.size(); c++) {
			if (!constructions[c]->task.expired() && constructions[c]->task.lock()->isCompleted()) {	
				constructions[c]->task.lock()->finished = true;
				constructions.erase(constructions.begin() + c);		
				c--;
				continue;
			}
			
			if (!constructions[c]->storage.expired() && constructions[c]->storage.lock()->isFull(-1) && !constructions[c]->is_all_delivered) {
				constructions[c]->is_all_delivered = true;
				constructions[c]->storage.lock()->remove = true;
				shared_ptr<Task> new_task = make_shared<Task>(task_id++, constructions[c]->pos, 1, 5.0f, 2);
				insertTask(tasks, new_task);
				constructions[c]->task = new_task;
			}
		}

		for (int t = 0; t < targets.size(); t++) {
			if (!pause &&targets[t]->isDead()) {
				targets.erase(targets.begin() + t);
				t--;
				continue;
			}
		}

		for (int r = 0; r < raiders.size(); r++) {
			if (!pause) {
				if (raiders[r]->combat.expired()) {
					raiders.erase(raiders.begin() + r);
					r--;
					continue;
				}

				raiders[r]->update(targets);
			}
			raiders[r]->draw();
		}

		for (shared_ptr<Worker> w : workers) {
			if (!pause) {
				w->update(resources, storages, tasks);
			}
			w->draw();
		}

		for (int t = 0; t < tasks.size(); t++) {
			if (tasks[t]->finished && tasks[t]->hasWorkers() == 0) {
				tasks.erase(tasks.begin() + t);
				t--;
			}
		}

		for (int s = 0; s < storages.size(); s++) {
			if (storages[s]->remove) {
				storages.erase(storages.begin() + s);
				s--;
			}
		}

		EndDrawing();
	}
	CloseWindow();
	return 0;
}