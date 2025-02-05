#include "any_game.hpp"

#include <algorithm>
#include <string>
#include <ctime>



Construction::Construction(int id, Vector2 pos, std::array<int, MAX_TYPE> resources, int number_of_resources, float work, int max_workers) :
	id(id),pos(pos),resources(resources), about_to_be_resources(resources), number_of_resources(number_of_resources),
	work(work), work_done(0.0f), max_workers(max_workers), current_workers(0) {};



// checks if all resources are delivered
bool Construction::isAllDelivered(){
	int cnt = 0;
	for (int i = 0; i < MAX_TYPE; i++) {
		cnt += resources[i];
	}
	return cnt <= 0;
}



// checks if all work is done
bool Construction::isCompleted() {
	return work_done >= work;
}



// checks if there is any worker working on construction
bool Construction::hasWorkers() {
	return current_workers > 0;
}



// checks if all worker spots are occupied
bool Construction::isFullyOccupied() {
	return current_workers >= max_workers;
}



Stockpile::Stockpile(int id, Vector2 pos, int capacity, Construction* construction) :
	id(id), pos(pos), r(40.0f), construction(construction), capacity(capacity), currently_stored(0), about_to_be_stored(0) {};



bool Stockpile::hasSpace() {
	return about_to_be_stored < capacity;
}



bool Stockpile::isFull() {
	return currently_stored >= capacity;
}



int Stockpile::spaceLeft() {
	return capacity - about_to_be_stored;
}



void Stockpile::draw() {
	if (construction == nullptr) {
		DrawRing(pos, r - 2, r, 0, 360, 0, DARKGRAY);

		//TODO change, not to calculate all the time but only recalculate when capacity changes
		float piece = 360.0f * (1.0f / (float)capacity);
		int drawn_pieces = 0;
		for (int i = 0; i < MAX_TYPE; i++) {
			DrawRing(pos, 0, r - 1, drawn_pieces * piece, (stored_types[i] + drawn_pieces) * piece, 0, type_color[i]);
			drawn_pieces += stored_types[i];
		}

		DrawText((std::to_string(currently_stored) + "/" + std::to_string(capacity)).c_str(), pos.x - 10, pos.y, 10, WHITE);
		DrawText(("s: " + std::to_string(id)).c_str(),pos.x-5,pos.y-r-2,15,RED);
	}
	else {
		DrawRing(pos, r - 2, r, 0, 360, 0, RED);
		float piece = 360.0f * (1.0f / (float)construction->number_of_resources);
		int drawn_pieces = 0;
		for (int i = 0; i < MAX_TYPE; i++) {		
			DrawRing(pos, 0, r - 1, drawn_pieces * piece, (construction->resources[i] + drawn_pieces) * piece, 0, type_color[i]);
			drawn_pieces += construction->resources[i];
		}	
	}
}



Resource::Resource(int id, Vector2 pos, int type) :
	id(id), pos(pos), r(5.0f), occupied(false), type(type) {};



Generator::Generator(int id, Vector2 pos, float r, int type, int max, float time_to_generate, float dispense_radius) :
	id(id), pos(pos), r(r), type(type), max(max), remaining(max), operated(false), occupied(false),
	time_operated(0.0f), time_to_generate(3.0f), dispense_radius(dispense_radius), just_generated(false) {};



void Generator::update(std::vector<Resource*> &resources) {
	if (operated/*&& isEmpty()*/) { // maybe redundant since Worker will only operate if is not empty
		if (time_operated >= time_to_generate) {
			Vector2 r_pos = { (pos.x - dispense_radius) + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (2 * dispense_radius)),
				(pos.y - r) + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (2 * dispense_radius)) };
			resources.emplace_back(new Resource(resource_id++,r_pos,type));
			remaining--;
			time_operated = 0.0f;
			just_generated = true;
		}
		time_operated += GetFrameTime();
	}
}



void Generator::draw() {
	DrawRing(pos, 10, r, 0, 360, 0, DARKGRAY);
	DrawRing(pos, 10, r, 0, (float)remaining / (float)max * 360.0f, 0, type_color[type]);
	DrawText(("g: " + std::to_string(id)).c_str(), pos.x - 5, pos.y - r - 2, 15, RED);
	//DrawRectangleLines(pos.x - dispense_radius, pos.y - dispense_radius, 2 * dispense_radius, 2 * dispense_radius, RED);
}



