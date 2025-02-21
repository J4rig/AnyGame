#pragma once
#include "any_game.hpp"

enum class WORKER_STATES {
	IDLE = 0,
	COLLECTING,
	TRANSPORTING,
	OPERATING
};

class Worker {
public:
	int id;
	int tribe;

	Vector2 pos;
	float r = 10.0f;
	float speed;

	weak_ptr<Target> target;

	WORKER_STATES state = WORKER_STATES::IDLE;

	int capacity = WORKER_CAPACITY;

	vector<int> collected_types = vector<int>(); // types picked up

	vector<int> types_to_deliver = vector<int>(); // types to pick up and deliver
	queue<int> amount_to_take = queue<int>(); // how many resources does worker take from targeted storages
	queue<int> amount_to_deliver = queue<int>(); // how many resources does worker deliver to targeted storages

	vector<weak_ptr<Resource>> targeted_resources = vector<weak_ptr<Resource>>();
	queue<weak_ptr<Storage>> targeted_storages = queue<weak_ptr<Storage>>();
	weak_ptr<Task> targeted_task = weak_ptr<Task>();

	Worker(int id, int tribe, Vector2 pos, weak_ptr<Target> target, float speed);

	void update(vector<shared_ptr<Resource>> resources, vector<shared_ptr<Storage>> storages, vector<shared_ptr<Task>> tasks);

	void draw() const;

	bool isPacked() const;

	bool collectResources(vector<shared_ptr<Resource>> resources, vector<shared_ptr<Storage>> storages);

	bool transportResources(vector<shared_ptr<Storage>> storages);

	bool completeTask(vector<shared_ptr<Task>> tasks);
};