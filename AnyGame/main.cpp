#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <string>
#include <ctime>

#define MAX_TYPE 3

#define SPEED_MOD 100

Color type_color[MAX_TYPE] = { MAGENTA,DARKGREEN,DARKBLUE };

int randomize_type(int* chances);

class Stockpile {
public :
	Vector2 pos;
	float r;
	int type;
	int cap;
	int stored;
	int new_stored;

	Stockpile(Vector2 pos, int type, int cap);

	bool isFull();
};

bool Stockpile::isFull() {
	return new_stored >= cap;
}

Stockpile::Stockpile(Vector2 pos, int type, int cap) :
	pos(pos), r(40.0f), type(type), cap(cap), stored(0), new_stored(0) {};

class Resource {
public:
	Vector2 pos;
	float r;
	bool occupied;
	int type;

	Resource(Vector2 pos, int type);
};

Resource::Resource(Vector2 pos, int type) :
	pos(pos), r(5.0f), occupied(false), type(type) {};

// TODO - Generator class that creates resources over time

class Generator {
public:
	Vector2 pos;
	float r;

	float normal_time_to_spawn;
	float deviation;
	float time_to_next_spawn;
	float time_since_last_spawn;

	int *chances;

	Generator(Vector2 pos, float r, float normal_time_to_spawn, float deviation, int chances[MAX_TYPE-1]);

	void update(std::vector<Resource*> &resources);
};

Generator::Generator(Vector2 pos, float r, float normal_time_to_spawn, float deviation, int chances[MAX_TYPE-1]) :
	pos(pos), r(r), normal_time_to_spawn(normal_time_to_spawn), deviation(deviation), time_to_next_spawn(0.0f),
	time_since_last_spawn(0.0f), chances(chances) {};

int randomize_type(int* chances) {
	int random_percentage = rand() % 100;
	int sum = 0;
	for (int i = 0; i < MAX_TYPE - 1; i++) {
		sum += abs(chances[i]);
	}
	int type = -1;
	for (int i = 0; i < MAX_TYPE - 1; i++) {
		int probability = 0;
		for (int j = 0; j < i; j++) {
			probability += abs(chances[j]);
		}
		if (random_percentage < probability / sum) {
			type = i;
		}
	}
	if (type == -1) {
		type = MAX_TYPE - 1;
	}
	return type;
}

void Generator::update(std::vector<Resource*> &resources) {
	if (time_since_last_spawn == 0.0f) {
		time_to_next_spawn = (normal_time_to_spawn - deviation) + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (2*deviation));
	}
	else if (time_since_last_spawn >= time_to_next_spawn) {
		Vector2 r_pos = { (pos.x - r) + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (2 * r)),
			(pos.y - r) + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (2 * r)) };

		int type = randomize_type(chances);

		resources.emplace_back(new Resource(r_pos,type));
		time_since_last_spawn = 0.0f;
	}
	time_since_last_spawn += GetFrameTime();
}



/*
		States:
		0 - idle
		1 - collecting
		2 - transporting - TODO - transports reources between two stockpiles or a stockpile and a processing unit
		3 - processing - TODO - takes an empty role of processor in a processing unit and creates new resource over some time
	*/
class Collector {
public:
	Vector2 pos;
	float r;
	float speed;

	int type;
	int new_type;
	int state;
	bool has_resource;
	
	Resource* targeted_resource;
	Stockpile* targeted_stockpile;
	Vector2 target_pos;
	

	Collector(Vector2 pos, float speed);

	void update(std::vector<Resource*> &resources, std::vector<Stockpile*> stockpiles, std::vector<Collector*> collectors);

	bool changeType(std::vector<Resource*> resources, std::vector<Stockpile*> stockpiles);

	bool is_optimal(Resource* targeted_resource, Stockpile* targeted_stockpile, std::vector<Collector*> collectors);

	bool collectResource(std::vector<Resource*> resources, std::vector<Stockpile*> stockpiles, std::vector<Collector*> collectors);
};

Resource* findClosestResource(Vector2 point, int type, std::vector<Resource*> resources);

Stockpile* findClosestStockpile(Vector2 point, int type, std::vector<Stockpile*> stockpiles, bool mode);

bool deleteResource(Resource* resource, std::vector<Resource*>& resources);

Collector::Collector(Vector2 pos, float speed) :
	pos(pos), r(10.0f), speed(speed), type(-1), new_type(-1), state(0), has_resource(false), targeted_resource(nullptr), targeted_stockpile(nullptr), target_pos(pos) {};

