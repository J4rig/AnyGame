#pragma once
#include <raylib.h>
#include <raymath.h>

#include <vector>
#include <queue>
#include <array>
#include <memory>
#include <functional>

constexpr auto MAX_TYPE = 4;

constexpr auto MAX_TRIBE = 4;

constexpr auto NATURAL_TYPE = 3;

constexpr auto SPEED_MOD = 100;

constexpr auto WORKER_CAPACITY = 4;

constexpr auto MAX_PRIORITY = 4;

constexpr auto STOCKPILE_CAPACITY = 10;

using namespace std;

inline const array<Color, MAX_TYPE> type_color = { MAGENTA, DARKGREEN, DARKBLUE, YELLOW };
inline const array<Color, MAX_TRIBE> tribe_color = { RED,LIGHTGRAY,ORANGE,BLUE };


class Tribe;

class Settlement;



class Storage;

class Task;

class Combat;



class Raider;

class Construction;

class Stockpile;

class Resource;

class Generator;

class Forge;

class Worker;



Vector2 rotateAroundPoint(Vector2 point, Vector2 center, float angleInRads);

shared_ptr<Resource> findClosestResource(Vector2 point, vector<shared_ptr<Resource>> resources, array<int,MAX_TYPE> valid_types);

weak_ptr<Storage> findStorageToIdle(Vector2 point, vector<shared_ptr<Storage>> storages);

weak_ptr<Storage> findStorageToDeliver(Vector2 point, vector<shared_ptr<Storage>> storages);

weak_ptr<Storage> findStorageToStore(Vector2 point, vector<shared_ptr<Storage>> storages, vector<int> types);

weak_ptr<Storage> findStorageToTake(Vector2 point, vector<shared_ptr<Storage>> storages, array<int, MAX_TYPE>& return_types, array<int, MAX_TYPE> wanted_types, int max_priority);

weak_ptr<Task> findTask(Vector2 point, vector<shared_ptr<Task>> tasks);

weak_ptr<Combat> findTarget(Vector2 point, weak_ptr<Combat> attacker, vector<shared_ptr<Combat>> targets);

array<int, MAX_TYPE> hasTypes(array<int, MAX_TYPE> stored_types, array<int, MAX_TYPE> types);

bool hasType(array<int, MAX_TYPE> stored_types, int type);

array<int, MAX_TYPE> arangeTypes(vector<int> types);

int resourceCount(array<int, MAX_TYPE> stored_types);

queue<int> cutToCapacity(array<int, MAX_TYPE> stored_types, int capacity);

array<int, MAX_TYPE> canBeStored(vector<shared_ptr<Storage>> storages);

void insertStorageShared(vector<shared_ptr<Storage>>& storages, shared_ptr<Storage> storage);

void insertStorageWeak(vector<weak_ptr<Storage>> &storages, shared_ptr<Storage> storage);

void insertTask(vector<shared_ptr<Task>> &tasks, shared_ptr<Task> task);