bool Generator::isEmpty() {
	return remaining <= 0;
}



Worker::Worker(int id, Vector2 pos, float speed) :
	id(id), pos(pos), r(10.0f), speed(speed), state(WORKER_STATES::IDLE), capacity(WORKER_CAPACITY), collected_types(std::vector<int>()),
	types_to_deliver(std::vector<int>()), targeted_resources(std::queue<Resource*>()), targeted_stockpiles(std::queue<Stockpile*>()),
	targeted_generator(nullptr), targeted_constructions(std::queue<Construction*>()) {
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
	DrawText(("w: " + std::to_string(id)).c_str(), pos.x - 5, pos.y - r - 2, 15, RED);
}



bool Worker::isFull() {
	return collected_types.size() >= capacity;
}



bool Worker::isPacked() {
	return types_to_deliver.size() >= capacity;
}



// returns closest resource
Resource* findClosestResource(Vector2 point, std::vector<Resource*> resources, int type) { //-1 if type is not important or number of type
	Resource* result = nullptr;
	float min_dst = -1.0f;
	for (Resource* r : resources) {
		if (!r->occupied && (min_dst == -1.0f || Vector2Distance(point, r->pos) < min_dst) && (type == -1 || type == r->type)) {
			result = r;
			min_dst = Vector2Distance(point, r->pos);
		}
	}
	return result;
}



// returns closest non-full stockpile with same type or neutral type
// mode defines if we take from or deliver to stockpile or if we are just looking for any stockpile: <0 any stockpile, 0 - deliver, >0 - take
// types is an array containing amounts of types required by construction
Stockpile* findClosestStockpile(Vector2 point, std::vector<Stockpile*> stockpiles, int mode,
	std::array<int, MAX_TYPE>& return_types, std::array<int, MAX_TYPE> types = { 0 }) {
	float min_distance = -1.0f;
	Stockpile* result = nullptr;

	for (Stockpile* s : stockpiles) {
		std::array<int, MAX_TYPE> tmp = hasTypes(s->stored_types, types);

		if (s->construction == nullptr && mode <= 0 && (s->hasSpace() || mode < 0) && // delivering to or looking for any stockpile
			(min_distance == -1.0f || Vector2Distance(point, s->pos) < min_distance)) {
			min_distance = Vector2Distance(point, s->pos);
			result = s;
		}
		else if (s->construction == nullptr && mode > 0 && resourceCount(tmp) > 0 &&
			(min_distance == -1.0f || Vector2Distance(point, s->pos) < min_distance)) { // taking from stockpile		
			return_types = tmp;
			min_distance = Vector2Distance(point, s->pos);
			result = s;
		}
	}
	if (mode > 0 && result != nullptr) {
		printf("Found stockpile: %i\n", result->id);
	}

	return result;
}



Generator* findClosestGenerator(Vector2 point, std::vector<Generator*> generators, std::vector<Worker*> workers) {
	float min_distance = -1.0f;
	Generator* result = nullptr;
	for (Generator* g : generators) {
		if (!g->occupied && !g->isEmpty() && (min_distance == -1.0f || Vector2Distance(point, g->pos) < min_distance)) {
			bool optimal = true;
			for (Worker* w : workers) {
				if (w->state == WORKER_STATES::IDLE && Vector2Distance(w->pos, g->pos) < Vector2Distance(point, g->pos)) {
					optimal = false;
					break;
				}
			}
			if (optimal) {
				min_distance = Vector2Distance(point, g->pos);
				result = g;
			}
		}
	}
	return result;
}


