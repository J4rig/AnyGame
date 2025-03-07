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

inline int node_id = 0;

inline int raider_id = 0;
inline int construction_id = 0;
inline int stockpile_id = 0;
inline int resource_id = 0;
inline int worker_id = 0;
inline int generator_id = 0;
inline int forge_id = 0;

inline int storage_id = 0;
inline int task_id = 0;
inline int target_id = 0;

inline int settlement_id = 0;
inline int tribe_id = 0;

inline int selected_tribe = 0;
inline int selected_settlement = 0;


inline const array<Color, MAX_TYPE> type_color = { MAGENTA, DARKGREEN, DARKBLUE, YELLOW };
inline const array<Color, MAX_TRIBE> tribe_color = { RED,LIGHTGRAY,ORANGE,BLUE };

class Drawing;

class Tribe;

class Settlement;


class Storage;

class Task;


class Construction;

class Node;

class Stockpile;

class Generator;

class Forge;


class Target;


class Raider;

class Worker;



Vector2 rotateAroundPoint(Vector2 point, Vector2 center, float angleInRads);

weak_ptr<Storage> findStorageToIdle(Vector2 point, vector<shared_ptr<Storage>> storages);

weak_ptr<Storage> findStorageToDeliver(Vector2 point, vector<shared_ptr<Storage>> storages);

weak_ptr<Storage> findStorageToStore(Vector2 point, vector<shared_ptr<Storage>> storages, vector<int> types);

weak_ptr<Storage> findStorageToTake(Vector2 point, int tribe, vector<shared_ptr<Storage>> storages, array<int, MAX_TYPE>& return_types, array<int, MAX_TYPE> wanted_types, int max_priority);

weak_ptr<Task> findTask(Vector2 point, vector<shared_ptr<Task>> tasks);

weak_ptr<Target> findTarget(Vector2 point, weak_ptr<Target> attacker, vector<weak_ptr<Target>> targets);

array<int, MAX_TYPE> hasTypes(array<int, MAX_TYPE> stored_types, array<int, MAX_TYPE> types);

bool hasType(array<int, MAX_TYPE> stored_types, int type);

array<int, MAX_TYPE> arangeTypes(vector<int> types);

int resourceCount(array<int, MAX_TYPE> stored_types);

queue<int> cutToCapacity(array<int, MAX_TYPE> stored_types, int capacity);

array<int, MAX_TYPE> canBeStored(vector<shared_ptr<Storage>> storages);

void insertStorageShared(vector<shared_ptr<Storage>>& storages, shared_ptr<Storage> storage);

void insertStorageWeak(vector<weak_ptr<Storage>>& storages, shared_ptr<Storage> storage);

void insertTaskShared(vector<shared_ptr<Task>>& tasks, shared_ptr<Task> task);

void insertTaskWeak(vector<weak_ptr<Task>>& tasks, shared_ptr<Task> task);