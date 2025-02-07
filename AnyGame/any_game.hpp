#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <queue>
#include <array>

#define MAX_TYPE 3

#define SPEED_MOD 100

#define WORKER_CAPACITY 2

int construction_id = 0;
int stockpile_id = 0;
int resource_id = 0;
int worker_id = 0;
int generator_id = 0;

Color type_color[MAX_TYPE] = { MAGENTA, DARKGREEN, DARKBLUE };

// class that holds information for structures before they are completed
class Construction {
public:
	int id;
	Vector2 pos;

	std::array<int,MAX_TYPE> resources;
	std::array<int, MAX_TYPE> about_to_be_resources;
	int number_of_resources;

	float work;
	float work_done;

	int max_workers;
	int current_workers;

	Construction(int id, Vector2 pos, std::array<int, MAX_TYPE> resources, int number_of_resources,float work, int max_workers);

	bool isAllDelivered();
	bool hasWorkers();
	bool isFullyOccupied();
	bool isCompleted();
};

class Storage {
public:
	std::array<int, MAX_TYPE> is = { 0 };
	std::array<int, MAX_TYPE> will_be = { 0 };
};

class Stockpile {
public:
	int id;
	Vector2 pos;
	float r;

	Construction* construction;

	int capacity;
	int currently_stored;
	int about_to_be_stored;
	Storage* stored_types = new Storage();

	Stockpile(int id, Vector2 pos, int cap, Construction* construction);

	bool hasSpace();
	bool isFull();
	int spaceLeft();
	void draw();
};

class Resource {
public:
	int id;
	Vector2 pos;
	float r;

	bool occupied;
	int type;

	Resource(int id, Vector2 pos, int type);
};

class Generator {
public:
	int id;
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

	Generator(int id, Vector2 pos, float r, int type, int remaining, float time_to_generate, float dispense_radius);

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
	int id;
	Vector2 pos;
	float r;
	float speed;

	WORKER_STATES state;

	int capacity;

	std::vector<int> collected_types; // types picked up

	std::vector<int> types_to_deliver; // types to pick up and deliver
	std::queue<int> amount_to_take; // how many resources do we take from given stockpile


	std::queue<Resource*> targeted_resources;
	std::queue<Stockpile*> targeted_stockpiles;
	Generator* targeted_generator;
	std::queue<Construction*> targeted_constructions;


	Worker(int id, Vector2 pos, float speed);

	void update(std::vector<Resource*>& resources, std::vector<Stockpile*> stockpiles, std::vector<Worker*> Workers,
				std::vector<Generator*> generators, std::vector<Construction*> constructions);

	void draw();

	// if is full
	bool isFull();	

	// if will be full after collecting everithing to deliver
	bool isPacked();

	bool collectResources(std::vector<Resource*> resources, std::vector<Stockpile*> stockpiles, std::vector<Worker*> Workers);

	bool deliverResourcesToConstructions(std::vector<Construction*> constructions, std::vector<Stockpile*> stockpiles);
};

Resource* findClosestResource(Vector2 point, std::vector<Resource*> resources, int type);

Stockpile* findClosestStockpile(Vector2 point, std::vector<Stockpile*> stockpiles, int mode, std::array<int, MAX_TYPE> &return_types, std::array<int, MAX_TYPE> type);

Generator* findClosestGenerator(Vector2 point, std::vector<Generator*> generators, std::vector<Worker*> Workers);

Construction* findClosestConstruction(Vector2 point, std::vector<Construction*> constructions, std::vector<Stockpile*> stockpiles,
									  Stockpile* &new_targeted_stockpile, std::array<int, MAX_TYPE>& types);


bool deleteResource(Resource* resource, std::vector<Resource*>& resources);

std::array<int, MAX_TYPE> hasTypes(std::array<int, MAX_TYPE> stored_types, std::array<int, MAX_TYPE> types);

bool hasType(std::array<int, MAX_TYPE> stored_types, int type);

std::array<int, MAX_TYPE> arangeTypes(std::vector<int> types);

int resourceCount(std::array<int, MAX_TYPE> stored_types);

std::queue<int> cutToCapacity(std::array<int, MAX_TYPE> stored_types, int capacity);