// returns closest construction that can have some stored resources delivered or nullptr,
// also returns closest stockpile that contains some of the resources that are needed and array of all needed resources
Construction* findClosestConstruction(Vector2 point, std::vector<Construction*> constructions, std::vector<Stockpile*> stockpiles,
	Stockpile*& new_targeted_stockpile, std::array<int, MAX_TYPE>& types) {

	float min_distance = -1.0f;
	Construction* result = nullptr;
	for (Construction* c : constructions) {
		if (!c->isAllDelivered() && (min_distance == -1.0f || Vector2Distance(point, c->pos) < min_distance) &&
			(new_targeted_stockpile = findClosestStockpile(point, stockpiles, 1, types, c->about_to_be_resources)) != nullptr) {
			
			min_distance = Vector2Distance(point, c->pos) < min_distance;
			result = c;
		}
	}

	if (result != nullptr) {
		printf("Sent in types :\n\t");
		for (int i = 0; i < MAX_TYPE; i++) {
			printf("%i, ", result->about_to_be_resources[i]);
		}
		printf("\nGot types:\n\t");
		for (int i = 0; i < MAX_TYPE; i++) {
			printf("%i, ",types[i]);
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



// deletes resource from vector of resources
bool deleteResource(Resource* resource, std::vector<Resource*>& resources) {
	int i = 0;
	for (Resource* r : resources) {
		if (r == resource) {
			resources.erase(resources.begin() + i);
			return true;
		}
		i++;
	}
	return false;
}



std::array<int, MAX_TYPE> hasTypes(std::array<int, MAX_TYPE> stored_types, std::array<int, MAX_TYPE> types) {
	std::array<int, MAX_TYPE> result = { 0 };
	for (int i = 0; i < MAX_TYPE; i++) {
		if (types[i] > 0 && stored_types[i] > 0) {
			result[i] = std::min(types[i], stored_types[i]);
		}
	}
	return result;
}



bool hasType(std::array<int, MAX_TYPE> stored_types, int type) {
	return stored_types[type] > 0;
}



std::array<int, MAX_TYPE> arangeTypes(std::vector<int> types) {
	std::array<int, MAX_TYPE> result = { 0 };
	for (int i : types) {
		result[i]++;
	}
	return result;
}



int resourceCount(std::array<int, MAX_TYPE> stored_types) {
	int result = 0;
	for (int i = 0; i < MAX_TYPE; i++) {
		result += stored_types[i];
	}
	return result;
}



std::queue<int> cutToCapacity(std::array<int, MAX_TYPE> stored_types, int capacity) {
	std::queue<int> result;
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



// if possible, sets all the necessary data for valid resource collection route
bool Worker::collectResources(std::vector<Resource*> resources, std::vector<Stockpile*> stockpiles, std::vector<Worker*> workers) {
	int found_resources = 0;

	Vector2 lastPos = {};

	Resource* new_targeted_resource = nullptr;

	while (found_resources < capacity) {
		if ((new_targeted_resource = findClosestResource(pos, resources, -1)) != nullptr) {
			new_targeted_resource->occupied = true;
			targeted_resources.push(new_targeted_resource);
			lastPos = new_targeted_resource->pos;
			found_resources++;
			new_targeted_resource = nullptr;
		}
		else {
			break;
		}	
	}

	std::queue<Resource*> tmp = {};
	while (!targeted_resources.empty()) {
		tmp.push(targeted_resources.front());
		targeted_resources.pop();
	}

	Stockpile* new_targeted_stockpile = nullptr;

	bool first = true;

	while (!tmp.empty()) {
		std::array<int, MAX_TYPE> types = { 0 }; // useless
		if ((new_targeted_stockpile = findClosestStockpile(lastPos, stockpiles, 0, types)) != nullptr) {
			while(!tmp.empty() && new_targeted_stockpile->hasSpace()) {
				new_targeted_stockpile->about_to_be_stored++;
				targeted_resources.push(tmp.front());
				tmp.pop();
			}

			if (first) {
				while (!targeted_stockpiles.empty()) {
					targeted_stockpiles.pop();
				}
				first = false;
			}

			if (targeted_stockpiles.empty() || new_targeted_stockpile != targeted_stockpiles.front()) {
				targeted_stockpiles.push(new_targeted_stockpile);
			}	
			new_targeted_stockpile = nullptr;
			
		}
		else {
			tmp.front()->occupied = false;
			found_resources--;
			tmp.pop();
		}
	}

	return found_resources > 0;
}


// if possible, sets all the necessary data for valid resource delivery route to a construction
bool Worker::deliverResourcesToConstructions(std::vector<Construction*> constructions, std::vector<Stockpile*> stockpiles) { 

	Construction* new_targeted_construction = nullptr;
	Stockpile* new_targeted_stockpile = nullptr;
	std::array<int, MAX_TYPE> types = { 0 };

	bool first = true;

	while (!isPacked()) {

		if ((new_targeted_construction = findClosestConstruction(pos, constructions, stockpiles, new_targeted_stockpile, types)) != nullptr &&
			new_targeted_stockpile  != nullptr) {

			std::queue<int> types_to_pickup = cutToCapacity(types, capacity - types_to_deliver.size());
			
			if (targeted_constructions.empty() || targeted_constructions.front() != new_targeted_construction) {
				targeted_constructions.push(new_targeted_construction);
			}

			if (first) {
				while (!targeted_stockpiles.empty()) {
					targeted_stockpiles.pop();
				}
				first = false;
			}

			targeted_stockpiles.push(new_targeted_stockpile);
			printf("Found stockpile %i\n", new_targeted_stockpile->id);
			amount_to_take.push(types_to_pickup.size());

			while (!types_to_pickup.empty()) {
				types_to_deliver.emplace_back(types_to_pickup.front());
				new_targeted_construction->about_to_be_resources[types_to_pickup.front()]--;
				new_targeted_stockpile->stored_types[types_to_pickup.front()]--;
				types_to_pickup.pop();
			}

		}
		else {
			break;
		}

		new_targeted_construction = nullptr;
		new_targeted_stockpile = nullptr;
	}

	return !types_to_deliver.empty();
}



void Worker::update(std::vector<Resource*> &resources, std::vector<Stockpile*> stockpiles, std::vector<Worker*> workers,
					std::vector<Generator*> generators, std::vector<Construction*> constructions) {

	if (state == WORKER_STATES::IDLE) {
		if (!targeted_stockpiles.empty()) {
			if (deliverResourcesToConstructions(constructions,stockpiles)) {

				//debug
				printf("w%i delivery plan:\n",id);
				
				std::queue<Construction*> tmp_con = targeted_constructions;
				printf("\tTargeted constructions:\n");
				while (!tmp_con.empty()) {
					printf("\t%i, ", tmp_con.front()->id);
					tmp_con.pop();
				}
				std::queue<Stockpile*> tmp_sto = targeted_stockpiles;
				std::queue<int> tmp_amo = amount_to_take;
				printf("\n\tTargeted stockpiles:\n");
				while (!tmp_sto.empty() && !tmp_amo.empty()) {
					printf("\t%i : %i, ", tmp_sto.front()->id, tmp_amo.front());
					tmp_amo.pop();
					tmp_sto.pop();
				}
				printf("\n");
				//

				state = WORKER_STATES::TRANSPORTING;
			}

			else if (collectResources(resources, stockpiles, workers)) {
				state = WORKER_STATES::COLLECTING;	
			}

			else if ((targeted_generator = findClosestGenerator(pos, generators, workers)) != nullptr) {
				targeted_generator->occupied = true;
				state = WORKER_STATES::GENERATING;
			}

			else if (Vector2Distance(pos, targeted_stockpiles.front()->pos) <= targeted_stockpiles.front()->r + r) {
				pos = rotateAroundPoint(pos, targeted_stockpiles.front()->pos, 0.5f * GetFrameTime());
			}

			else {
				pos = Vector2MoveTowards(pos, targeted_stockpiles.front()->pos, SPEED_MOD * speed * GetFrameTime());
			}

		}
		else {
			std::array<int, MAX_TYPE> type = { 0 }; //useless
			Stockpile* tmp = findClosestStockpile(pos, stockpiles, -1, type);
			if (tmp != nullptr) {
				targeted_stockpiles.push(tmp);
			}
		}
	}

	else if (state == WORKER_STATES::COLLECTING) {
		if (!targeted_resources.empty()) {
			if (Vector2Distance(pos, targeted_resources.front()->pos) <= 0.5f) {
				deleteResource(targeted_resources.front(), resources);
				collected_types.emplace_back(targeted_resources.front()->type);
				targeted_resources.pop();
			}
			else
			{
				pos = Vector2MoveTowards(pos, targeted_resources.front()->pos, SPEED_MOD * speed * GetFrameTime());
			}
		}
		else {
			if (Vector2Distance(pos, targeted_stockpiles.front()->pos) <= targeted_stockpiles.front()->r + r) { // crashes here
				
				while (!collected_types.empty() && !targeted_stockpiles.front()->isFull()) {
					targeted_stockpiles.front()->stored_types[collected_types.back()]++;
					targeted_stockpiles.front()->currently_stored++;
					collected_types.pop_back();
				}
				
				if (targeted_stockpiles.size() > 1) {
					targeted_stockpiles.pop();
				}
				else {
					state = WORKER_STATES::IDLE;
				}

			}
			else {
				pos = Vector2MoveTowards(pos, targeted_stockpiles.front()->pos, SPEED_MOD * speed * GetFrameTime());
			}
		}
	}

	else if (state == WORKER_STATES::GENERATING) {
		if (Vector2Distance(pos, targeted_generator->pos) <= 0.5f) {
			pos = targeted_generator->pos;
			targeted_generator->operated = true;
			if (targeted_generator->just_generated) {
				targeted_generator->just_generated = false;
				targeted_generator->operated = false;
				targeted_generator->time_operated = 0.0f;
				targeted_generator->occupied = false;
				state = WORKER_STATES::IDLE;
			}
		}
		else {
			pos = Vector2MoveTowards(pos, targeted_generator->pos, speed * SPEED_MOD * GetFrameTime());
		}
	}

	else if (state == WORKER_STATES::TRANSPORTING) {
		if (!types_to_deliver.empty()) {
			if (Vector2Distance(pos, targeted_stockpiles.front()->pos) <= targeted_stockpiles.front()->r + r) {
				
				//changes to stockpile
				targeted_stockpiles.front()->about_to_be_stored--;
				targeted_stockpiles.front()->currently_stored--;
				//targeted_stockpiles.front()->stored_types[types_to_deliver.front()]--;

				//changes to worker
				collected_types.emplace_back(types_to_deliver.front());
				types_to_deliver.erase(types_to_deliver.begin());
				amount_to_take.front()--;
				if (amount_to_take.front() <= 0) {
					targeted_stockpiles.pop();
					amount_to_take.pop();
				}
			}
			else {
				pos = Vector2MoveTowards(pos, targeted_stockpiles.front()->pos, SPEED_MOD * speed * GetFrameTime());
			}
		}
		else {
			if (Vector2Distance(pos, targeted_constructions.front()->pos) <= 5.0f) { // TODO fix distance parameter
				bool found = false;
				for (int i = 0; i < collected_types.size(); i++) {
					if (hasType(targeted_constructions.front()->resources, collected_types[i])) {
						targeted_constructions.front()->resources[collected_types[i]]--;
						collected_types.erase(collected_types.begin() + i);
						found = true;
						break;
					}
				}
				
				if (!found) {
					targeted_constructions.pop();
				}
				
				if (collected_types.empty()) {
					targeted_constructions.pop();
					state = WORKER_STATES::IDLE;
				}
				
			}
			else {
				pos = Vector2MoveTowards(pos, targeted_constructions.front()->pos, SPEED_MOD * speed * GetFrameTime());
			}
		}
	}

}



int main() {

	bool pause = false;

	std::srand(std::time(nullptr)); // time based seed for RNG

	std::vector<Generator*> generators;

	std::vector<Resource*> resources;

	std::vector<Worker*> workers;

	std::vector<Stockpile*> stockpiles;

	std::vector<Construction*> constructions;

	bool draw_dummy = false;
	Stockpile* stockpile_dummy = new Stockpile(0,{ 0,0 }, 0, nullptr);

	InitWindow(1600, 800, "AnyGame");
	SetTargetFPS(30);
	while (!WindowShouldClose()) {

		if (IsKeyReleased(KEY_SPACE)) {
			pause = !pause;
		}

		if (IsKeyReleased(KEY_BACKSPACE)) {
			constructions.erase(constructions.begin(), constructions.begin() + constructions.size());
			stockpiles.erase(stockpiles.begin(), stockpiles.begin() + stockpiles.size());
			workers.erase(workers.begin(), workers.begin() + workers.size());
			resources.erase(resources.begin(), resources.begin() + resources.size());
			generators.erase(generators.begin(), generators.begin() + generators.size());
		}

		if (IsKeyReleased(KEY_G)) {
			Vector2 mouse_pos = GetMousePosition();
			generators.emplace_back(new Generator(generator_id++, mouse_pos,20,rand()%3,3,30.f,30));
		}

		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
			Vector2 mouse_pos = GetMousePosition();
			int collide = false;
			for (Resource* res : resources) {
				if (CheckCollisionPointCircle(mouse_pos, res->pos, res->r)) {
					collide = true;
					res->type = ++res->type % MAX_TYPE;
				}
			}
			if (!collide) {
				resources.emplace_back(new Resource(resource_id++, mouse_pos, std::rand() % MAX_TYPE));
			}
		}

		if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
			Vector2 mouse_pos = GetMousePosition();
			workers.emplace_back(new Worker(worker_id++, mouse_pos, 1.0f));
		}

		if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
			stockpile_dummy->pos = GetMousePosition();
			draw_dummy = true;
			
		}
		
		if (IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE)) {
			Vector2 mouse_pos = GetMousePosition();
			int collide = false;
			for (Stockpile* s : stockpiles) {
				if (CheckCollisionCircles(mouse_pos, stockpile_dummy->r, s->pos, s->r)) {
					collide = true;
				}
			}
			if (!collide) {
				stockpiles.emplace_back(new Stockpile(stockpile_id++, mouse_pos, 10, nullptr));
			}
			draw_dummy = false;
		}

		if (IsKeyPressed(KEY_C)) {
			Vector2 mouse_pos = GetMousePosition();
			int collide = false;
			for (Stockpile* s : stockpiles) {
				if (CheckCollisionCircles(mouse_pos, stockpile_dummy->r, s->pos, s->r)) {
					collide = true;
				}
			}
			if (!collide) {
				std::array<int, MAX_TYPE> tmp = { 1,1,1 };
				Construction* c = new Construction(construction_id++, mouse_pos, tmp, 3, 5.0f, 2);
				constructions.emplace_back(c);
				stockpiles.emplace_back(new Stockpile(stockpile_id++, mouse_pos, 10, c));
			}
			draw_dummy = false;
		}

		BeginDrawing();
		ClearBackground(BLACK);

		for (Stockpile* s : stockpiles) {
			s->draw();

			/*if (!pause) {
				printf("s%i stored resources:\n", s->id);
				for (int i = 0; i < MAX_TYPE; i++) {
					printf("%i ", s->stored_types[i]);
				}
				printf("\n");
			}*/
		}

		for (Construction* c : constructions) {
			//printf("is all delivered: %i\n", c->isAllDelivered());
			DrawText(("c: " + std::to_string(c->id)).c_str(), c->pos.x + 5, c->pos.y - 20 - 2, 15, RED);
		}

		for (Generator* g : generators) {
			if (g->isEmpty()) {
				int i = 0;
				for (Generator* n : generators) {
					if (g == n) {
						generators.erase(generators.begin() + i);
						break;
					}
					i++;
				}
				continue;
			}
			if (!pause) {
				g->update(resources);
			}			
			g->draw();
		}

		for (Resource* res : resources) {
			DrawCircleV(res->pos, res->r, type_color[res->type]);
			DrawText(("r: " + std::to_string(res->id)).c_str(), res->pos.x - 5, res->pos.y - res->r - 2, 15, RED);
		}

		int i = 0;
		for (Worker* w : workers) {
			if (!pause) {
				w->update(resources, stockpiles, workers, generators, constructions);
				for (int j : w->collected_types) {
				}
			}
			w->draw();
			i++;
		}
		
		if (draw_dummy) {
			DrawCircleLinesV(stockpile_dummy->pos, stockpile_dummy->r, DARKGRAY);
		}

		EndDrawing();
	}
	CloseWindow();
	return 0;
}