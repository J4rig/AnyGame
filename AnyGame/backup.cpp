//#include "any_game.hpp"
//
//#include <algorithm>
//#include <string>
//#include <ctime>
//#include <iostream>
//#include <limits>
//
//Storage::Storage(int id, Vector2 pos, int priority, int capacity, array<int, MAX_TYPE> limits, bool can_take) :
//	capacity(capacity), can_be(limits), can_take(can_take) {
//};
//
//int Storage::isStored() {
//	int result = 0;
//	for (int i = 0; i < MAX_TYPE; i++) {
//		result += is[i];
//	}
//	return result;
//}
//
//int Storage::aboutToBeStored() {
//	int result = 0;
//	for (int i = 0; i < MAX_TYPE; i++) {
//		result += will_be[i];
//	}
//	return result;
//}
//
//bool Storage::isFull() {
//	return isStored() >= capacity;
//}
//
//bool Storage::hasSpace() {
//	return aboutToBeStored() < capacity;
//}
//
//int Storage::spaceLeft() {
//	return capacity - aboutToBeStored();
//}
//
//
//
//Task::Task(int id, Vector2 pos, int priority, float work_to_do, int max_workers) :
//	work_to_do(work_to_do), max_workers(max_workers) {
//};
//
//
//bool Task::hasWorkers() {
//	return current_workers > 0;
//}
//
//bool Task::isFullyOccupied() {
//	return current_workers >= max_workers;
//}
//
//bool Task::isCompleted() {
//	return work_done >= work_to_do;
//}
//
//
//
//Construction::Construction(int id, Vector2 pos, weak_ptr<Storage> storage, weak_ptr<Task> task) :
//	id(id), pos(pos), storage(storage), task(task) {
//};
//
//
//
//Stockpile::Stockpile(int id, Vector2 pos, float r, weak_ptr <Construction> construction, weak_ptr <Storage> storage) :
//	id(id), pos(pos), construction(construction), storage(storage) {
//};
//
//
//
//void Stockpile::draw() {
//	if (construction.expired()) {
//		DrawRing(pos, r - 2, r, 0, 360, 0, DARKGRAY);
//
//		//TODO change, not to calculate all the time but only recalculate when capacity changes
//		float piece = 360.0f * (1.0f / (float)(*storage.lock()).capacity);
//		int drawn_pieces = 0;
//		for (int i = 0; i < MAX_TYPE; i++) {
//			DrawRing(pos, 0, r - 1, drawn_pieces * piece, ((*storage.lock()).is[i] + drawn_pieces) * piece, 0, type_color[i]);
//			drawn_pieces += (*storage.lock()).is[i];
//		}
//
//		DrawText((to_string((*storage.lock()).isStored()) + "/" + to_string((*storage.lock()).capacity)).c_str(), pos.x - 10, pos.y, 10, WHITE);
//		DrawText(("s: " + to_string(id)).c_str(), pos.x - 5, pos.y - r - 2, 15, RED);
//	}
//	else if (!(*(*construction.lock()).task.lock()).isCompleted()) {
//		DrawRing(pos, r - 2, r, 0, 360, 0, RED);
//		float piece = 360.0f * (1.0f / (float)(*(*construction.lock()).storage.lock()).capacity);
//		int drawn_pieces = 0;
//		for (int i = 0; i < MAX_TYPE; i++) {
//			int pieces = (*(*construction.lock()).storage.lock()).is[i];
//			DrawRing(pos, 0, r - 1, drawn_pieces * piece, (piece + drawn_pieces) * piece, 0, type_color[i]);
//			drawn_pieces += pieces;
//		}
//	}
//	else {
//		DrawRing(pos, r - 2, r, 0, 360, 0, RED);
//	}
//}
//
//
//
//Resource::Resource(int id, Vector2 pos, int type) :
//	id(id), pos(pos), type(type) {
//};
//
//
//
//Generator::Generator(int id, Vector2 pos, float r, int type, int max, float time_to_generate, float dispense_radius) :
//	id(id), pos(pos), r(r), type(type), max(max), remaining(max), operated(false), occupied(false),
//	time_operated(0.0f), time_to_generate(3.0f), dispense_radius(dispense_radius), just_generated(false) {
//};
//
//
//
//void Generator::update(vector<Resource*>& resources) {
//	if (operated/*&& isEmpty()*/) { // maybe redundant since Worker will only operate if is not empty
//		if (time_operated >= time_to_generate) {
//			Vector2 r_pos = { (pos.x - dispense_radius) + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (2 * dispense_radius)),
//				(pos.y - r) + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (2 * dispense_radius)) };
//			resources.emplace_back(new Resource(resource_id++, r_pos, type));
//			remaining--;
//			time_operated = 0.0f;
//			just_generated = true;
//		}
//		time_operated += GetFrameTime();
//	}
//}
//
//
//
//void Generator::draw() {
//	DrawRing(pos, 10, r, 0, 360, 0, DARKGRAY);
//	DrawRing(pos, 10, r, 0, (float)remaining / (float)max * 360.0f, 0, type_color[type]);
//	DrawText(("g: " + to_string(id)).c_str(), pos.x - 5, pos.y - r - 2, 15, RED);
//	//DrawRectangleLines(pos.x - dispense_radius, pos.y - dispense_radius, 2 * dispense_radius, 2 * dispense_radius, RED);
//}
//
//
//
//bool Generator::isEmpty() {
//	return remaining <= 0;
//}
//
//
//
//Worker::Worker(int id, Vector2 pos, float speed) :
//	id(id), pos(pos), r(10.0f), speed(speed), state(WORKER_STATES::IDLE), capacity(WORKER_CAPACITY), collected_types(vector<int>()),
//	types_to_deliver(vector<int>()), targeted_resources(queue<Resource*>()), targeted_stockpiles(queue<Stockpile*>()),
//	targeted_generator(nullptr), targeted_constructions(queue<Construction*>()) {
//};
//
//
//
//void Worker::draw() {
//	DrawRing(pos, 4 /*resource radius -1 so it overlaps and hides imperfections*/, r, 0, 360, 0, DARKGRAY);
//
//	if (!collected_types.empty()) {
//		//TODO change, not to calculate all the time but only recalculate when capacity changes
//		float piece = 360.0f * (1.0f / (float)capacity);
//
//		for (int i = 0; i < collected_types.size(); i++) {
//			//DrawCircleV(pos, 5, type_color[collected_types[i]]);
//			DrawRing(pos, 0, 5/*Resource radius*/, i * piece, (i + 1) * piece, 1, type_color[collected_types[i]]);
//		}
//	}
//	DrawText(("w: " + to_string(id)).c_str(), pos.x - 5, pos.y - r - 2, 15, RED);
//}
//
//
//
//bool Worker::isFull() {
//	return collected_types.size() >= capacity;
//}
//
//
//
//bool Worker::isPacked() {
//	return types_to_deliver.size() >= capacity;
//}
//
//
//
//// returns closest resource
//Resource* findClosestResource(Vector2 point, vector<Resource*> resources, int type) { //-1 if type is not important or number of type
//	Resource* result = nullptr;
//	float min_dst = -1.0f;
//	for (Resource* r : resources) {
//		if (!r->occupied && (min_dst == -1.0f || Vector2Distance(point, r->pos) < min_dst) && (type == -1 || type == r->type)) {
//			result = r;
//			min_dst = Vector2Distance(point, r->pos);
//		}
//	}
//	return result;
//}
//
//
//
//// returns closest non-full stockpile with same type or neutral type
//// mode defines if we take from or deliver to stockpile or if we are just looking for any stockpile: <0 any stockpile, 0 - deliver, >0 - take
//// types is an array containing amounts of types required by construction
//weak_ptr<Storage> findStorage(Vector2 point, priority_queue<shared_ptr<Storage>> storages, int mode,
//	array<int, MAX_TYPE>& return_types, array<int, MAX_TYPE> wanted_types) {
//
//	float min_distance = numeric_limits<float>::max();
//	float new_distance;
//
//	bool found = false;
//
//	weak_ptr<Storage> result = weak_ptr<Storage>();
//
//	priority_queue<shared_ptr<Storage>> s = storages;
//	while (!s.empty()) {
//		array<int, MAX_TYPE> found_types = hasTypes(s.top()->will_be, wanted_types);
//		if (mode < 0) {
//			found = true;
//		}
//		else if (mode == 0) {
//			if (s.top()->hasSpace() && (result.expired() || (*result.lock()).priority <= s.top()->priority)) {
//				found = true;
//			}
//		}
//		else { // mode > 0 
//			if (resourceCount(found_types) > 0 && s.top()->can_take) {
//			}
//		}
//
//		if (found && (new_distance = min(Vector2Distance(point, s.top()->pos), min_distance)) != min_distance) {
//			min_distance = new_distance;
//			result = s.top();
//			if (mode > 0) {
//				return_types = found_types;
//			}
//		}
//		s.pop();
//	}
//
//	return result;
//}
//
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
//
//
//
//Generator* findClosestGenerator(Vector2 point, vector<Generator*> generators, vector<Worker*> workers) {
//	float min_distance = -1.0f;
//	Generator* result = nullptr;
//	for (Generator* g : generators) {
//		if (!g->occupied && !g->isEmpty() && (min_distance == -1.0f || Vector2Distance(point, g->pos) < min_distance)) {
//			bool optimal = true;
//			for (Worker* w : workers) {
//				if (w->state == WORKER_STATES::IDLE && Vector2Distance(w->pos, g->pos) < Vector2Distance(point, g->pos)) {
//					optimal = false;
//					break;
//				}
//			}
//			if (optimal) {
//				min_distance = Vector2Distance(point, g->pos);
//				result = g;
//			}
//		}
//	}
//	return result;
//}
//
//
//// returns closest construction that can have some stored resources delivered or nullptr,
//// also returns closest stockpile that contains some of the resources that are needed and array of all needed resources
//Construction* findClosestConstruction(Vector2 point, vector<Construction*> constructions, vector<Stockpile*> stockpiles,
//	Stockpile*& new_targeted_stockpile, array<int, MAX_TYPE>& types) {
//
//	float min_distance = -1.0f;
//	Construction* result = nullptr;
//	for (Construction* c : constructions) {
//		if (!c->isAllDelivered() && (min_distance == -1.0f || Vector2Distance(point, c->pos) < min_distance) &&
//			(new_targeted_stockpile = findClosestStockpile(point, stockpiles, 1, types, c->about_to_be_resources)) != nullptr) {
//
//			min_distance = Vector2Distance(point, c->pos) < min_distance;
//			result = c;
//		}
//	}
//
//	if (result != nullptr) {
//
//		//// debug
//		//printf("Sent in types :\n\t");
//		//for (int i = 0; i < MAX_TYPE; i++) {
//		//	printf("%i, ", result->about_to_be_resources[i]);
//		//}
//		//printf("\nGot types:\n\t");
//		//for (int i = 0; i < MAX_TYPE; i++) {
//		//	printf("%i, ",types[i]);
//		//}
//		////
//	}
//
//
//	return result;
//}
//
//
//Construction* findClosestConstructionToConstruct(Vector2 point, vector<Construction*> constructions) {
//	float min_distance = -1.0f;
//	Construction* result = nullptr;
//	for (Construction* c : constructions) {
//		if (c->isAllDelivered() && !c->isFullyOccupied() && !c->isCompleted() && (min_distance == -1.0f || Vector2Distance(point, c->pos) < min_distance)) {
//
//			min_distance = Vector2Distance(point, c->pos) < min_distance;
//			result = c;
//		}
//	}
//	return result;
//}
//
//
//
//// rotates point position around center point by specified angle
//Vector2 rotateAroundPoint(Vector2 point, Vector2 center, float angleInRads) {
//	float s = sin(angleInRads);
//	float c = cos(angleInRads);
//
//	point = Vector2Subtract(point, center);
//
//	Vector2 new_point = { point.x * c - point.y * s, point.x * s + point.y * c };
//
//	return Vector2Add(new_point, center);
//}
//
//
//
//// deletes resource from vector of resources
//bool deleteResource(Resource* resource, vector<Resource*>& resources) {
//	int i = 0;
//	for (Resource* r : resources) {
//		if (r == resource) {
//			resources.erase(resources.begin() + i);
//			return true;
//		}
//		i++;
//	}
//	return false;
//}
//
//
//
//array<int, MAX_TYPE> hasTypes(array<int, MAX_TYPE> stored_types, array<int, MAX_TYPE> types) {
//	array<int, MAX_TYPE> result = { 0 };
//	for (int i = 0; i < MAX_TYPE; i++) {
//		if (types[i] > 0 && stored_types[i] > 0) {
//			result[i] = min(types[i], stored_types[i]);
//		}
//	}
//	return result;
//}
//
//
//
//bool hasType(array<int, MAX_TYPE> stored_types, int type) {
//	return stored_types[type] > 0;
//}
//
//
//
//array<int, MAX_TYPE> arangeTypes(vector<int> types) {
//	array<int, MAX_TYPE> result = { 0 };
//	for (int i : types) {
//		result[i]++;
//	}
//	return result;
//}
//
//
//
//int resourceCount(array<int, MAX_TYPE> stored_types) {
//	int result = 0;
//	for (int i = 0; i < MAX_TYPE; i++) {
//		result += stored_types[i];
//	}
//	return result;
//}
//
//
//
//queue<int> cutToCapacity(array<int, MAX_TYPE> stored_types, int capacity) {
//	queue<int> result;
//	for (int i = 0; i < MAX_TYPE; i++) {
//		for (int j = 0; j < stored_types[i]; j++) {
//			if (result.size() < capacity) {
//				result.push(i);
//			}
//			else break;
//		}
//	}
//	return result;
//}
//
//
//
//// if possible, sets all the necessary data for valid resource collection route
//bool Worker::collectResources(vector<Resource*> resources, vector<Stockpile*> stockpiles, vector<Worker*> workers) {
//	int found_resources = 0;
//
//	Vector2 lastPos = {};
//
//	Resource* new_targeted_resource = nullptr;
//
//	while (found_resources < capacity) {
//		if ((new_targeted_resource = findClosestResource(pos, resources, -1)) != nullptr) {
//			new_targeted_resource->occupied = true;
//			targeted_resources.push(new_targeted_resource);
//			lastPos = new_targeted_resource->pos;
//			found_resources++;
//			new_targeted_resource = nullptr;
//		}
//		else {
//			break;
//		}
//	}
//
//	queue<Resource*> tmp = {};
//	while (!targeted_resources.empty()) {
//		tmp.push(targeted_resources.front());
//		targeted_resources.pop();
//	}
//
//	Stockpile* new_targeted_stockpile = nullptr;
//
//	bool first = true;
//
//	while (!tmp.empty()) {
//		array<int, MAX_TYPE> types = { 0 }; // useless
//		if ((new_targeted_stockpile = findClosestStockpile(lastPos, stockpiles, 0, types)) != nullptr) {
//			while (!tmp.empty() && new_targeted_stockpile->hasSpace()) {
//				new_targeted_stockpile->about_to_be_stored++;
//				targeted_resources.push(tmp.front());
//				tmp.pop();
//			}
//
//			if (first) {
//				while (!targeted_stockpiles.empty()) {
//					targeted_stockpiles.pop();
//				}
//				first = false;
//			}
//
//			if (targeted_stockpiles.empty() || new_targeted_stockpile != targeted_stockpiles.front()) {
//				targeted_stockpiles.push(new_targeted_stockpile);
//			}
//			new_targeted_stockpile = nullptr;
//
//		}
//		else {
//			tmp.front()->occupied = false;
//			found_resources--;
//			tmp.pop();
//		}
//	}
//
//	return found_resources > 0;
//}
//
//
//// if possible, sets all the necessary data for valid resource delivery route to a construction
//bool Worker::deliverResourcesToConstructions(vector<Construction*> constructions, vector<Stockpile*> stockpiles) {
//
//	Construction* new_targeted_construction = nullptr;
//	Stockpile* new_targeted_stockpile = nullptr;
//	array<int, MAX_TYPE> types = { 0 };
//
//	bool first = true;
//
//	while (!isPacked()) {
//
//		if ((new_targeted_construction = findClosestConstruction(pos, constructions, stockpiles, new_targeted_stockpile, types)) != nullptr &&
//			new_targeted_stockpile != nullptr) {
//
//			queue<int> types_to_pickup = cutToCapacity(types, capacity - types_to_deliver.size());
//
//			////debug
//			//cout << "\t\tAmount to deliver: " << types_to_pickup.size() << "\n";
//			////
//
//			if (targeted_constructions.empty() || targeted_constructions.front() != new_targeted_construction) {
//				targeted_constructions.push(new_targeted_construction);
//				amount_to_deliver.push(types_to_pickup.size());
//			}
//			else {
//				amount_to_deliver.front() += types_to_pickup.size();
//			}
//
//			if (first) {
//				while (!targeted_stockpiles.empty()) {
//					targeted_stockpiles.pop();
//				}
//				first = false;
//			}
//
//			if (targeted_stockpiles.empty() || targeted_stockpiles.front() != new_targeted_stockpile) {
//				targeted_stockpiles.push(new_targeted_stockpile);
//				amount_to_take.push(types_to_pickup.size());
//
//			}
//			else {
//				amount_to_take.front() += types_to_pickup.size();
//			}
//
//			//// debug
//			//printf("Found stockpile %i\n", new_targeted_stockpile->id);
//			////
//
//			while (!types_to_pickup.empty()) {
//				types_to_deliver.emplace_back(types_to_pickup.front());
//				new_targeted_construction->about_to_be_resources[types_to_pickup.front()]--;
//				new_targeted_stockpile->stored_types->will_be[types_to_pickup.front()]--;
//				types_to_pickup.pop();
//			}
//
//		}
//		else {
//			break;
//		}
//
//		new_targeted_construction = nullptr;
//		new_targeted_stockpile = nullptr;
//	}
//
//	return !types_to_deliver.empty();
//}
//
//
//
//bool Worker::workOnConstruction(vector<Construction*> constructions) {
//	Construction* new_targeted_construction;
//	if ((new_targeted_construction = findClosestConstructionToConstruct(pos, constructions)) != nullptr) {
//		new_targeted_construction->current_workers++;
//		targeted_constructions.push(new_targeted_construction);
//
//	}
//	return !targeted_constructions.empty();
//}
//
//
//
//void Worker::update(vector<shared_ptr<Resource>> resources,
//	priority_queue<shared_ptr<Storage>, vector<shared_ptr<Storage>>, decltype(storage_cmp)> storages,
//	priority_queue<shared_ptr<Task>, vector<shared_ptr<Task>>, decltype(task_cmp)> tasks
//) {
//
//	if (state == WORKER_STATES::IDLE) {
//		if (!targeted_stockpiles.empty()) {
//			if (workOnConstruction(constructions)) {
//				state = WORKER_STATES::CONSTRUCTING;
//			}
//			else if (deliverResourcesToConstructions(constructions, stockpiles)) {
//
//				////debug
//				//printf("w%i delivery plan:\n",id);
//				//
//				//queue<Construction*> tmp_con = targeted_constructions;
//				//printf("\tTargeted constructions:\n");
//				//while (!tmp_con.empty()) {
//				//	printf("\t%i, ", tmp_con.front()->id);
//				//	tmp_con.pop();
//				//}
//				//queue<Stockpile*> tmp_sto = targeted_stockpiles;
//				//queue<int> tmp_amo = amount_to_take;
//				//printf("\n\tTargeted stockpiles:\n");
//				//while (!tmp_sto.empty() && !tmp_amo.empty()) {
//				//	printf("\t%i : %i, ", tmp_sto.front()->id, tmp_amo.front());
//				//	tmp_amo.pop();
//				//	tmp_sto.pop();
//				//}
//				//printf("\n\tTypes to deliver :\n\t");
//				//for (int type : types_to_deliver) {
//				//	printf("%i, ", type);
//				//}
//				//printf("\n\tAmount to deliver:\n\t");
//				//queue<int> tmp_dlv = amount_to_deliver;
//				//while (!tmp_dlv.empty()) {
//				//	printf("%i, ", amount_to_deliver.front());
//				//	tmp_dlv.pop();
//				//}
//				//printf("\n");
//				////
//
//				state = WORKER_STATES::TRANSPORTING;
//			}
//
//			else if (collectResources(resources, stockpiles, workers)) {
//				state = WORKER_STATES::COLLECTING;
//			}
//
//			else if ((targeted_generator = findClosestGenerator(pos, generators, workers)) != nullptr) {
//				targeted_generator->occupied = true;
//				state = WORKER_STATES::GENERATING;
//			}
//
//			else if (Vector2Distance(pos, targeted_stockpiles.front()->pos) <= targeted_stockpiles.front()->r + r) {
//				pos = rotateAroundPoint(pos, targeted_stockpiles.front()->pos, 0.5f * GetFrameTime());
//			}
//
//			else {
//				pos = Vector2MoveTowards(pos, targeted_stockpiles.front()->pos, SPEED_MOD * speed * GetFrameTime());
//			}
//
//		}
//		else {
//			array<int, MAX_TYPE> type = { 0 }; //useless
//			Stockpile* tmp = findClosestStockpile(pos, stockpiles, -1, type);
//			if (tmp != nullptr) {
//				targeted_stockpiles.push(tmp);
//			}
//		}
//	}
//
//	else if (state == WORKER_STATES::COLLECTING) {
//		if (!targeted_resources.empty()) {
//			if (Vector2Distance(pos, targeted_resources.front()->pos) <= 0.5f) {
//				deleteResource(targeted_resources.front(), resources);
//				collected_types.emplace_back(targeted_resources.front()->type);
//				targeted_resources.pop();
//			}
//			else
//			{
//				pos = Vector2MoveTowards(pos, targeted_resources.front()->pos, SPEED_MOD * speed * GetFrameTime());
//			}
//		}
//		else {
//			if (Vector2Distance(pos, targeted_stockpiles.front()->pos) <= targeted_stockpiles.front()->r + r) { // crashes here
//
//				while (!collected_types.empty() && !targeted_stockpiles.front()->isFull()) {
//					targeted_stockpiles.front()->stored_types->is[collected_types.back()]++;
//					targeted_stockpiles.front()->stored_types->will_be[collected_types.back()]++;
//					targeted_stockpiles.front()->currently_stored++;
//					collected_types.pop_back();
//				}
//
//				if (targeted_stockpiles.size() > 1) {
//					targeted_stockpiles.pop();
//				}
//				else {
//					state = WORKER_STATES::IDLE;
//				}
//
//			}
//			else {
//				pos = Vector2MoveTowards(pos, targeted_stockpiles.front()->pos, SPEED_MOD * speed * GetFrameTime());
//			}
//		}
//	}
//
//	else if (state == WORKER_STATES::GENERATING) {
//		if (Vector2Distance(pos, targeted_generator->pos) <= 0.5f) {
//			pos = targeted_generator->pos;
//			targeted_generator->operated = true;
//			if (targeted_generator->just_generated) {
//				targeted_generator->just_generated = false;
//				targeted_generator->operated = false;
//				targeted_generator->time_operated = 0.0f;
//				targeted_generator->occupied = false;
//				state = WORKER_STATES::IDLE;
//			}
//		}
//		else {
//			pos = Vector2MoveTowards(pos, targeted_generator->pos, speed * SPEED_MOD * GetFrameTime());
//		}
//	}
//
//	else if (state == WORKER_STATES::TRANSPORTING) {
//		if (!types_to_deliver.empty()) {
//			if (Vector2Distance(pos, targeted_stockpiles.front()->pos) <= targeted_stockpiles.front()->r + r) {
//
//				//changes to stockpile
//				targeted_stockpiles.front()->about_to_be_stored--;
//				targeted_stockpiles.front()->currently_stored--;
//				targeted_stockpiles.front()->stored_types->is[types_to_deliver.front()]--;
//
//				//changes to worker
//				collected_types.emplace_back(types_to_deliver.front());
//				types_to_deliver.erase(types_to_deliver.begin());
//				amount_to_take.front()--;
//				if (amount_to_take.front() <= 0) {
//					targeted_stockpiles.pop();
//					amount_to_take.pop();
//				}
//			}
//			else {
//				pos = Vector2MoveTowards(pos, targeted_stockpiles.front()->pos, SPEED_MOD * speed * GetFrameTime());
//			}
//		}
//		else {
//			if (Vector2Distance(pos, targeted_constructions.front()->pos) <= 5.0f) { // TODO fix distance parameter
//
//				while (amount_to_deliver.front() > 0) {
//					targeted_constructions.front()->resources[collected_types.front()]--;
//					collected_types.erase(collected_types.begin());
//					amount_to_deliver.front()--;
//				}
//
//				amount_to_deliver.pop();
//				targeted_constructions.pop();
//
//				if (collected_types.empty()) {
//
//					state = WORKER_STATES::IDLE;
//				}
//
//			}
//			else {
//				pos = Vector2MoveTowards(pos, targeted_constructions.front()->pos, SPEED_MOD * speed * GetFrameTime());
//			}
//		}
//	}
//	else if (state == WORKER_STATES::CONSTRUCTING) {
//		if (!targeted_constructions.front()->isCompleted()) {
//			if (Vector2Distance(pos, targeted_constructions.front()->pos) <= 5.0f) { // TODO fix distance parameter
//
//				targeted_constructions.front()->work_done += GetFrameTime();
//
//				//rotateAroundPoint(pos, targeted_constructions.front()->pos, 0.5f * GetFrameTime());
//
//			}
//			else {
//				pos = Vector2MoveTowards(pos, targeted_constructions.front()->pos, SPEED_MOD * speed * GetFrameTime());
//			}
//		}
//		else {
//			targeted_constructions.front()->current_workers--;
//			targeted_constructions.pop();
//			state = WORKER_STATES::IDLE;
//		}
//
//	}
//
//}
//
//
//
//int main() {
//
//	bool pause = false;
//
//	srand(time(nullptr)); // time based seed for RNG
//
//	priority_queue<shared_ptr<Storage>, vector<shared_ptr<Storage>>, decltype(storage_cmp)> storages(storage_cmp);
//
//	priority_queue<shared_ptr<Task>, vector<shared_ptr<Task>>, decltype(task_cmp)> tasks(task_cmp);
//
//	vector<shared_ptr<Resource>> resources;
//
//	vector<Worker*> workers;
//
//	vector<Stockpile*> stockpiles;
//
//
//	InitWindow(1600, 800, "AnyGame");
//	SetTargetFPS(30);
//	while (!WindowShouldClose()) {
//
//		if (IsKeyReleased(KEY_SPACE)) {
//			pause = !pause;
//		}
//
//		if (IsKeyReleased(KEY_BACKSPACE)) {
//			workers.erase(workers.begin(), workers.begin() + workers.size());
//			resources.erase(resources.begin(), resources.begin() + resources.size());
//		}
//
//		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
//			Vector2 mouse_pos = GetMousePosition();
//			int collide = false;
//			for (shared_ptr<Resource> res : resources) {
//				if (CheckCollisionPointCircle(mouse_pos, res->pos, res->r * 2)) {
//					collide = true;
//					res->type = ++res->type % MAX_TYPE;
//				}
//			}
//			if (!collide) {
//				resources.emplace_back(new Resource(resource_id++, mouse_pos, rand() % MAX_TYPE));
//			}
//		}
//
//		if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
//			Vector2 mouse_pos = GetMousePosition();
//			workers.emplace_back(new Worker(worker_id++, mouse_pos, 1.0f));
//		}
//
//		if (IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE)) {
//			Vector2 mouse_pos = GetMousePosition();
//			array<int, MAX_TYPE> limits = { -1,-1,-1 };
//
//			shared_ptr<Storage> new_storage = make_shared<Storage>(storage_id++, mouse_pos, 1, 10, limits, true);
//
//			storages.emplace(new_storage);
//
//			stockpiles.emplace_back(new Stockpile(stockpile_id++, mouse_pos, 40.0f, weak_ptr<Construction>(), new_storage));
//		}
//
//		BeginDrawing();
//		ClearBackground(BLACK);
//
//
//
//		int i = 0;
//		for (Stockpile* s : stockpiles) {
//			s->draw();
//		}
//
//		for (shared_ptr<Resource> res : resources) {
//			DrawCircleV(res->pos, res->r, type_color[res->type]);
//			DrawText(("r: " + to_string(res->id)).c_str(), res->pos.x - 5, res->pos.y - res->r - 2, 15, RED);
//		}
//
//		i = 0;
//		for (Worker* w : workers) {
//			if (!pause) {
//				w->update(resources, storages, tasks);
//			}
//			w->draw();
//			i++;
//		}
//
//		EndDrawing();
//	}
//	CloseWindow();
//	return 0;
//}