// set new_type to type with most unoccupied resources and return true if type has changed
bool Collector::changeType(std::vector<Resource*> resources, std::vector<Stockpile*> stockpiles) {
	
	int tmp[MAX_TYPE] = {0};
	if (resources.size() == 0) {
		return false;
	}
	for (Resource* r : resources) {
		if (!r->occupied) {
			tmp[r->type]++;
		}
	}
	int max = 0;
	for (int i = 0; i < MAX_TYPE; i++) {
		if (max < tmp[i]) {
			bool stockpile_exists = false;
			for (Stockpile* s : stockpiles) {
				if ((s->type == -1 || s->type == i) && s->new_stored < s->cap) {
					stockpile_exists = true;
					break;
				}
			}
			if (stockpile_exists) {
				max = tmp[i];
				new_type = i;
			}
		}
	}

	return type != new_type;

}

// returns closest resource with same type
Resource* findClosestResource(Vector2 point, int type, std::vector<Resource*> resources) {
	if (resources.empty()) {
		return nullptr;
	}
	Resource* result = nullptr;
	float min_dst = -1.0f;
	for (Resource* r : resources) {
		if (!r->occupied && r->type == type && (min_dst == -1.0f || Vector2Distance(point,r->pos) < min_dst )) {
			result = r;
			min_dst = Vector2Distance(point, r->pos);
		}
	}
	return result;
}

// returns closest non-full stockpile with same type or neutral type
Stockpile* findClosestStockpile(Vector2 point, int type, std::vector<Stockpile*> stockpiles, bool mode) {
	float min_distance = -1.0f;
	Stockpile* result = nullptr;
	for (Stockpile* s : stockpiles) {
		if ((s->cap > s->new_stored || mode) && (s->type == type || s->type == -1) &&
		(min_distance == -1.0f || Vector2Distance(point, s->pos) < min_distance)) {
			min_distance = Vector2Distance(point, s->pos);
			result = s;
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
			//delete(resource);
			return true;
		}
		i++;
	}
	return false;
}


// check if this collector is the best option
// if there is another idle collector it can be more optimal to collect the selected resource
bool Collector::is_optimal(Resource* targeted_resource, Stockpile* targeted_stockpile, std::vector<Collector*> collectors) {
	for (Collector* c : collectors) {
		if (c == this) {
			continue;
		}
		// if another collector is idle, it can be more optimal to be assigned to collect the resource
		if (c->state == 0) {
			// both collectors have different types than selected resource
			// one closer to the selected stockpile is more optimal
			if (type != targeted_resource->type && c->type != targeted_resource->type) {
				if (Vector2Distance(pos, targeted_stockpile->pos) > Vector2Distance(c->pos, targeted_stockpile->pos)) {
					return false;
				}
			}
			// this collector has different type but another collector has the same type as a selected resource,
			// that makes him optimal if the distance between him and the resource is less
			// than the distance between this collector and selected stockpile + selected stockpile and selected resource
			else if (type != targeted_resource->type && c->type == targeted_resource->type) {
				if (Vector2Distance(pos, targeted_stockpile->pos) + Vector2Distance(targeted_stockpile->pos, targeted_resource->pos)
				> Vector2Distance(c->pos, targeted_resource->pos)) {
					return false;
				}
			}

			else if (type == targeted_resource->type && c->type != targeted_resource->type) {
				if (Vector2Distance(c->pos, targeted_stockpile->pos) + Vector2Distance(targeted_stockpile->pos, targeted_resource->pos)
					< Vector2Distance(pos, targeted_resource->pos)) {
					return false;
				}
			}
		}
	}
}

// if possible, sets all the necessary data for valid resource collection route
bool Collector::collectResource(std::vector<Resource*> resources, std::vector<Stockpile*> stockpiles, std::vector<Collector*> collectors) {
	Vector2 new_target_pos;
	Resource* new_targeted_resource;
	Stockpile* new_targeted_stockpile;

	changeType(resources, stockpiles);
	new_targeted_resource = findClosestResource(pos, new_type, resources);
	if (new_targeted_resource != nullptr) {
		new_target_pos = new_targeted_resource->pos;
		new_targeted_stockpile = findClosestStockpile(new_target_pos, new_type, stockpiles, false);
		if (new_targeted_stockpile != nullptr) {
			if (!is_optimal(new_targeted_resource, new_targeted_stockpile, collectors)) {
				return false;
			}
			targeted_resource = new_targeted_resource;
			targeted_stockpile = new_targeted_stockpile;
			target_pos = new_target_pos;

			targeted_stockpile->new_stored++;
			targeted_resource->occupied = true;
			state = 1;
			return true;
		}
	}
	return false;
}


