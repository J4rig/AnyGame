#include <raylib.h>
#include <raymath.h>

#include <vector>
#include <queue>
#include <array>
#include <memory>

#define MAX_TYPE 4

#define NATURAL_TYPE 3

#define SPEED_MOD 100

#define WORKER_CAPACITY 2

#define MAX_PRIORITY 4

#define STOCKPILE_CAPACITY 10

using namespace std;

int construction_id = 0;
int stockpile_id = 0;
int resource_id = 0;
int worker_id = 0;
int generator_id = 0;
int forge_id = 0;

int storage_id = 0;
int task_id = 0;

Color type_color[MAX_TYPE] = { MAGENTA, DARKGREEN, DARKBLUE, YELLOW };



// class that stores resources
class Storage {
public:
	int id;

	Vector2 pos;

	int priority;

	int capacity;

	bool can_take;

	bool remove = false;

	array<int, MAX_TYPE> is = { 0 };
	array<int, MAX_TYPE> will_be = { 0 };
	array<int, MAX_TYPE> can_be = { 0 };

	Storage(int id, Vector2 pos, int priority, int capacity, array<int, MAX_TYPE> limits, bool can_take);

	int isStored();
	int aboutToBeStored();

	bool isFull(int type);
	bool hasSpace(int type);
	int spaceLeft(int type);
};



// class that stores progres done on various operations
class Task {
public:
	int id;

	Vector2 pos;

	int priority;

	float work_to_do;
	float work_done = 0.0f;

	int max_workers;
	int current_workers = 0;

	bool finished = false;

	Task(int id, Vector2 pos, int priority, float work_to_do, int max_workers);

	bool hasWorkers();
	bool isFullyOccupied();
	bool isCompleted();
};



//class that holds information for structures before they are completed
class Construction {
public:
	int id;
	Vector2 pos;

	bool is_all_delivered = false;
	weak_ptr<Storage> storage;

	weak_ptr<Task> task;

	Construction(int id, Vector2 pos, weak_ptr<Storage> storage, weak_ptr<Task> task);

	//void update(); // if storage is full creeates task if task is completed deletes self
};



class Stockpile {
public:
	int id;

	Vector2 pos;
	float r;

	weak_ptr<Construction> construction;
	weak_ptr<Storage> storage;

	Stockpile(int id, Vector2 pos, float r, weak_ptr < Construction> construction, weak_ptr <Storage> storage);

	void draw();
};



class Resource {
public:
	int id;
	Vector2 pos;
	float r = 5.0f;

	int type;

	bool occupied = false;
	bool taken = false;

	Resource(int id, Vector2 pos, int type);

	void draw();

	//void update(); // delete if taken
};



class Generator {
public:
	int id;
	Vector2 pos;
	float r;

	int type;

	int max;
	int remaining;

	weak_ptr<Task> task;

	float dispense_radius;

	Generator(int id, Vector2 pos, float r, int type, int max, float dispense_radius);

	void draw();

	bool isEmpty();

	//void update(); // delete if isEmpty() else if just_genreated reset task
};

class Forge {
public:

	int id;
	Vector2 pos;
	float r;

	weak_ptr<Storage> storage;
	weak_ptr<Task> task;

	int type = 3;

	Forge(int id, Vector2 pos, float r, weak_ptr<Storage> storage);

	void draw();

};



auto storage_cmp = [](shared_ptr<Storage> left, shared_ptr<Storage> right) {return left->priority > right->priority; };
auto task_cmp = [](shared_ptr<Task > left, shared_ptr<Task> right) {return left->priority > right->priority; };



enum class WORKER_STATES {
	IDLE = 0,
	COLLECTING,
	TRANSPORTING,
	OPERATING
};

class Worker {
public:
	int id;
	Vector2 pos;
	float r = 10.0f;
	float speed;

	WORKER_STATES state = WORKER_STATES::IDLE;

	int capacity;

	vector<int> collected_types; // types picked up

	vector<int> types_to_deliver; // types to pick up and deliver
	queue<int> amount_to_take = queue<int>(); // how many resources does worker take from targeted storages
	queue<int> amount_to_deliver = queue<int>(); // how many resources does worker deliver to targeted storages

	queue<weak_ptr<Resource>> targeted_resources;
	queue<weak_ptr<Storage>> targeted_storages;
	weak_ptr<Task> targeted_task;

	Worker(int id, Vector2 pos, float speed);

	void update(vector<shared_ptr<Resource>> resources,
		vector<shared_ptr<Storage>> storages,
		vector<shared_ptr<Task>> tasks
		);

	void draw();

	//if is full
	bool isFull();	

	//if will be full after collecting everithing to deliver
	bool isPacked();


	// if possible, sets all the necessary data for valid resource collection route
	bool collectResources(vector<shared_ptr<Resource>> resources, vector<shared_ptr<Storage>> storages);

	bool transportResources(vector<shared_ptr<Storage>> storages);

	bool completeTask(vector<shared_ptr<Task>> tasks);

	//bool deliverResourcesToConstructions(vector<Construction*> constructions, vector<Stockpile*> stockpiles);

	//bool workOnConstruction(vector<Construction*> constructions);
};

Vector2 rotateAroundPoint(Vector2 point, Vector2 center, float angleInRads);

shared_ptr<Resource> findClosestResource(Vector2 point, vector<shared_ptr<Resource>> resources, array<int,MAX_TYPE> valid_types);

weak_ptr<Storage> findStorageToIdle(Vector2 point, vector<shared_ptr<Storage>> storages);

weak_ptr<Storage> findStorageToDeliver(Vector2 point, vector<shared_ptr<Storage>> storages);

weak_ptr<Storage> findStorageToStore(Vector2 point, vector<shared_ptr<Storage>> storages,int type);

weak_ptr<Storage> findStorageToTake(Vector2 point, vector<shared_ptr<Storage>> storages, array<int, MAX_TYPE>& return_types, array<int, MAX_TYPE> wanted_types, int max_priority);

weak_ptr<Task> findTask(Vector2 point, vector<shared_ptr<Task>> tasks);

array<int, MAX_TYPE> hasTypes(array<int, MAX_TYPE> stored_types, array<int, MAX_TYPE> types);

bool hasType(array<int, MAX_TYPE> stored_types, int type);

array<int, MAX_TYPE> arangeTypes(vector<int> types);

int resourceCount(array<int, MAX_TYPE> stored_types);

queue<int> cutToCapacity(array<int, MAX_TYPE> stored_types, int capacity);

array<int, MAX_TYPE> canBeStored(vector<shared_ptr<Storage>> storages);

void insertStorage(vector<shared_ptr<Storage>> &storages, shared_ptr<Storage> storage);

void insertTask(vector<shared_ptr<Task>> &tasks, shared_ptr<Task> task);