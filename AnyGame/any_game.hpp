#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <queue>

#define MAX_TYPE 3

#define SPEED_MOD 100

Color type_color[MAX_TYPE] = { MAGENTA,DARKGREEN,DARKBLUE };

class Stockpile {
public:
	Vector2 pos;
	float r;

	int capacity;
	int currently_stored;
	int about_to_be_stored;
	int stored_types[MAX_TYPE] = { 0 };

	Stockpile(Vector2 pos, int cap);

	bool hasSpace();
	bool isFull();
	int spaceLeft();
	void draw();
};

class Resource {
public:
	Vector2 pos;
	float r;
	bool occupied;
	int type;

	Resource(Vector2 pos, int type);
};

class Generator {
public:
	Vector2 pos;
	float r;

	int type;

	int max;
	int remaining;

	bool operated; //true if worker is already on site
	bool occupied; //true if worker is moving on site

	float time_operated;
	float time_to_generate;

	float dispense_radius;

	bool just_generated;

	Generator(Vector2 pos, float r, int type, int remaining, float time_to_generate, float dispense_radius);

	void update(std::vector<Resource*>& resources);

	void draw();

	bool isEmpty();
};


/*
		States:
		0 - idle
		1 - collecting - TODO - update decision algorithm for current version of game
		2 - generating - TODO - operates a generator
		3 - transporting - TODO - transports reources between two stockpiles or a stockpile and a processing unit
		4 - processing - TODO - operates a processor
	*/

enum class WorkerStates {
	IDLE = 0,
	COLLECTING,
	GENERATING
};

class Worker {
public:
	Vector2 pos;
	float r;
	float speed;

	//int type;
	//int new_type;

	WorkerStates state;

	int capacity;
	int collected;
	std::vector<int> collected_types;

	//int collected_types[MAX_TYPE] = { 0 };

	std::queue<Resource*> targeted_resources;
	std::queue<Stockpile*> targeted_stockpiles;
	Generator* targeted_generator;


	Worker(Vector2 pos, float speed);

	void update(std::vector<Resource*>& resources, std::vector<Stockpile*> stockpiles, std::vector<Worker*> Workers, std::vector<Generator*> generators);

	void draw();

	//bool changeType(std::vector<Resource*> resources, std::vector<Stockpile*> stockpiles);

	//bool is_optimal(Resource* targeted_resource, Stockpile* targeted_stockpile, std::vector<Worker*> Workers);

	bool isFull();

	bool collectResource(std::vector<Resource*> resources, std::vector<Stockpile*> stockpiles, std::vector<Worker*> Workers);
};

Resource* findClosestResource(Vector2 point, std::vector<Resource*> resources);

Stockpile* findClosestStockpile(Vector2 point, std::vector<Stockpile*> stockpiles, bool mode);

Generator* findClosestGenerator(Vector2 point, std::vector<Generator*> generators, std::vector<Worker*> Workers);

bool deleteResource(Resource* resource, std::vector<Resource*>& resources);