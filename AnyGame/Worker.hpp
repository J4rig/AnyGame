#pragma once
#include "any_game.hpp"
#include "Drawing.hpp"

enum class WORKER_STATES {
	IDLE = 0,
	TRANSPORTING,
	OPERATING
};

class Worker : public Drawing{
public:
	int id;

	int tribe;

	Vector2 pos;
	float r = 10.0f;
	float speed = 1.0f;

	weak_ptr<Target> target;

	WORKER_STATES state = WORKER_STATES::IDLE;

	int capacity = WORKER_CAPACITY;

	vector<int> collected_types = vector<int>(); // types picked up

	vector<int> types_to_deliver = vector<int>(); // types to pick up and deliver

	vector<weak_ptr<Storage>> targeted_storages = vector<weak_ptr<Storage>>();
	weak_ptr<Task> targeted_task = weak_ptr<Task>();

	Worker(int z, int id, int tribe, Vector2 pos, weak_ptr<Target> target);

	void update(vector<shared_ptr<Storage>> storages, vector<shared_ptr<Task>> tasks);

	void draw() const;

	bool isPacked() const;

	bool transportResources(vector<shared_ptr<Storage>> storages);

	bool completeTask(vector<shared_ptr<Task>> tasks);

	vector<int> die();

	void forgetStorage(shared_ptr<Storage> storage);
};