#include "any_game.hpp"

#include <string>
#include <ctime>




Stockpile::Stockpile(Vector2 pos, int capacity) :
	pos(pos), r(40.0f), capacity(capacity), currently_stored(0), about_to_be_stored(0) {};

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
	DrawRing(pos, r - 2, r, 0, 360, 0, DARKGRAY);

	//TODO change, not to calculate all the time but only recalculate when capacity changes
	float piece = 360.0f * (1.0f / (float)capacity);
	int drawn_pieces = 0;
	for (int i = 0; i < MAX_TYPE; i++) {
		//printf("drawn_pieces: %i\n", drawn_pieces);
		DrawRing(pos, 0, r-1, drawn_pieces*piece,(stored_types[i]+drawn_pieces)*piece,0,type_color[i]);
		drawn_pieces += stored_types[i];
	}

	DrawText((std::to_string(currently_stored) + "/" + std::to_string(capacity)).c_str(), pos.x - 10, pos.y, 10, WHITE);
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
	pos(pos), r(10.0f), speed(speed), state(WorkerStates::IDLE), capacity(2), collected(0), collected_types(std::vector<int>()),
	targeted_resources(std::queue<Resource*>()), targeted_stockpiles(std::queue<Stockpile*>()), targeted_generator(nullptr) {
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
Resource* findClosestResource(Vector2 point, std::vector<Resource*> resources) {
	if (resources.empty()) {
		return nullptr;
	}
	Resource* result = nullptr;
	float min_dst = -1.0f;
	for (Resource* r : resources) {
		if (!r->occupied && (min_dst == -1.0f || Vector2Distance(point, r->pos) < min_dst)) {
			result = r;
			min_dst = Vector2Distance(point, r->pos);
		}
	}
	return result;
}

// returns closest non-full stockpile with same type or neutral type
// mode defines if the stockpile can be full or not
Stockpile* findClosestStockpile(Vector2 point, std::vector<Stockpile*> stockpiles, bool mode) {
	float min_distance = -1.0f;
	Stockpile* result = nullptr;
	for (Stockpile* s : stockpiles) {
		if ((!s->hasSpace() || mode) && (min_distance == -1.0f || Vector2Distance(point, s->pos) < min_distance)) {
			min_distance = Vector2Distance(point, s->pos);
			result = s;
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
				if (w->state == WorkerStates::IDLE && Vector2Distance(w->pos, g->pos) < Vector2Distance(point, g->pos)) {
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
bool Worker::collectResource(std::vector<Resource*> resources, std::vector<Stockpile*> stockpiles, std::vector<Worker*> workers) {
	int found_resources = 0;

	Resource* new_targeted_resource;
	Stockpile* new_targeted_stockpile;

	while (found_resources < capacity) {
		new_targeted_resource = findClosestResource(pos, resources);
		if (new_targeted_resource != nullptr) {
			new_targeted_stockpile = findClosestStockpile(new_targeted_resource->pos, stockpiles, false);
			if (new_targeted_stockpile != nullptr) {
				targeted_resources.push(new_targeted_resource);
				if (targeted_stockpiles.front()->isFull()) {
					targeted_stockpiles.pop();
					targeted_stockpiles.push(new_targeted_stockpile);
				}
				else if (targeted_stockpiles.front() != new_targeted_stockpile) {
					targeted_stockpiles.push(new_targeted_stockpile);
				}
				targeted_stockpiles.front()->about_to_be_stored++;
				targeted_resources.front()->occupied = true;
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



void Worker::update(std::vector<Resource*> &resources, std::vector<Stockpile*> stockpiles, std::vector<Worker*> workers, std::vector<Generator*> generators) {

	if (state == WorkerStates::IDLE) {
		
		if (!targeted_stockpiles.empty()) {
			if (!collectResource(resources, stockpiles, workers)) {
				targeted_generator = findClosestGenerator(pos, generators, workers);	
				if (targeted_generator != nullptr) {
					targeted_generator->occupied = true;
					state = WorkerStates::GENERATING;
				}
				else if (Vector2Distance(pos, targeted_stockpiles.front()->pos) <= targeted_stockpiles.front()->r + r) {
					pos = rotateAroundPoint(pos, targeted_stockpiles.front()->pos, 0.5f * GetFrameTime());
				}
				else {
					pos = Vector2MoveTowards(pos, targeted_stockpiles.front()->pos, SPEED_MOD * speed * GetFrameTime());
				}
			}
			else {
				state = WorkerStates::COLLECTING;
			}
		}
		else {
			Stockpile* tmp = findClosestStockpile(pos, stockpiles, true);
			if (tmp != nullptr) {
				targeted_stockpiles.push(tmp);
			}
		}
		
	}

	else if (state == WorkerStates::COLLECTING) {
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
					state = WorkerStates::IDLE;
				}

			}
			else {
				pos = Vector2MoveTowards(pos, targeted_stockpiles.front()->pos, SPEED_MOD * speed * GetFrameTime());
			}
		}
	}

	else if (state == WorkerStates::GENERATING) {
		if (Vector2Distance(pos, targeted_generator->pos) <= 0.5f) {
			pos = targeted_generator->pos;
			targeted_generator->operated = true;
			if (targeted_generator->just_generated) {
				targeted_generator->just_generated = false;
				targeted_generator->operated = false;
				targeted_generator->time_operated = 0.0f;
				targeted_generator->occupied = false;
				state = WorkerStates::IDLE;
			}
		}
		else {
			pos = Vector2MoveTowards(pos, targeted_generator->pos, speed * SPEED_MOD * GetFrameTime());
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

	bool draw_dummy = false;
	Stockpile* stockpile_dummy = new Stockpile({ 0,0 }, 0);

	InitWindow(1600, 800, "Window");
	SetTargetFPS(30);
	while (!WindowShouldClose()) {

		if (IsKeyReleased(KEY_SPACE)) {
			pause = !pause;
		}

		if (IsKeyReleased(KEY_BACKSPACE)) {
			stockpiles.erase(stockpiles.begin(), stockpiles.begin() + stockpiles.size());
			workers.erase(workers.begin(), workers.begin() + workers.size());
			resources.erase(resources.begin(), resources.begin() + resources.size());
			generators.erase(generators.begin(), generators.begin() + generators.size());
		}

		if (IsKeyReleased(KEY_G)) {
			Vector2 mousePos = GetMousePosition();
			generators.emplace_back(new Generator(mousePos,20,rand()%3,3,30.f,30));
		}

		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
			Vector2 mousePos = GetMousePosition();
			int collide = false;
			for (Resource* res : resources) {
				if (CheckCollisionPointCircle(mousePos, res->pos, res->r)) {
					collide = true;
					res->type = ++res->type % MAX_TYPE;
				}
			}
			if (!collide) {
				resources.emplace_back(new Resource(mousePos, std::rand() % MAX_TYPE));
			}
		}

		if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
			Vector2 mousePos = GetMousePosition();
			workers.emplace_back(new Worker(mousePos, 1.0f));
		}

		if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
			stockpile_dummy->pos = GetMousePosition();
			draw_dummy = true;
			
		}
		
		if (IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE)) {
			Vector2 mousePos = GetMousePosition();
			int collide = false;
			for (Stockpile* s : stockpiles) {
				if (CheckCollisionCircles(mousePos, stockpile_dummy->r, s->pos, s->r)) {
					collide = true;
				}
			}
			if (!collide) {
				stockpiles.emplace_back(new Stockpile(mousePos, 10));
			}
			draw_dummy = false;
		}

		BeginDrawing();
		ClearBackground(BLACK);

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
				w->update(resources, stockpiles, workers, generators);
				//printf("Worker %i state: %i\n", i, w->state);
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