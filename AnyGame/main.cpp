#include "any_game.hpp"

#include <string>
#include <ctime>


Construction::Construction(Vector2 pos, std::array<int, MAX_TYPE> resources, int number_of_resources, float work, int max_workers) :
	pos(pos),resources(resources), about_to_be_resources(resources), number_of_resources(number_of_resources),
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


Stockpile::Stockpile(Vector2 pos, int capacity, Construction* construction) :
	pos(pos), r(40.0f), construction(construction), capacity(capacity), currently_stored(0), about_to_be_stored(0) {};

int Stockpile::hasType(std::array<int, MAX_TYPE> types) {
	for (int i = 0; i < MAX_TYPE; i++) {
		if (types[i] > 0 && stored_types[i] > 0) {
			return i;
		}
	}
	return -1;
}

bool Stockpile::hasSpace() {
	return about_to_be_stored >= capacity;
}

bool Stockpile::isFull() {
	return currently_stored >= capacity;
}

int Stockpile::spaceLeft() {
	return capacity - currently_stored;
}

void Stockpile::draw() {
	if (construction == nullptr) {
		DrawRing(pos, r - 2, r, 0, 360, 0, DARKGRAY);

		//TODO change, not to calculate all the time but only recalculate when capacity changes
		float piece = 360.0f * (1.0f / (float)capacity);
		int drawn_pieces = 0;
		for (int i = 0; i < MAX_TYPE; i++) {
			//printf("drawn_pieces: %i\n", drawn_pieces);
			DrawRing(pos, 0, r - 1, drawn_pieces * piece, (stored_types[i] + drawn_pieces) * piece, 0, type_color[i]);
			drawn_pieces += stored_types[i];
		}

		DrawText((std::to_string(currently_stored) + "/" + std::to_string(capacity)).c_str(), pos.x - 10, pos.y, 10, WHITE);
	}
	else {
		DrawRing(pos, r - 2, r, 0, 360, 0, RED);
		float piece = 360.0f * (1.0f / (float)construction->number_of_resources);
		int drawn_pieces = 0;
		//printf("piece: %i\n", construction->number_of_resources);
		for (int i = 0; i < MAX_TYPE; i++) {		
			DrawRing(pos, 0, r - 1, drawn_pieces * piece, (construction->resources[i] + drawn_pieces) * piece, 0, type_color[i]);
			drawn_pieces += construction->resources[i];
		}

		
	}
	
}

Resource::Resource(Vector2 pos, int type) :
	pos(pos), r(5.0f), occupied(false), type(type) {};



Generator::Generator(Vector2 pos, float r, int type, int max, float time_to_generate, float dispense_radius) :
	pos(pos), r(r), type(type), max(max), remaining(max), operated(false), occupied(false),
	time_operated(0.0f), time_to_generate(3.0f), dispense_radius(dispense_radius), just_generated(false) {};


void Generator::update(std::vector<Resource*> &resources) {
	if (operated/*&& isEmpty()*/) { // maybe redundant since Worker will only operate if is not empty
		if (time_operated >= time_to_generate) {
			Vector2 r_pos = { (pos.x - dispense_radius) + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (2 * dispense_radius)),
				(pos.y - r) + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (2 * dispense_radius)) };
			resources.emplace_back(new Resource(r_pos,type));
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
	//DrawRectangleLines(pos.x - dispense_radius, pos.y - dispense_radius, 2 * dispense_radius, 2 * dispense_radius, RED);
}

bool Generator::isEmpty() {
	return remaining <= 0;
}


Worker::Worker(Vector2 pos, float speed) :
	pos(pos), r(10.0f), speed(speed), state(WORKER_STATES::IDLE), capacity(2), collected(0), collected_types(std::vector<int>()), type_to_deliver(-1),
	targeted_resources(std::queue<Resource*>()), targeted_stockpiles(std::queue<Stockpile*>()), targeted_generator(nullptr), targeted_construction(nullptr) {
};

void Worker::draw() {
	DrawRing(pos, 4 /*resource radius -1 so it overlaps and hides imperfections*/, r, 0, 360, 0, DARKGRAY);

	if (collected > 0) {
		//TODO change, not to calculate all the time but only recalculate when capacity changes
		float piece = 360.0f * (1.0f / (float)capacity);

		for (int i = 0; i < collected; i++) {
			//DrawCircleV(pos, 5, type_color[collected_types[i]]);
			DrawRing(pos, 0, 5/*Resource radius*/, i * piece, (i+1) * piece, 1, type_color[collected_types[i]]);
		}
	}
}

