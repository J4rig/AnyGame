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
	return isStored() >= capacity || is[type] >= can_be[type];
}

bool Storage::hasSpace(int type) {
	if (type >= 0 && type < MAX_TYPE) {
		return aboutToBeStored() < capacity && will_be[type] < can_be[type];
	}
	return aboutToBeStored() < capacity;
}

int Storage::spaceLeft(int type) {
	return min(capacity - aboutToBeStored(), can_be[type] - will_be[type]);
}



Task::Task(int id, Vector2 pos, int priority, float work_to_do, int max_workers) :
	id(id), pos(pos), priority(priority), work_to_do(work_to_do), max_workers(max_workers) {};


bool Task::hasWorkers() {
	return current_workers > 0;
}

bool Task::isFullyOccupied() {
	return current_workers >= max_workers;
}

bool Task::isCompleted() {
	return work_done >= work_to_do;
}



Construction::Construction(int id, Vector2 pos, weak_ptr<Storage> storage, weak_ptr<Task> task) :
	id(id),pos(pos), storage(storage), task(task) {};



Stockpile::Stockpile(int id, Vector2 pos, float r, weak_ptr <Construction> construction, weak_ptr <Storage> storage) :
	id(id), pos(pos), r(r), construction(construction), storage(storage) {};



void Stockpile::draw() {
	if (construction.expired()) {
		DrawRing(pos, r - 2, r, 0, 360, 0, DARKGRAY);

		//TODO change, not to calculate all the time but only recalculate when capacity changes
		float piece = 360.0f * (1.0f / (float)storage.lock()->capacity);
		int drawn_pieces = 0;
		for (int i = 0; i < MAX_TYPE; i++) {
			DrawRing(pos, 0, r - 1, drawn_pieces * piece, (storage.lock()->is[i] + drawn_pieces) * piece, 0, type_color[i]);
			drawn_pieces += storage.lock()->is[i];
		}

		DrawText((to_string(storage.lock()->isStored()) + "/" + to_string(storage.lock()->capacity)).c_str(), pos.x - 10, pos.y, 10, WHITE);
		DrawText(("s: " + to_string(id)).c_str(),pos.x-5,pos.y-r-2,15,RED);
		DrawText(("r: " + to_string(storage.lock()->id) + " p: " + to_string(storage.lock()->priority)).c_str(), pos.x + 5, pos.y + r - 2, 15, GREEN);
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
	/*else {
		DrawRing(pos, r - 2, r, 0, 360, 0, RED);
	}*/
}



Resource::Resource(int id, Vector2 pos, int type) :
	id(id), pos(pos), type(type) {};


void Resource::draw() {
	DrawCircleV(pos, r, type_color[type]);
	DrawText(("r: " + to_string(id)).c_str(), pos.x - 5, pos.y - r - 2, 15, RED);
}



Generator::Generator(int id, Vector2 pos, float r, int type, int max, weak_ptr<Task> task, float dispense_radius) :
	id(id), pos(pos), r(r), type(type), max(max), remaining(max), task(task), dispense_radius(dispense_radius), just_generated(false) {};



void Generator::draw() {
	DrawRing(pos, 10, r, 0, 360, 0, DARKGRAY);
	DrawRing(pos, 10, r, 0, (float)remaining / (float)max * 360.0f, 0, type_color[type]);
	DrawText(("g: " + to_string(id)).c_str(), pos.x - 5, pos.y - r - 2, 15, RED);
	DrawText(("t: " + to_string(task.lock()->id) + " p: " + to_string(task.lock()->priority)).c_str(), pos.x + 5, pos.y + r - 2, 15, GREEN);
	//DrawRectangleLines(pos.x - dispense_radius, pos.y - dispense_radius, 2 * dispense_radius, 2 * dispense_radius, RED);
}



bool Generator::isEmpty() {
	return remaining <= 0;
}



Worker::Worker(int id, Vector2 pos, float speed) :
	id(id), pos(pos), speed(speed), capacity(WORKER_CAPACITY), collected_types(vector<int>()), types_to_deliver(vector<int>()),
	targeted_resources(queue<weak_ptr<Resource>>()), targeted_storages(queue<weak_ptr<Storage>>()), targeted_task(weak_ptr<Task>()) {
};



void Worker::draw() {
	DrawRing(pos, 4 /*resource radius -1 so it overlaps and hides imperfections*/, r, 0, 360, 0, DARKGRAY);

	if (!collected_types.empty()) {
		//TODO change, not to calculate all the time but only recalculate when capacity changes
		float piece = 360.0f * (1.0f / (float)capacity);

		for (int i = 0; i < collected_types.size(); i++) {
			//DrawCircleV(pos, 5, type_color[collected_types[i]]);
			DrawRing(pos, 0, 5/*Resource radius*/, i * piece, (i + 1) * piece, 1, type_color[collected_types[i]]);
		}
	}
	DrawText(("w: " + to_string(id)).c_str(), pos.x - 5, pos.y - r - 2, 15, RED);
}



bool Worker::isFull() {
	return collected_types.size() >= capacity;
}



bool Worker::isPacked() {
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

array<int, MAX_TYPE> hasTypes(array<int, MAX_TYPE> stored_types, array<int, MAX_TYPE> types) {
	array<int, MAX_TYPE> result = { 0 };
	for (int i = 0; i < MAX_TYPE; i++) {
		if (types[i] > 0 && stored_types[i] > 0) {
			result[i] = min(types[i], stored_types[i]);
		}
	}
	return result;
}

weak_ptr<Storage> findStorageToIdle(Vector2 point, StorageQueue storages) {
	float min_distance = numeric_limits<float>::max();
	float new_distance;

	weak_ptr<Storage> result = weak_ptr<Storage>();

	StorageQueue s = storages;
	while (!s.empty()) {
		if ((new_distance = min(Vector2Distance(point, s.top()->pos), min_distance)) != min_distance) {
			min_distance = new_distance;
			result = s.top();
		}
		s.pop();
	}
	return result;
}

// returns closest non-full stockpile with same type or neutral type
// mode defines if we take from or deliver to stockpile or if we are just looking for any stockpile: <0 any stockpile, 0 - deliver, >0 - take
// types is an array containing amounts of types required by construction
weak_ptr<Storage> findStorageToStore(Vector2 point, StorageQueue storages, int type) {

	float min_distance = numeric_limits<float>::max();
	float new_distance;

	weak_ptr<Storage> result = weak_ptr<Storage>();

	StorageQueue s = storages;
	while (!s.empty()) {
		if (s.top()->hasSpace(type) && (result.expired() || result.lock()->priority <= s.top()->priority)) {
			if ((new_distance = min(Vector2Distance(point, s.top()->pos), min_distance)) != min_distance || result.lock()->priority < s.top()->priority) {
				min_distance = new_distance;
				result = s.top();
			}
		}
		s.pop();
	}
	
	return result;
}

weak_ptr<Storage> findStorageToTake(Vector2 point, StorageQueue storages, array<int, MAX_TYPE>& return_types, array<int, MAX_TYPE> wanted_types, int max_priority) {
	float min_distance = numeric_limits<float>::max();
	float new_distance;

	weak_ptr<Storage> result = weak_ptr<Storage>();

	StorageQueue s = storages;
	while (!s.empty()) {
		if (s.top()->priority >= max_priority && !s.top()->can_take) {
			s.pop();
			continue;
		}

		std::array<int, MAX_TYPE> tmp = hasTypes(s.top()->will_be, wanted_types);
		if (resourceCount(tmp) > 0 && (new_distance = min(Vector2Distance(point, s.top()->pos), min_distance)) != min_distance) {	
			return_types = tmp;
			min_distance = new_distance;
			result = s.top();
		}
		s.pop();
	}

	return result;
}

//weak_ptr<Task> findTask(Vector2 point, priority_queue<shared_ptr<Task>> tasks) {
//	float min_distance = numeric_limits<float>::max();
//	float new_distance;
//
//	weak_ptr<Task> result = weak_ptr<Task>();
//
//	priority_queue<shared_ptr<Task>> t = tasks;
//	while (!t.empty()) {
//		if (!t.top()->isFullyOccupied() && (result.expired() || (*result.lock()).priority <= t.top()->priority)) {
//			if ((new_distance = min(Vector2Distance(point, t.top()->pos), min_distance)) != min_distance) {
//				min_distance = new_distance;
//				result = t.top();
//			}
//		}
//
//		t.pop();
//	}
//	return result;
//}



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

array<int, MAX_TYPE> canBeStored(StorageQueue storages) {
	int amount = 0;
	array<int, MAX_TYPE> result = { 0 };
	while (!storages.empty()) {
		for (int i = 0; i < MAX_TYPE; i++) {
			if ((amount = storages.top()->spaceLeft(i)) > 0) {
				result[i] += amount;
			}
		}
		storages.pop();
	}
	return result;
}



bool Worker::collectResources(vector<shared_ptr<Resource>> resources, StorageQueue storages) {

	array<int, MAX_TYPE> can_be_stored = canBeStored(storages);
	/*for (int i = 0; i < MAX_TYPE; i++) {
		cout << can_be_stored[i] << " ";
	}
	cout << "\n";*/

	int found_resources = 0;

	Vector2 lastPos = {};

	shared_ptr<Resource> new_targeted_resource = nullptr;

	while (found_resources < capacity) {
		if ((new_targeted_resource = findClosestResource(pos, resources, can_be_stored)) != nullptr) {
			new_targeted_resource->occupied = true;
			targeted_resources.push(new_targeted_resource);
			lastPos = new_targeted_resource->pos;
			can_be_stored[new_targeted_resource->type]--;
			found_resources++;
			new_targeted_resource = nullptr;
		}
		else {
			break;
		}
	}

	queue<weak_ptr<Resource>> tmp_resources = queue<weak_ptr<Resource>>();
	while (!targeted_resources.empty()) {
		tmp_resources.push(targeted_resources.front().lock());
		targeted_resources.pop();
	}

	weak_ptr<Storage> new_targeted_stockpile = weak_ptr<Storage>();

	bool first = true;

	while (!tmp_resources.empty()) {
		new_targeted_stockpile = findStorageToStore(lastPos, storages, tmp_resources.front().lock()->type);
		if (!new_targeted_stockpile.expired()) {
			while (!tmp_resources.empty() && new_targeted_stockpile.lock()->hasSpace(tmp_resources.front().lock()->type)) {
				new_targeted_stockpile.lock()->will_be[tmp_resources.front().lock()->type]++;
				targeted_resources.push(tmp_resources.front());
				tmp_resources.pop();
			}

			if (first) {
				while (!targeted_storages.empty()) {
					targeted_storages.pop();
				}
				first = false;
			}

			if (targeted_storages.empty() || new_targeted_stockpile.lock() != targeted_storages.front().lock()) {
				targeted_storages.push(new_targeted_stockpile);
			}
			new_targeted_stockpile = weak_ptr<Storage>();

		}
		else {
			tmp_resources.front().lock()->occupied = false;
			found_resources--;
			tmp_resources.pop();
		}
	}

	return found_resources > 0;
}


bool Worker::transportResources(StorageQueue storages) {

	queue<weak_ptr<Storage>> deliver_queue = {};

	bool first = true;
	while (!isPacked()) {
		shared_ptr<Storage> deliver_to = findStorageToStore(pos, storages, -1).lock();
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

		queue<int> types_to_pickup = cutToCapacity(from_types, capacity - types_to_deliver.size());

		if (first) {
			targeted_storages = {};
			first = false;
		}

		if (targeted_storages.empty() || targeted_storages.front().lock()->id != take_from->id) {
			targeted_storages.push(take_from);
			amount_to_take.push(types_to_pickup.size());
		}
		else {
			amount_to_take.front() += types_to_pickup.size();
		}

		if (deliver_queue.empty() || deliver_queue.front().lock() != deliver_to) {
			deliver_queue.push(deliver_to);
			amount_to_deliver.push(types_to_pickup.size());
			
		}
		else {
			amount_to_deliver.front() += types_to_pickup.size();
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



void Worker::update(vector<shared_ptr<Resource>> resources,
	priority_queue<shared_ptr<Storage>, vector<shared_ptr<Storage>>, decltype(storage_cmp)> storages,
	priority_queue<shared_ptr<Task>, vector<shared_ptr<Task>>, decltype(task_cmp)> tasks
) {

	if (state == WORKER_STATES::IDLE) {
		if (!targeted_storages.empty()) {
			/*if (workOnConstruction(constructions)) {
				state = WORKER_STATES::CONSTRUCTING;
			}*/
			

			if (collectResources(resources, storages)) {
				state = WORKER_STATES::COLLECTING;	
			}

			else if (transportResources(storages)) {
				state = WORKER_STATES::TRANSPORTING;
			}

			/*else if ((targeted_generator = findClosestGenerator(pos, generators, workers)) != nullptr) {
				targeted_generator->occupied = true;
				state = WORKER_STATES::GENERATING;
			}*/
			
			else if (Vector2Distance(pos, targeted_storages.front().lock()->pos) <= 40 + 10) {
				pos = rotateAroundPoint(pos, targeted_storages.front().lock()->pos, 0.5f * GetFrameTime());
			}

			else {
				pos = Vector2MoveTowards(pos, targeted_storages.front().lock()->pos, SPEED_MOD * speed * GetFrameTime());
			}

		}
		else {
			weak_ptr<Storage> tmp = findStorageToIdle(pos, storages);
			if (!tmp.expired()) {
				targeted_storages.push(tmp);
			}
		}
	}

	else if (state == WORKER_STATES::COLLECTING) {
		if (!targeted_resources.empty()) {
			if (Vector2Distance(pos, targeted_resources.front().lock()->pos) <= 0.5f) {
				targeted_resources.front().lock()->taken = true;
				collected_types.emplace_back(targeted_resources.front().lock()->type);
				targeted_resources.pop();
			}
			else
			{
				pos = Vector2MoveTowards(pos, targeted_resources.front().lock()->pos, SPEED_MOD * speed * GetFrameTime());
			}
		}
		else {
			if (Vector2Distance(pos, targeted_storages.front().lock()->pos) <= 40+10) {
				
				while (!collected_types.empty() && !targeted_storages.front().lock()->isFull(collected_types.back())) {
					targeted_storages.front().lock()->is[collected_types.back()]++;
					//targeted_storages.front().lock()->will_be[collected_types.back()]++;
					collected_types.pop_back();
				}
				
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
				//changes to stockpile
				targeted_storages.front().lock()->is[types_to_deliver.front()]--;

				//changes to worker
				collected_types.emplace_back(types_to_deliver.front());
				types_to_deliver.erase(types_to_deliver.begin());
				amount_to_take.front()--;
				if (amount_to_take.front() <= 0) {
					targeted_storages.pop();
					amount_to_take.pop();
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

	//else if (state == WORKER_STATES::CONSTRUCTING) {
	//	if (!targeted_constructions.front()->isCompleted()) {
	//		if (Vector2Distance(pos, targeted_constructions.front()->pos) <= 5.0f) { // TODO fix distance parameter

	//			targeted_constructions.front()->work_done += GetFrameTime();

	//			//rotateAroundPoint(pos, targeted_constructions.front()->pos, 0.5f * GetFrameTime());

	//		}
	//		else {
	//			pos = Vector2MoveTowards(pos, targeted_constructions.front()->pos, SPEED_MOD * speed * GetFrameTime());
	//		}
	//	}
	//	else {
	//		targeted_constructions.front()->current_workers--;
	//		targeted_constructions.pop();
	//		state = WORKER_STATES::IDLE;
	//	}
	//	
	//}

}



int main() {

	bool pause = false;

	srand(time(nullptr)); // time based seed for RNG

	StorageQueue storages(storage_cmp);

	TaskQueue tasks(task_cmp);

	vector<shared_ptr<Generator>> generators;

	vector<shared_ptr<Stockpile>> stockpiles;

	vector<shared_ptr<Resource>> resources;

	vector<shared_ptr<Worker>> workers;

	vector<shared_ptr<Construction>> constructions;

	
	

	InitWindow(1600, 800, "AnyGame");
	SetTargetFPS(30);
	while (!WindowShouldClose()) {

		if (IsKeyReleased(KEY_SPACE)) {
			pause = !pause;
		}

		if (IsKeyReleased(KEY_BACKSPACE)) {
			construction_id = 0;
			stockpile_id = 0;
			resource_id = 0;
			worker_id = 0;
			generator_id = 0;
			storage_id = 0;
			task_id = 0;

			while (!storages.empty()) {
				storages.pop();
			}

			while (!tasks.empty()) {
				tasks.pop();
			}

			//workers.erase(workers.begin(), workers.begin() + workers.size());
			resources.erase(resources.begin(), resources.begin() + resources.size());
			generators.erase(generators.begin(), generators.begin() + generators.size());
			stockpiles.erase(stockpiles.begin(), stockpiles.begin() + stockpiles.size());
			workers.erase(workers.begin(), workers.begin() + workers.size());

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
				resources.emplace_back(make_shared<Resource>(resource_id++, mouse_pos, rand() % MAX_TYPE));
			}
		}

		if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
			Vector2 mouse_pos = GetMousePosition();
			workers.emplace_back(new Worker(worker_id++, mouse_pos, 1.0f));
		}

		if (IsKeyReleased(KEY_G)) {
			Vector2 mouse_pos = GetMousePosition();

			shared_ptr<Task> new_task = make_shared<Task>(task_id++, mouse_pos, rand() % 3, 5.0f,1);
			tasks.push(new_task);
			generators.emplace_back(make_shared<Generator>(generator_id++, mouse_pos, 20, rand() % 3, 3, new_task, 30));
		}

		if (IsKeyPressed(KEY_C)) {
			Vector2 mouse_pos = GetMousePosition();

			std::array<int, MAX_TYPE> tmp = { 1,1,1 };
			shared_ptr<Storage> s = make_shared<Storage>(storage_id++, mouse_pos, 2, 3, tmp, false);
			storages.push(s);
			shared_ptr<Construction> c = make_shared<Construction>(construction_id++,mouse_pos,s,weak_ptr<Task>());
			constructions.emplace_back(c);
			stockpiles.emplace_back(make_shared<Stockpile>(stockpile_id++, mouse_pos, 40, c, weak_ptr<Storage>()));
		}

		if (IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE)) {
			Vector2 mouse_pos = GetMousePosition();
			array<int, MAX_TYPE> limits = { 10, 10, 10 };
			
			shared_ptr<Storage> new_storage = make_shared<Storage>(storage_id++, mouse_pos, 1, 10, limits, true);
			storages.push(new_storage);

			stockpiles.emplace_back(new Stockpile(stockpile_id++, mouse_pos, 40.0f, weak_ptr<Construction>(),new_storage));
		}

		BeginDrawing();
		ClearBackground(BLACK);

		int i = 0;
		for (shared_ptr<Stockpile> s : stockpiles) {
			s->draw();
		}

		for (shared_ptr<Generator> g : generators) {
			g->draw();
		}

		/*StorageQueue sq = storages;
		while (!sq.empty()) {
			cout << sq.top()->id << ": ";
			for (int i = 0; i < MAX_TYPE; i++) {
				cout << sq.top()->is[i] << " ";
			}
			cout << "\n";
			sq.pop();
		}*/

		i = 0;
		for (int res = 0; res < resources.size(); res++) {
			if (resources[res]->taken) {
				resources.erase(resources.begin() + res);
				res--;
			}
			else {
				resources[res]->draw();
			}
		}

		i = 0;
		for (shared_ptr<Worker> w : workers) {
			if (!pause) {
				w->update(resources, storages, tasks);
			}
			w->draw();
			i++;
		}

		EndDrawing();
	}
	CloseWindow();
	return 0;
}