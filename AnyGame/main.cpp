#pragma once
#include "any_game.hpp"

#include "Storage.hpp"
#include "Task.hpp"
#include "Combat.hpp"

#include "Raider.hpp"
#include "Construction.hpp"
#include "Stockpile.hpp"
#include "Resource.hpp"
#include "Generator.hpp"
#include "Forge.hpp"
#include "Worker.hpp"

#include "Tribe.hpp"
#include "Settlement.hpp"

#include <ctime>
#include <iostream>
#include <string>

int raider_id = 0;
int construction_id = 0;
int stockpile_id = 0;
int resource_id = 0;
int worker_id = 0;
int generator_id = 0;
int forge_id = 0;

int storage_id = 0;
int task_id = 0;
int combat_id = 0;

int settlement_id = 0;
int tribe_id = 0;

int selected_tribe = -1;

int main() {

	bool pause = false;

	srand((unsigned int)time(nullptr)); // time based seed for RNG

	
	vector<weak_ptr<Storage>> storages;

	vector<weak_ptr<Task>> tasks;

	vector<weak_ptr<Combat>> targets;



	vector<weak_ptr<Generator>> generators;

	vector<weak_ptr<Stockpile>> stockpiles;

	vector<weak_ptr<Worker>> workers;

	vector<weak_ptr<Construction>> constructions;

	vector<weak_ptr<Forge>> forges;

	vector<weak_ptr<Raider>> raiders;


	vector<weak_ptr<Settlement>> settlements;


	vector<shared_ptr<Resource>> resources;

	vector<shared_ptr<Tribe>> tribes;

	
	
	InitWindow(1600, 800, "AnyGame");
	SetTargetFPS(30);
	while (!WindowShouldClose()) {

		if (IsKeyReleased(KEY_KP_ADD)) {
			selected_tribe = ++selected_tribe % MAX_TRIBE;
		}

		if (IsKeyReleased(KEY_KP_SUBTRACT)) {
			if (selected_tribe == 0) {
				selected_tribe = MAX_TRIBE;
			}
			selected_tribe = --selected_tribe % MAX_TRIBE;
		}

		if (IsKeyReleased(KEY_T) && tribe_id < MAX_TRIBE) {
			selected_tribe = tribe_id++;
			Vector2 mouse_pos = GetMousePosition();

			// worker
			shared_ptr<Worker> new_worker = make_shared<Worker>(worker_id++, selected_tribe, Vector2AddValue(mouse_pos, 60), 1);
			workers.emplace_back(new_worker);

			// storage
			array<int, MAX_TYPE> limits = { 0 };
			limits.fill(STOCKPILE_CAPACITY);

			shared_ptr<Storage> new_storage = make_shared<Storage>(storage_id++, mouse_pos, 0, STOCKPILE_CAPACITY, limits, true);
			insertStorageWeak(storages, new_storage);

			//stockpile
			shared_ptr<Stockpile> new_stockpile = make_shared<Stockpile>(stockpile_id++, selected_tribe, mouse_pos, 40.0f, weak_ptr<Construction>(), new_storage);
			stockpiles.emplace_back(new_stockpile);

			shared_ptr<Settlement> new_settlement = make_shared<Settlement>(settlement_id++, mouse_pos,1);

			new_settlement->workers.emplace_back(new_worker);
			insertStorageShared(new_settlement->storages, new_storage);
			new_settlement->storages.emplace_back(new_storage);
			new_settlement->stockpiles.emplace_back(new_stockpile);

			settlements.emplace_back(new_settlement);

			shared_ptr<Tribe> new_tribe = make_shared<Tribe>(selected_tribe, new_settlement);
			tribes.emplace_back(new_tribe);
		}

		if (IsKeyReleased(KEY_S) && selected_tribe != -1) {
			Vector2 mouse_pos = GetMousePosition();

			// worker
			shared_ptr<Worker> new_worker = make_shared<Worker>(worker_id++, selected_tribe, Vector2AddValue(mouse_pos, 60), 1);
			workers.emplace_back(new_worker);

			// storage
			array<int, MAX_TYPE> limits = { 0 };
			limits.fill(STOCKPILE_CAPACITY);

			shared_ptr<Storage> new_storage = make_shared<Storage>(storage_id++, mouse_pos, 0, STOCKPILE_CAPACITY, limits, true);
			insertStorageWeak(storages, new_storage);

			//stockpile
			shared_ptr<Stockpile> new_stockpile = make_shared<Stockpile>(stockpile_id++, selected_tribe, mouse_pos, 40.0f, weak_ptr<Construction>(), new_storage);
			stockpiles.emplace_back(new_stockpile);

			shared_ptr<Settlement> new_settlement = make_shared<Settlement>(settlement_id++, mouse_pos, 1);

			new_settlement->workers.emplace_back(new_worker);
			insertStorageShared(new_settlement->storages, new_storage);
			new_settlement->storages.emplace_back(new_storage);
			new_settlement->stockpiles.emplace_back(new_stockpile);

			settlements.emplace_back(new_settlement);

			tribes[selected_tribe]->settlements.emplace_back(new_settlement);
		}

		/*if (IsKeyReleased(KEY_SPACE)) {
			pause = !pause;
		}
		
		if (IsKeyReleased(KEY_BACKSPACE)) {
			raider_id = 0;
			construction_id = 0;
			stockpile_id = 0;
			resource_id = 0;
			worker_id = 0;
			generator_id = 0;
			forge_id = 0;

			storage_id = 0;
			task_id = 0;
			combat_id = 0;

			storages.erase(storages.begin(), storages.begin() + storages.size());
			tasks.erase(tasks.begin(), tasks.begin() + tasks.size());
			targets.erase(targets.begin(), targets.begin() + targets.size());

			resources.erase(resources.begin(), resources.begin() + resources.size());
			generators.erase(generators.begin(), generators.begin() + generators.size());
			stockpiles.erase(stockpiles.begin(), stockpiles.begin() + stockpiles.size());
			workers.erase(workers.begin(), workers.begin() + workers.size());
			forges.erase(forges.begin(), forges.begin() + forges.size());
			constructions.erase(constructions.begin(), constructions.begin() + constructions.size());
			raiders.erase(raiders.begin(), raiders.begin() + raiders.size());
		}*/

		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
			Vector2 mouse_pos = GetMousePosition();
			int collide = false;
			for (shared_ptr<Resource> res : resources) {
				if (CheckCollisionPointCircle(mouse_pos, res->pos, res->r*2)) {
					collide = true;
					res->type = ++res->type % MAX_TYPE;
				}
			}
			if (!collide) {
				resources.emplace_back(make_shared<Resource>(resource_id++, mouse_pos, rand() % NATURAL_TYPE));
			}
		}

		/*if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
			Vector2 mouse_pos = GetMousePosition();
			workers.emplace_back(new Worker(worker_id++, mouse_pos, 1.0f));
		}

		if (IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE)) {
			Vector2 mouse_pos = GetMousePosition();
			array<int, MAX_TYPE> limits = { 0 };
			limits.fill(STOCKPILE_CAPACITY);

			shared_ptr<Storage> new_storage = make_shared<Storage>(storage_id++, mouse_pos, 0, STOCKPILE_CAPACITY, limits, true);
			insertStorage(storages, new_storage);
		
			stockpiles.emplace_back(new Stockpile(stockpile_id++, mouse_pos, 40.0f, weak_ptr<Construction>(), new_storage));
		}

		if (IsKeyReleased(KEY_G)) {
			Vector2 mouse_pos = GetMousePosition();
			generators.emplace_back(make_shared<Generator>(generator_id++, mouse_pos, 20, rand() % NATURAL_TYPE, 3, 30));
		}

		if (IsKeyReleased(KEY_C)) {
			Vector2 mouse_pos = GetMousePosition();

			std::array<int, MAX_TYPE> tmp = { 0 };
			tmp[0] = 1;
			tmp[1] = 1;
			tmp[2] = 1;
			shared_ptr<Storage> new_storage = make_shared<Storage>(storage_id++, mouse_pos, 2, 3, tmp, false);
			insertStorage(storages, new_storage);
			shared_ptr<Construction> c = make_shared<Construction>(construction_id++,mouse_pos, new_storage,weak_ptr<Task>());
			constructions.emplace_back(c);
			stockpiles.emplace_back(make_shared<Stockpile>(stockpile_id++, mouse_pos, 40.0f, c, weak_ptr<Storage>()));
		}

		if (IsKeyReleased(KEY_F)) {
			Vector2 mouse_pos = GetMousePosition();
			std::array<int, MAX_TYPE> limits = { 0 };
			limits[1] = 1;
			limits[2] = 1;
			shared_ptr<Storage> new_storage = make_shared<Storage>(stockpile_id++, mouse_pos, 1, 2, limits, true);
			insertStorage(storages, new_storage);
			shared_ptr<Forge> forge = make_shared<Forge>(forge_id++, mouse_pos, 40, new_storage);
			forges.emplace_back(forge);
		}

		if (IsKeyReleased(KEY_R)) {
			Vector2 mouse_pos = GetMousePosition();
			shared_ptr<Combat> new_target = make_shared<Combat>(combat_id++, mouse_pos, 15, 10, 2, 3);
			targets.emplace_back(new_target);
			shared_ptr<Raider> new_raider = make_shared<Raider>(raider_id++, mouse_pos, new_target);
			raiders.emplace_back(new_raider);
		}*/

		for (shared_ptr<Tribe> tribe : tribes) {
			for (shared_ptr<Settlement> settlement : tribe->settlements) {

				for (shared_ptr<Stockpile> stockpile : settlement->stockpiles) {
					if (stockpile->construction.expired() && stockpile->storage.expired()) {
						array<int, MAX_TYPE> limits = { 0 };
						limits.fill(STOCKPILE_CAPACITY);

						shared_ptr<Storage> new_storage = make_shared<Storage>(storage_id++, stockpile->pos, 0, 10, limits, true);
						insertStorageWeak(storages, new_storage);
						insertStorageShared(settlement->storages, new_storage);
						stockpile->storage = new_storage;
					}
				}

				for (shared_ptr<Worker> w : settlement->workers) {
					w->update(resources, settlement->storages, settlement->tasks);
				}

				for (int task = 0; task < settlement->tasks.size(); task++) {
					if (settlement->tasks[task]->finished && settlement->tasks[task]->hasWorkers() == 0) {
						settlement->tasks.erase(settlement->tasks.begin() + task);
						task--;
					}
				}

				for (int storage = 0; storage < settlement->storages.size(); storage++) {
					if (settlement->storages[storage]->remove) {
						settlement->storages.erase(settlement->storages.begin() + storage);
						storage--;
					}
				}
			}
		}

		BeginDrawing();
		ClearBackground(BLACK);

		if (selected_tribe != -1) {
			DrawText(to_string(selected_tribe).c_str(), 0, 0, 20, tribe_color[selected_tribe]);
		}

		for (weak_ptr<Stockpile> s : stockpiles) {
			s.lock()->draw();
		}

		/*for (int g = 0; g < generators.size(); g++) {

			if (generators[g]->task.expired() && !generators[g]->isEmpty()) {
				shared_ptr<Task> new_task = make_shared<Task>(task_id++, generators[g]->pos, 0, 5.0f, 1);
				insertTask(tasks, new_task);
				generators[g]->task = new_task;
			}

			if (generators[g]->task.lock()->isCompleted()) {
				generators[g]->remaining--;
				resources.emplace_back(make_shared<Resource>(resource_id++,Vector2Add(generators[g]->pos, {(float)(rand() % 60 -30),(float)(rand() % 60 -30)} ), generators[g]->type));
				generators[g]->task.lock()->finished = true;
			}

			if (generators[g]->isEmpty()) {
				generators.erase(generators.begin() + g);
				g--;
			}
			else {
				generators[g]->draw();
			}
		}*/

		for (int res = 0; res < resources.size(); res++) {
			if (resources[res]->taken) {
				resources.erase(resources.begin() + res);
				res--;
			}
			else {
				resources[res]->draw();
			}
		}

		/*for (int f = 0; f < forges.size(); f++) {
			if (!forges[f]->task.expired() && forges[f]->task.lock()->isCompleted() && !forges[f]->task.lock()->finished) {
				forges[f]->storage.lock()->is = { 0 };
				forges[f]->storage.lock()->will_be = { 0 };
				resources.emplace_back(make_shared<Resource>(resource_id++, Vector2Add(forges[f]->pos, {(float)(rand() % 60 - 30),(float)(rand() % 60 - 30)}), 3));
				forges[f]->task.lock()->finished = true;
			}

			if ((!forges[f]->task.expired() && forges[f]->storage.lock()->hasSpace(-1))) {
				forges[f]->task.lock()->finished = true;
			}

			if (forges[f]->task.expired() && forges[f]->storage.lock()->isFull(-1)) {
				shared_ptr<Task> new_task = make_shared<Task>(task_id++, forges[f]->pos, 2, 3.0f, 1);
				insertTask(tasks, new_task);
				forges[f]->task = new_task;
			}
			forges[f]->draw();
		}*/

		/*for (int c = 0; c < constructions.size(); c++) {
			if (!constructions[c]->task.expired() && constructions[c]->task.lock()->isCompleted()) {	
				constructions[c]->task.lock()->finished = true;
				constructions.erase(constructions.begin() + c);		
				c--;
				continue;
			}
			
			if (!constructions[c]->storage.expired() && constructions[c]->storage.lock()->isFull(-1) && !constructions[c]->is_all_delivered) {
				constructions[c]->is_all_delivered = true;
				constructions[c]->storage.lock()->remove = true;
				shared_ptr<Task> new_task = make_shared<Task>(task_id++, constructions[c]->pos, 1, 5.0f, 2);
				insertTask(tasks, new_task);
				constructions[c]->task = new_task;
			}
		}*/

		/*for (int t = 0; t < targets.size(); t++) {
			if (!pause &&targets[t]->isDead()) {
				targets.erase(targets.begin() + t);
				t--;
				continue;
			}
		}*/

		/*for (int r = 0; r < raiders.size(); r++) {
			if (!pause) {
				if (raiders[r]->combat.expired()) {
					raiders.erase(raiders.begin() + r);
					r--;
					continue;
				}

				raiders[r]->update(targets);
			}
			raiders[r]->draw();
		}*/

		for (weak_ptr<Worker> w : workers) {
			w.lock()->draw();
		}

		EndDrawing();
	}
	CloseWindow();
	return 0;
}

// kubino<3astrid weeeeeeeeeeeeeee /(- 3-)/  