bool Worker::isFull() {
	return collected >= capacity;
}

// returns closest resource
Resource* findClosestResource(Vector2 point, std::vector<Resource*> resources, int type) { //-1 if type is not important or number of type
	Resource* result = nullptr;
	float min_dst = -1.0f;
	for (Resource* r : resources) {
		if (!r->occupied && (min_dst == -1.0f || Vector2Distance(point, r->pos) < min_dst) &&
			(type == -1 || type == r->type)) {

			result = r;
			min_dst = Vector2Distance(point, r->pos);
		}
	}
	return result;
}

// returns closest non-full stockpile with same type or neutral type
// mode defines if we take from or deliver to stockpile or if we are just looking for any stockpile: <0 any stockpile, 0 - deliver, >0 - take
// types is an array containing amounts of types required by construction
Stockpile* findClosestStockpile(Vector2 point, std::vector<Stockpile*> stockpiles, int mode, int& return_type, std::array<int, MAX_TYPE> types = { 0 }) {
	float min_distance = -1.0f;
	Stockpile* result = nullptr;
	bool found = false;
	for (Stockpile* s : stockpiles) {
		if (s->construction == nullptr && mode <= 0 && (!s->hasSpace() || mode < 0)) { // delivering to or looking for any stockpile
			found = true;
		}
		else if (s->construction == nullptr && mode > 0 && (return_type = s->hasType(types)) > -1) { // taking from stockpile
			found = true;
		}

		if (found && (min_distance == -1.0f || Vector2Distance(point, s->pos) < min_distance)) {
			min_distance = Vector2Distance(point, s->pos);
			result = s;
			found = false;
		}
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

Construction* findClosestConstruction(Vector2 point, std::vector<Construction*> constructions, std::vector<Stockpile*> stockpiles) {

	float min_distance = -1.0f;
	Construction* result = nullptr;
	for (Construction* c : constructions) {
		if (!c->isAllDelivered() && (min_distance == -1.0f || Vector2Distance(point, c->pos) < min_distance)) {
			int type = 0;
			Stockpile* tmpStockpile = findClosestStockpile(point, stockpiles, 1 , type, c->about_to_be_resources);

			if (tmpStockpile != nullptr) {
				min_distance = Vector2Distance(point, c->pos) < min_distance;
				result = c;
			}
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

// if possible, sets all the necessary data for valid resource collection route
// TODO add suport for capacity > 1
bool Worker::collectResources(std::vector<Resource*> resources, std::vector<Stockpile*> stockpiles, std::vector<Worker*> workers) {
	int found_resources = 0;

	Resource* new_targeted_resource = nullptr;
	Stockpile* new_targeted_stockpile = nullptr;

	while (found_resources < capacity) {
		new_targeted_resource = findClosestResource(pos, resources, -1);
		if (new_targeted_resource != nullptr) {
			int type = 0;
			new_targeted_stockpile = findClosestStockpile(new_targeted_resource->pos, stockpiles, 0, type);
			if (new_targeted_stockpile != nullptr) {
				new_targeted_resource->occupied = true;
				new_targeted_stockpile->about_to_be_stored++;
				targeted_resources.push(new_targeted_resource);
				if (targeted_stockpiles.front()->isFull()) {
					targeted_stockpiles.pop();
					targeted_stockpiles.push(new_targeted_stockpile);
				}
				else if (targeted_stockpiles.front() != new_targeted_stockpile) {
					targeted_stockpiles.push(new_targeted_stockpile);
				}
				found_resources++;
			}
			else {			
				break;
			}
		}
		else {
			break;
		}

		new_targeted_resource = nullptr;
		new_targeted_stockpile = nullptr;
	}

	return found_resources > 0;
}

bool Worker::deliverResourcesToConstruction(std::vector<Construction*> constructions, std::vector<Stockpile*> stockpiles, int& type) { // TODO fix ugly code

	Construction* new_targeted_construction = nullptr;
	Stockpile* new_targeted_stockpile = nullptr;

	if ((new_targeted_construction = findClosestConstruction(pos, constructions, stockpiles)) != nullptr &&
		(new_targeted_stockpile = findClosestStockpile(pos, stockpiles, 1, type, new_targeted_construction->about_to_be_resources)) != nullptr) {
		//printf("type: %i\n", type);
		targeted_construction = new_targeted_construction;
		// maybe empty the queue of stockpiles first
		targeted_stockpiles.push(new_targeted_stockpile);
		targeted_construction->about_to_be_resources[type]--;
		return true;
	}
	return false;
}



void Worker::update(std::vector<Resource*> &resources, std::vector<Stockpile*> stockpiles, std::vector<Worker*> workers,
					std::vector<Generator*> generators, std::vector<Construction*> constructions) {

	if (state == WORKER_STATES::IDLE) {
		if (!targeted_stockpiles.empty()) {
			if (deliverResourcesToConstruction(constructions,stockpiles,type_to_deliver)) {
				state = WORKER_STATES::TRANSPORTING;
			}

			else if (!collectResources(resources, stockpiles, workers)) {
				targeted_generator = findClosestGenerator(pos, generators, workers);	
				if (targeted_generator != nullptr) {
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
				state = WORKER_STATES::COLLECTING;
			}
		}
		else {
			int type = 0;
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
				collected++;
				
			}
			else
			{
				pos = Vector2MoveTowards(pos, targeted_resources.front()->pos, SPEED_MOD * speed * GetFrameTime());
			}
		}
		else {
			if (Vector2Distance(pos, targeted_stockpiles.front()->pos) <= targeted_stockpiles.front()->r + r) {
				
				while (collected > 0 && !targeted_stockpiles.front()->isFull()) {
					targeted_stockpiles.front()->stored_types[collected_types[collected_types.size()-1]]++;
					targeted_stockpiles.front()->currently_stored++;
					collected_types.erase(collected_types.begin() + collected_types.size()  - 1);
					collected--;
				}
				
				if (collected > 0 && /* not sure if necessary --> */ !targeted_stockpiles.empty()) {
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
		if (collected <= 0) {
			if (Vector2Distance(pos, targeted_stockpiles.front()->pos) <= targeted_stockpiles.front()->r + r) {
				collected_types.emplace_back(type_to_deliver);
				collected++;
				targeted_stockpiles.front()->about_to_be_stored--;
				targeted_stockpiles.front()->currently_stored--;
				targeted_stockpiles.front()->stored_types[type_to_deliver]--;
				
			}
			else {
				pos = Vector2MoveTowards(pos, targeted_stockpiles.front()->pos, SPEED_MOD * speed * GetFrameTime());
			}
		}
		else {
			if (Vector2Distance(pos, targeted_construction->pos) <= 5.0f) { // TODO fix distance parameter
				targeted_construction->resources[collected_types.front()]--;
				collected_types.erase(collected_types.begin());
				collected--;
				state = WORKER_STATES::IDLE;
			}
			else {
				pos = Vector2MoveTowards(pos, targeted_construction->pos, SPEED_MOD * speed * GetFrameTime());
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
	Stockpile* stockpile_dummy = new Stockpile({ 0,0 }, 0, nullptr);

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
			generators.emplace_back(new Generator(mouse_pos,20,rand()%3,3,30.f,30));
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
				resources.emplace_back(new Resource(mouse_pos, std::rand() % MAX_TYPE));
			}
		}

		if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
			Vector2 mouse_pos = GetMousePosition();
			workers.emplace_back(new Worker(mouse_pos, 1.0f));
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
				if (stockpiles.empty()) {
					stockpiles.emplace_back(new Stockpile(mouse_pos, 10, nullptr));
				}
				else {
					std::array<int,MAX_TYPE> tmp = {1,1,1};
					Construction* c = new Construction(mouse_pos, tmp, 3, 5.0f, 2);
					constructions.emplace_back(c);
					stockpiles.emplace_back(new Stockpile(mouse_pos, 10, c));
				}	
			}
			draw_dummy = false;
		}

		BeginDrawing();
		ClearBackground(BLACK);

		for (Construction* c : constructions) {
			//printf("is all delivered: %i\n", c->isAllDelivered());
		}

		for (Stockpile* s : stockpiles) {
			s->draw();
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
		}

		int i = 0;
		for (Worker* w : workers) {
			if (!pause) {
				w->update(resources, stockpiles, workers, generators, constructions);
				//printf("Worker %i collected: ", i);
				for (int j : w->collected_types) {
					//printf("%i ",j);
				}
				//printf("\n");
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