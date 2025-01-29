#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <queue>
#include <array>

#define MAX_TYPE 3

#define SPEED_MOD 100

Color type_color[MAX_TYPE] = { MAGENTA,DARKGREEN,DARKBLUE };

// class that holds information for structures before they are completed
class Construction {
public:

	Vector2 pos;

	std::array<int,MAX_TYPE> resources;
	std::array<int, MAX_TYPE> about_to_be_resources;
	int number_of_resources;

	float work;
	float work_done;

	int max_workers;
	int current_workers;

	Construction(Vector2 pos, std::array<int, MAX_TYPE> resources, int number_of_resources,float work, int max_workers);

	bool isAllDelivered();
	bool hasWorkers();
	bool isFullyOccupied();
	bool isCompleted();
};

class Stockpile {
public:
	Vector2 pos;
	float r;

	Construction* construction;

	int capacity;
	int currently_stored;
	int about_to_be_stored;
	int stored_types[MAX_TYPE] = { 0 };

	Stockpile(Vector2 pos, int cap, Construction* construction);

	int hasType(std::array<int, MAX_TYPE> types);
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
		1 - collecting - TODO - optimize - add an option to collect resources directly to constructions or processing units
		2 - generating - TODO - optimize
		3 - transporting - TODO - transports reources from a stockpile to a construction or a processing unit
		4 - processing - TODO - operates a processor
	*/

enum class WORKER_STATES {
	IDLE = 0,
	COLLECTING,
	GENERATING,
	TRANSPORTING
};

class Worker {
public:
	Vector2 pos;
	float r;
	float speed;

	WORKER_STATES state;

	int capacity;
	int collected;
	std::vector<int> collected_types;

	int type_to_deliver;

	std::queue<Resource*> targeted_resources;
	std::queue<Stockpile*> targeted_stockpiles;
	Generator* targeted_generator;
	Construction* targeted_construction;


	Worker(Vector2 pos, float speed);

	void update(std::vector<Resource*>& resources, std::vector<Stockpile*> stockpiles, std::vector<Worker*> Workers,
				std::vector<Generator*> generators, std::vector<Construction*> constructions);

	void draw();

	bool isFull();

	bool collectResources(std::vector<Resource*> resources, std::vector<Stockpile*> stockpiles, std::vector<Worker*> Workers);

	bool deliverResourcesToConstruction(std::vector<Construction*> constructions, std::vector<Stockpile*> stockpiles, int& type);
};

Resource* findClosestResource(Vector2 point, std::vector<Resource*> resources, int type);

Stockpile* findClosestStockpile(Vector2 point, std::vector<Stockpile*> stockpiles, int mode, int& return_type, std::array<int, MAX_TYPE> type);

Generator* findClosestGenerator(Vector2 point, std::vector<Generator*> generators, std::vector<Worker*> Workers);

Construction* findClosestConstruction(Vector2 point, std::vector<Construction*> constructions, std::vector<Stockpile*> stockpiles);


bool deleteResource(Resource* resource, std::vector<Resource*>& resources);