// decides what to do next for each collector
void Collector::update(std::vector<Resource*> &resources, std::vector<Stockpile*> stockpiles, std::vector<Collector*> collectors) {

	if (state == 0) { //idle
		
		if (targeted_stockpile != nullptr) {
			if (!collectResource(resources, stockpiles, collectors)) {
				if (Vector2Distance(pos, target_pos) <= targeted_stockpile->r + r) {
					pos = rotateAroundPoint(pos, target_pos, 0.5f * GetFrameTime());
				}
				else {
					pos = Vector2MoveTowards(pos, target_pos, SPEED_MOD * speed * GetFrameTime());
				}
			}
		}
		else {
			targeted_stockpile = findClosestStockpile(pos, new_type, stockpiles, true);
			if (targeted_stockpile != nullptr) {
				target_pos = targeted_stockpile->pos;
			}
		}
		
	}

	else if (state == 1) { // collecting
		if (type == new_type) {
			if (has_resource == false) {
				if (Vector2Distance(pos, target_pos) <= 0.5f) {
					deleteResource(targeted_resource, resources);
					has_resource = true;
				}
				else
				{
					pos = Vector2MoveTowards(pos, target_pos, SPEED_MOD * speed * GetFrameTime());
				}
			}
			else {
				if (targeted_stockpile->pos != target_pos) {
					target_pos = targeted_stockpile->pos;
				}
				else {
					if (Vector2Distance(pos, target_pos) <= targeted_stockpile->r + r) {
						targeted_stockpile->stored++;
						has_resource = false;
						state = 0;
					}
					else {
						pos = Vector2MoveTowards(pos, target_pos, SPEED_MOD * speed * GetFrameTime());
					}
				}
			}
		}
		else {
			target_pos = targeted_stockpile->pos;
			if (Vector2Distance(pos, target_pos) <= targeted_stockpile->r + r) {
				type = new_type;
				target_pos = targeted_resource->pos;
			}
			else {
				pos = Vector2MoveTowards(pos, target_pos, SPEED_MOD * speed * GetFrameTime());
			}
		}
	}

}

int main() {

	bool pause = false;

	std::srand(std::time(nullptr)); // time based seed for RNG

	std::vector<Generator*> generators;

	std::vector<Resource*> resources;

	std::vector<Collector*> collectors;

	std::vector<Stockpile*> stockpiles;

	bool draw_dummy = false;
	float dummy_time = 0.0f;
	Stockpile* stockpile_dummy = new Stockpile({ 0,0 }, 0, 0);

	InitWindow(1600, 800, "Window");
	SetTargetFPS(30);
	while (!WindowShouldClose()) {

		if (IsKeyReleased(KEY_SPACE)) {
			pause = !pause;
		}

		if (IsKeyReleased(KEY_BACKSPACE)) {
			stockpiles.erase(stockpiles.begin(), stockpiles.begin() + stockpiles.size());
			collectors.erase(collectors.begin(), collectors.begin() + collectors.size());
			resources.erase(resources.begin(), resources.begin() + resources.size());
		}

		if (IsKeyReleased(KEY_G)) {
			Vector2 mousePos = GetMousePosition();
			int chances[2] = { 33,33 };
			generators.emplace_back(new Generator(mousePos, 50, 3, 0.5f, chances));
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
			collectors.emplace_back(new Collector(mousePos, 1.0f));
		}

		if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
			Vector2 mousePos = GetMousePosition();
			draw_dummy = true;
			dummy_time += GetFrameTime();
			stockpile_dummy->pos = mousePos;
			stockpiles.empty() ? stockpile_dummy->type = -1 : stockpile_dummy->type = (int)dummy_time % MAX_TYPE;

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
				stockpiles.emplace_back(new Stockpile(mousePos, stockpile_dummy->type, 2));
			}
			draw_dummy = false;
			dummy_time = 0.0f;
		}

		BeginDrawing();
		ClearBackground(BLACK);

		for (Generator* g : generators) {
			g->update(resources);
			DrawRectangleLines(g->pos.x - g->r, g->pos.y - g->r, 2 * g->r, 2 * g->r, RED);
		}

		for (Stockpile* s : stockpiles) {
			DrawCircleV(s->pos, s->r, s->type == -1 ? DARKGRAY : type_color[s->type]);
			DrawText((std::to_string(s->stored) + "/" + std::to_string(s->cap)).c_str(), s->pos.x - 10, s->pos.y, 10, WHITE);
		}

		for (Resource* res : resources) {
			DrawCircleV(res->pos, res->r, type_color[res->type]);
		}

		int i = 0;
		for (Collector* c : collectors) {
			if (!pause) {
				c->update(resources, stockpiles, collectors);
				printf("Collector %i new_type: %i\n", i, c->new_type);
			}

			if (c->has_resource) {
				DrawCircleV(c->pos, c->r, c->type == -1 ? DARKGRAY : type_color[c->type]);
			}
			else {
				DrawRing(c->pos, 5, c->r, 0, 360, 0, c->type == -1 ? DARKGRAY : type_color[c->type]);
			}
			i++;
		}

		if (draw_dummy) {
			DrawCircleV(stockpile_dummy->pos, stockpile_dummy->r, stockpile_dummy->type == -1 ? DARKGRAY : type_color[stockpile_dummy->type]);
		}

		EndDrawing();
	}
	CloseWindow();
	return 0;
}