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

#include <ctime>
#include <iostream>

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

int main() {

	bool pause = false;

	srand((unsigned int)time(nullptr)); // time based seed for RNG

	
	vector<shared_ptr<Storage>> storages;

	vector<shared_ptr<Task>> tasks;

	vector<shared_ptr<Combat>> targets;


	vector<shared_ptr<Generator>> generators;

	vector<shared_ptr<Stockpile>> stockpiles;

	vector<shared_ptr<Resource>> resources;

	vector<shared_ptr<Worker>> workers;

	vector<shared_ptr<Construction>> constructions;

	vector<shared_ptr<Forge>> forges;

	vector<shared_ptr<Raider>> raiders; 

	

	InitWindow(1600, 800, "AnyGame");
	SetTargetFPS(30);
	while (!WindowShouldClose()) {

		if (IsKeyReleased(KEY_SPACE)) {
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


		}

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

		if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT)) {
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
		}

		BeginDrawing();
		ClearBackground(BLACK);

		for (shared_ptr<Stockpile> s : stockpiles) {
			if (s->construction.expired() && s->storage.expired()) {
				array<int, MAX_TYPE> limits = { 0 };
				limits.fill(STOCKPILE_CAPACITY);

				shared_ptr<Storage> new_storage = make_shared<Storage>(storage_id++, s->pos, 0, 10, limits, true);
				insertStorage(storages, new_storage);
				s->storage = new_storage;
			}
			s->draw();
		}

		for (int g = 0; g < generators.size(); g++) {

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
		}

		for (int res = 0; res < resources.size(); res++) {
			if (resources[res]->taken) {
				resources.erase(resources.begin() + res);
				res--;
			}
			else {
				resources[res]->draw();
			}
		}

		for (int f = 0; f < forges.size(); f++) {
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
		}

		for (int c = 0; c < constructions.size(); c++) {
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
		}

		for (int t = 0; t < targets.size(); t++) {
			if (!pause &&targets[t]->isDead()) {
				targets.erase(targets.begin() + t);
				t--;
				continue;
			}
		}

		for (int r = 0; r < raiders.size(); r++) {
			if (!pause) {
				if (raiders[r]->combat.expired()) {
					raiders.erase(raiders.begin() + r);
					r--;
					continue;
				}

				raiders[r]->update(targets);
			}
			raiders[r]->draw();
		}

		for (shared_ptr<Worker> w : workers) {
			if (!pause) {
				w->update(resources, storages, tasks);
			}
			w->draw();
		}

		for (int t = 0; t < tasks.size(); t++) {
			if (tasks[t]->finished && tasks[t]->hasWorkers() == 0) {
				tasks.erase(tasks.begin() + t);
				t--;
			}
		}

		for (int s = 0; s < storages.size(); s++) {
			if (storages[s]->remove) {
				storages.erase(storages.begin() + s);
				s--;
			}
		}

		EndDrawing();
	}
	CloseWindow();
	return 0;
}