#pragma once
#include "any_game.hpp"

#include "Storage.hpp"
#include "Task.hpp"
#include "Target.hpp"

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

int main() {

	bool pause = false;

	srand((unsigned int)time(nullptr)); // time based seed for RNG

	
	vector<weak_ptr<Storage>> storages;

	vector<weak_ptr<Task>> tasks;

	vector<weak_ptr<Target>> targets;



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

		if (IsKeyReleased(KEY_SPACE)) {
			pause = !pause;
		}

		if (IsKeyDown(KEY_LEFT_CONTROL)) {
			if (IsKeyReleased(KEY_KP_ADD) && selected_settlement != -1) {
				selected_settlement = ++selected_settlement % (int)tribes[selected_tribe]->settlements.size();
			}

			if (IsKeyReleased(KEY_KP_SUBTRACT) && selected_settlement != -1) {
				if (selected_settlement == 0) {
					selected_settlement = (int)tribes[selected_tribe]->settlements.size();
				}
				selected_settlement = --selected_settlement % (int)tribes[selected_tribe]->settlements.size();
			}
		}

		else {
			if (IsKeyReleased(KEY_KP_ADD) && selected_tribe != -1) {
				selected_tribe = ++selected_tribe % (int)tribes.size();
				selected_settlement = 0;
			}

			if (IsKeyReleased(KEY_KP_SUBTRACT) && selected_tribe != -1) {
				if (selected_tribe == 0) {
					selected_tribe = (int)tribes.size();
				}
				selected_tribe = --selected_tribe % (int)tribes.size();
				selected_settlement = 0;
			}
		}

		if (IsKeyReleased(KEY_DELETE)) {
			for (shared_ptr<Tribe> tribe : tribes) {
				for (shared_ptr<Target> target : tribe->targets) {
					if (target->health == 30) {
						target->health = 0;
					}
				}
			}
		}

		
		if (IsKeyReleased(KEY_T) && tribe_id < MAX_TRIBE) {
			selected_tribe = tribe_id++;
			selected_settlement = 0;
			Vector2 mouse_pos = GetMousePosition();

			// storage
			array<int, MAX_TYPE> limits = { 0 };
			limits.fill(STOCKPILE_CAPACITY);

			shared_ptr<Storage> new_storage = make_shared<Storage>(storage_id++, mouse_pos, 0, STOCKPILE_CAPACITY, limits, true);
			insertStorageWeak(storages, new_storage);

			//target
			shared_ptr<Target> new_target = make_shared<Target>(target_id++, selected_tribe, &mouse_pos, 10, 30, 0, 0);
			targets.emplace_back(new_target);
			

			// stockpile
			shared_ptr<Stockpile> new_stockpile = make_shared<Stockpile>(stockpile_id++, selected_tribe, mouse_pos, 40.0f, weak_ptr<Construction>(), new_storage, new_target);
			stockpiles.emplace_back(new_stockpile);

			// settlement
			shared_ptr<Settlement> new_settlement = make_shared<Settlement>(settlement_id++, mouse_pos, 1);

			
			insertStorageShared(new_settlement->storages, new_storage);
			new_settlement->stockpiles.emplace_back(new_stockpile);

			settlements.emplace_back(new_settlement);

			// tribe
			shared_ptr<Tribe> new_tribe = make_shared<Tribe>(selected_tribe, new_settlement);
			new_tribe->targets.emplace_back(new_target);
			tribes.emplace_back(new_tribe);

			
		}

		if (IsKeyReleased(KEY_S) && selected_tribe != -1) {
			Vector2 mouse_pos = GetMousePosition();

			// storage
			array<int, MAX_TYPE> limits = { 0 };
			limits.fill(STOCKPILE_CAPACITY);

			shared_ptr<Storage> new_storage = make_shared<Storage>(storage_id++, mouse_pos, 0, STOCKPILE_CAPACITY, limits, true);
			insertStorageWeak(storages, new_storage);

			//target
			shared_ptr<Target> new_target = make_shared<Target>(target_id++, selected_tribe, &mouse_pos, 10, 30, 0, 0);
			targets.emplace_back(new_target);
			tribes[selected_tribe]->targets.emplace_back(new_target);

			// stockpile
			shared_ptr<Stockpile> new_stockpile = make_shared<Stockpile>(stockpile_id++, selected_tribe, mouse_pos, 40.0f, weak_ptr<Construction>(), new_storage, new_target);
			stockpiles.emplace_back(new_stockpile);

			// settlement
			selected_settlement = (int)tribes[selected_tribe]->settlements.size();
			shared_ptr<Settlement> new_settlement = make_shared<Settlement>(selected_settlement, mouse_pos, 1);

 			insertStorageShared(new_settlement->storages, new_storage);
			new_settlement->stockpiles.emplace_back(new_stockpile);

			settlements.emplace_back(new_settlement);

			tribes[selected_tribe]->settlements.emplace_back(new_settlement);
		}
		 
		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
			Vector2 mouse_pos = GetMousePosition();

			//resource
			int collide = false;
			for (shared_ptr<Resource> res : resources) {
				if (CheckCollisionPointCircle(mouse_pos, res->pos, res->r * 2)) {
					collide = true;
					res->type = ++res->type % MAX_TYPE;
				}
			}
			if (!collide) {
				resources.emplace_back(make_shared<Resource>(resource_id++, mouse_pos, rand() % NATURAL_TYPE));
			}
		}

		if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT) && selected_settlement != -1) {
			Vector2 mouse_pos = GetMousePosition();
			//target
			shared_ptr<Target> new_target = make_shared<Target>(target_id++, selected_tribe, &mouse_pos, 10, 2, 0, 0);
			targets.emplace_back(new_target);
			tribes[selected_tribe]->targets.emplace_back(new_target);

			// worker
			shared_ptr<Worker> new_worker = make_shared<Worker>(worker_id++, selected_tribe, mouse_pos, new_target, 1);
			new_worker->target.lock()->pos = &new_worker->pos;
			workers.emplace_back(new_worker);

			tribes[selected_tribe]->settlements[selected_settlement]->workers.emplace_back(new_worker);
	
		}

		if (IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE) && selected_settlement != -1) {
			Vector2 mouse_pos = GetMousePosition();

			// storage
			array<int, MAX_TYPE> limits = { 0 };
			limits.fill(STOCKPILE_CAPACITY);

			shared_ptr<Storage> new_storage = make_shared<Storage>(storage_id++, mouse_pos, 0, STOCKPILE_CAPACITY, limits, true);
			insertStorageWeak(storages, new_storage);

			//target
			shared_ptr<Target> new_target = make_shared<Target>(target_id++, selected_tribe, &mouse_pos, 10, 30, 0, 0);
			targets.emplace_back(new_target);
			tribes[selected_tribe]->targets.emplace_back(new_target);

			// stockpile
			shared_ptr<Stockpile> new_stockpile = make_shared<Stockpile>(stockpile_id++, selected_tribe, mouse_pos, 40.0f, weak_ptr<Construction>(), new_storage, new_target);
			stockpiles.emplace_back(new_stockpile);

			insertStorageShared(tribes[selected_tribe]->settlements[selected_settlement]->storages, new_storage);
			tribes[selected_tribe]->settlements[selected_settlement]->storages.emplace_back(new_storage);
			tribes[selected_tribe]->settlements[selected_settlement]->stockpiles.emplace_back(new_stockpile);
		}

		if (IsKeyReleased(KEY_C) && selected_settlement != -1) {
			Vector2 mouse_pos = GetMousePosition();

			//construction storage
			std::array<int, MAX_TYPE> limits = { 0 };
			limits[0] = 1;
			limits[1] = 1;
			limits[2] = 1;
			shared_ptr<Storage> new_storage = make_shared<Storage>(storage_id++, mouse_pos, 2, 3, limits, false);
			insertStorageWeak(storages, new_storage);
			insertStorageShared(tribes[selected_tribe]->settlements[selected_settlement]->storages, new_storage);

			//construction
			shared_ptr<Construction> new_construction = make_shared<Construction>(construction_id++, mouse_pos, new_storage, weak_ptr<Task>());
			constructions.emplace_back(new_construction);
			tribes[selected_tribe]->settlements[selected_settlement]->constructions.emplace_back(new_construction);

			//target
			shared_ptr<Target> new_target = make_shared<Target>(target_id++, selected_tribe, &mouse_pos, 10, 30, 0, 0);
			targets.emplace_back(new_target);
			tribes[selected_tribe]->targets.emplace_back(new_target);

			//stockpile
			shared_ptr<Stockpile> new_stockpile = make_shared<Stockpile>(stockpile_id++, selected_tribe, mouse_pos, 40.0f, new_construction, weak_ptr<Storage>(), new_target);
			stockpiles.emplace_back(new_stockpile);
			tribes[selected_tribe]->settlements[selected_settlement]->stockpiles.emplace_back(new_stockpile);
		}

		if (IsKeyReleased(KEY_F) && selected_settlement != -1) {
			Vector2 mouse_pos = GetMousePosition();

			//storage
			std::array<int, MAX_TYPE> limits = { 0 };
			limits[1] = 1;
			limits[2] = 1;
			shared_ptr<Storage> new_storage = make_shared<Storage>(stockpile_id++, mouse_pos, 1, 2, limits, true);
			insertStorageShared(tribes[selected_tribe]->settlements[selected_settlement]->storages, new_storage);
			insertStorageWeak(storages, new_storage);

			//forge
			shared_ptr<Forge> new_forge = make_shared<Forge>(forge_id++, mouse_pos, 40, new_storage);
			tribes[selected_tribe]->settlements[selected_settlement]->forges.emplace_back(new_forge);
			forges.emplace_back(new_forge);
		}

		if (IsKeyReleased(KEY_R) && selected_tribe != -1) {
			Vector2 mouse_pos = GetMousePosition();

			//target
			shared_ptr<Target> new_target = make_shared<Target>(target_id++, selected_tribe, &mouse_pos, 15, 10, 2, 3);
			targets.emplace_back(new_target);
			tribes[selected_tribe]->targets.emplace_back(new_target);

			//raider
			shared_ptr<Raider> new_raider = make_shared<Raider>(raider_id++, mouse_pos, new_target);
			new_raider->combat.lock()->pos = &new_raider->pos;
			raiders.emplace_back(new_raider);
			tribes[selected_tribe]->raiders.emplace_back(new_raider);
		}
		
		/*if (IsKeyReleased(KEY_BACKSPACE)) {
			raider_id = 0;
			construction_id = 0;
			stockpile_id = 0;
			resource_id = 0;
			worker_id = 0;
			generator_id = 0;
			forge_id = 0;

			storage_id = 0;
			task_id = 0;
			target_id = 0;

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

		for (shared_ptr<Tribe> tribe : tribes) {

			if (pause) {
				continue;
			}

			for (int target = 0; target < tribe->targets.size(); target++) {
				if (tribe->targets[target]->isDead() && !tribe->targets[target]->canAttack()) {
					tribe->targets.erase(tribe->targets.begin() + target);
					target--;
					continue;
				}
			}

			for (shared_ptr<Settlement> settlement : tribe->settlements) {

				for (int worker = 0; worker < settlement->workers.size(); worker++) {
					if (settlement->workers[worker]->target.expired()) {
						vector<int> types = settlement->workers[worker]->die();
						for (int type : types) {
							shared_ptr<Resource> new_resource = make_shared<Resource>(resource_id++, settlement->workers[worker]->pos, type);
							resources.emplace_back(new_resource);
						}
						settlement->workers.erase(settlement->workers.begin() + worker);
						worker--;
						continue;
					}
					settlement->workers[worker]->update(resources, settlement->storages, settlement->tasks);
				}

				for (int raider = 0; raider < tribe->raiders.size(); raider++) {
					if (tribe->raiders[raider]->combat.expired()) {
						tribe->raiders.erase(tribe->raiders.begin() + raider);
						raider--;
						continue;
					}
					tribe->raiders[raider]->update(targets);
				}

				for (int task = 0; task < settlement->tasks.size(); task++) {
					if (settlement->tasks[task]->finished && settlement->tasks[task]->hasWorkers() == 0) {
						settlement->tasks.erase(settlement->tasks.begin() + task);
						task--;
					}
				}

				for (int forge = 0; forge < settlement->forges.size(); forge++) {
					if (!settlement->forges[forge]->task.expired() && settlement->forges[forge]->task.lock()->isCompleted() && !settlement->forges[forge]->task.lock()->finished) {
						settlement->forges[forge]->storage.lock()->is = { 0 };
						settlement->forges[forge]->storage.lock()->will_be = { 0 };
						resources.emplace_back(make_shared<Resource>(resource_id++, Vector2Add(settlement->forges[forge]->pos, { (float)(rand() % 60 - 30),(float)(rand() % 60 - 30) }), 3));
						settlement->forges[forge]->task.lock()->finished = true;
					}

					if ((!settlement->forges[forge]->task.expired() && settlement->forges[forge]->storage.lock()->hasSpace(-1))) {
						settlement->forges[forge]->task.lock()->finished = true;
					}

					if (settlement->forges[forge]->task.expired() && settlement->forges[forge]->storage.lock()->isFull(-1)) {
						shared_ptr<Task> new_task = make_shared<Task>(task_id++, settlement->forges[forge]->pos, 2, 3.0f, 1);
						insertTaskShared(settlement->tasks, new_task);
						insertTaskWeak(tasks, new_task);
						settlement->forges[forge]->task = new_task;
					}
				}

				for (int c = 0; c < settlement->constructions.size(); c++) {
					if (!settlement->constructions[c]->task.expired() && settlement->constructions[c]->task.lock()->isCompleted()) {
						settlement->constructions[c]->task.lock()->finished = true;
						settlement->constructions.erase(settlement->constructions.begin() + c);
						c--;
						continue;
					}

					if (!settlement->constructions[c]->storage.expired() && settlement->constructions[c]->storage.lock()->isFull(-1) && !settlement->constructions[c]->is_all_delivered) {
						settlement->constructions[c]->is_all_delivered = true;
						settlement->constructions[c]->storage.lock()->remove = true;
						shared_ptr<Task> new_task = make_shared<Task>(task_id++, settlement->constructions[c]->pos, 1, 5.0f, 2);
						insertTaskShared(settlement->tasks, new_task);
						insertTaskWeak(tasks, new_task);
						settlement->constructions[c]->task = new_task;
					}
				}

				for (int storage = 0; storage < settlement->storages.size(); storage++) {
					if (settlement->storages[storage]->destroy) {
						for (shared_ptr<Worker> worker : settlement->workers) {
							worker->forgetStorage(settlement->storages[storage], resources);
						}

						for (int i = 0; i < MAX_TYPE; i++) {
							for (int j = 0; j < settlement->storages[storage]->is[i]; j++) {
								shared_ptr<Resource> new_resource = make_shared<Resource>(resource_id++, settlement->storages[storage]->pos, i);
								resources.emplace_back(new_resource);
							}
						}
						settlement->storages[storage]->remove = true;
					}

					if (settlement->storages[storage]->remove) {
						settlement->storages.erase(settlement->storages.begin() + storage);
						storage--;
					}
				}

				for (int stockpile = 0; stockpile < settlement->stockpiles.size(); stockpile++) {
					if (settlement->stockpiles[stockpile]->target.expired()) {
						if (settlement->stockpiles[stockpile]->construction.expired()) {
							settlement->stockpiles[stockpile]->storage.lock()->destroy = true;
						}
						else if (!settlement->stockpiles[stockpile]->construction.lock()->storage.expired()) {
							settlement->stockpiles[stockpile]->construction.lock()->storage.lock()->destroy = true;
							//delete construction task
						}

						settlement->stockpiles.erase(settlement->stockpiles.begin() + stockpile);
						stockpile--;
						continue;
					}

					if (settlement->stockpiles[stockpile]->construction.expired() && settlement->stockpiles[stockpile]->storage.expired()) {
						array<int, MAX_TYPE> limits = { 0 };
						limits.fill(STOCKPILE_CAPACITY);

						shared_ptr<Storage> new_storage = make_shared<Storage>(storage_id++, settlement->stockpiles[stockpile]->pos, 0, STOCKPILE_CAPACITY, limits, true);
						insertStorageWeak(storages, new_storage);
						insertStorageShared(settlement->storages, new_storage);
						settlement->stockpiles[stockpile]->storage = new_storage;
					}
				}
			}
		}

		BeginDrawing();
		ClearBackground(BLACK);

		if (selected_tribe != -1) {
			DrawText(to_string(selected_tribe).c_str(), 0, 0, 20, tribe_color[selected_tribe]);
			if (selected_settlement != -1) {
				DrawText(to_string(selected_settlement).c_str(), 0, 21, 20, WHITE);
			}
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

		

		/*for (int f = 0; f < forges.size(); f++) {
			
			forges[forge]->draw();
		}*/

		std::erase_if(tasks, [](weak_ptr<Task> t) {return t.expired(); });

		std::erase_if(storages, [](weak_ptr<Storage> s) {return s.expired(); });

		std::erase_if(constructions, [](weak_ptr<Construction> c) {return c.expired(); });

		std::erase_if(targets, [](weak_ptr<Target> t) {return t.expired(); });

		std::erase_if(forges, [](weak_ptr<Forge> f) {return f.expired(); });
		for (weak_ptr<Forge> f : forges) {
			f.lock()->draw();
		}

		std::erase_if(stockpiles, [](weak_ptr<Stockpile> s) {return s.expired(); });
		for (weak_ptr<Stockpile> s : stockpiles) {
			s.lock()->draw();
		}

		std::erase_if(raiders, [](weak_ptr<Raider> r) {return r.expired(); });
		for (weak_ptr<Raider> raider : raiders) {
			raider.lock()->draw();
		}

		std::erase_if(resources, [](shared_ptr<Resource> r) {return r->taken; });
		for (shared_ptr<Resource> resource : resources) {
				resource->draw();
		}

		std::erase_if(workers, [](weak_ptr<Worker> w) {return w.expired(); });
		for (weak_ptr<Worker> worker : workers) {
			worker.lock()->draw();
		}

		EndDrawing();
	}
	CloseWindow();
	return 0;
}

// kubino<3astrid weeeeeeeeeeeeeee /(- 3-)/  