#pragma once
#include "any_game.hpp"

#include "Storage.hpp"
#include "Task.hpp"
#include "Target.hpp"

#include "Drawing.hpp"

#include "Raider.hpp"
#include "Construction.hpp"
#include "Stockpile.hpp"
#include "Generator.hpp"
#include "Forge.hpp"
#include "Worker.hpp"
#include "Node.hpp"

#include "Tribe.hpp"
#include "Settlement.hpp"

#include <ctime>
#include <iostream>
#include <string>
#include <tuple>
#include <algorithm>



tuple<shared_ptr<Storage>,shared_ptr<Node>>
createNode(vector<int> types, Vector2 pos) {
	shared_ptr<float> r = make_shared<float>(NODE_R);
	array<int, MAX_TYPE> limits = { 0 };

	shared_ptr<Storage> new_storage = make_shared<Storage>(storage_id++, -1, pos, r, 0, (int)types.size(), limits, true);

	for (int i : types) {
		new_storage->is[i]++;
		new_storage->will_be[i]++;
		new_storage->can_be[i]++;
	}

	shared_ptr<Node> new_node = make_shared<Node>(1,node_id++,pos, r, new_storage);
	
	return make_tuple(new_storage, new_node);
}

tuple<shared_ptr<Storage>, shared_ptr<Construction>, shared_ptr<Target>, shared_ptr<Stockpile>> 
createStockpile(Vector2 pos) {
	shared_ptr<float> r = make_shared<float>(STOCKPILE_R);
	//construction storage
	std::array<int, MAX_TYPE> limits = { 0 };
	limits[0] = 2;
	shared_ptr<Storage> new_storage = make_shared<Storage>(storage_id++, selected_tribe, pos, r, 3, 2, limits, false);

	//construction
	shared_ptr<Construction> new_construction = make_shared<Construction>(construction_id++, pos, new_storage, weak_ptr<Task>());

	//target
	shared_ptr<Target> new_target = make_shared<Target>(target_id++, selected_tribe, &pos, 10.0f, 30, 0, 0);

	//stockpile
	shared_ptr<Stockpile> new_stockpile = make_shared<Stockpile>(2,stockpile_id++, selected_tribe, pos, r, new_construction, weak_ptr<Storage>(), new_target);
	return make_tuple(new_storage, new_construction, new_target, new_stockpile);

}

//tuple<shared_ptr<Storage>, shared_ptr<Task>, shared_ptr<Generator>>
//createGenerator(Vector2 pos) {
//
//}

tuple<shared_ptr<Storage>, shared_ptr<Construction>, shared_ptr<Target>, shared_ptr<Forge>> 
createForge(array<int, MAX_TYPE> recipe, array<int, MAX_TYPE> produce, Vector2 pos) {
	shared_ptr<float> r = make_shared<float>(FORGE_R);
	//construction storage
	std::array<int, MAX_TYPE> limits = { 0 };
	limits[1] = 1;
	limits[2] = 1;
	shared_ptr<Storage> new_storage = make_shared<Storage>(storage_id++, selected_tribe, pos, r, 3, 2, limits, false);

	//construction
	shared_ptr<Construction> new_construction = make_shared<Construction>(construction_id++, pos, new_storage, weak_ptr<Task>());

	//target
	shared_ptr<Target> new_target = make_shared<Target>(target_id++, selected_tribe, &pos, 10, 30, 0, 0);

	//forge
	shared_ptr<Forge> new_forge = make_shared<Forge>(3,forge_id++, selected_tribe, pos, r, recipe, produce, new_construction, new_target);
	return make_tuple(new_storage, new_construction, new_target, new_forge);
}

int main() {

	bool pause = false;

	srand((unsigned int)time(nullptr)); // time based seed for RNG


	vector<shared_ptr<Storage>> storages;
	vector<shared_ptr<Node>> nodes;
	vector<shared_ptr<Tribe>> tribes;
	vector<weak_ptr<Drawing>> drawings;



	//vector<weak_ptr<Task>> tasks;

	vector<weak_ptr<Target>> targets;

	//vector<weak_ptr<Construction>> constructions;

	vector<weak_ptr<Generator>> generators;

	vector<weak_ptr<Raider>> raiders;

	//vector<weak_ptr<Settlement>> settlements;

	

	vector<int> create_resource_types = {};
	int last_recipe_index = -1;

	for (int i = 0; i < MAX_TRIBE; i++) {
		shared_ptr<Settlement> new_settlement = make_shared<Settlement>(settlement_id++, Vector2{ 0,0 }, 1.0f);
		tribes.emplace_back(make_shared<Tribe>(tribe_id++, new_settlement));
	}

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

		if (IsKeyReleased(KEY_ENTER) && !create_resource_types.empty()) {
			last_recipe_index = (int)create_resource_types.size();
		}

		if (IsKeyReleased(KEY_ONE)) {
			auto pos = lower_bound(create_resource_types.begin(), create_resource_types.end(), 0);
			create_resource_types.insert(pos,0);
		}
		if (IsKeyReleased(KEY_TWO)) {
			auto pos = lower_bound(create_resource_types.begin(), create_resource_types.end(), 1);
			create_resource_types.insert(pos, 1);
		}
		if (IsKeyReleased(KEY_THREE)) {
			auto pos = lower_bound(create_resource_types.begin(), create_resource_types.end(), 2);
			create_resource_types.insert(pos, 2);
		}
		if (IsKeyReleased(KEY_FOUR)) {
			auto pos = lower_bound(create_resource_types.begin(), create_resource_types.end(), 3);
			create_resource_types.insert(pos, 3);
		}

		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && !create_resource_types.empty()) {
			tuple<shared_ptr<Storage>, shared_ptr<Node>> result = createNode(create_resource_types, GetMousePosition());
			insertStorageShared(storages, get<0>(result));
			//insertStorageWeak(storages, get<0>(result));
			nodes.emplace_back(get<1>(result));


			shared_ptr<Drawing> node = get<1>(result);
			drawings.push_back(node);
			create_resource_types.clear();
			last_recipe_index = -1;
		}

		if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT) && selected_settlement != -1) {
			Vector2 mouse_pos = GetMousePosition();
			//target
			shared_ptr<Target> new_target = make_shared<Target>(target_id++, selected_tribe, &mouse_pos, 10, 2, 0, 0);
			targets.emplace_back(new_target);
			tribes[selected_tribe]->targets.emplace_back(new_target);

			// worker
			shared_ptr<Worker> new_worker = make_shared<Worker>(4,worker_id++, selected_tribe, mouse_pos, new_target);
			new_worker->target.lock()->pos = &new_worker->pos;
			drawings.emplace_back(new_worker);

			tribes[selected_tribe]->settlements[selected_settlement]->workers.emplace_back(new_worker);

		}

		if (IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE) && selected_settlement != -1) {
			tuple<shared_ptr<Storage>, shared_ptr<Construction>, shared_ptr<Target>, shared_ptr<Stockpile>>
				result = createStockpile(GetMousePosition());

			insertStorageShared(storages, get<0>(result));
			//insertStorageWeak(storages, get<0>(result));

			tribes[selected_tribe]->settlements[selected_settlement]->constructions.emplace_back(get<1>(result));
			tribes[selected_tribe]->targets.emplace_back(get<2>(result));
			tribes[selected_tribe]->settlements[selected_settlement]->stockpiles.emplace_back(get<3>(result));
			drawings.emplace_back(get<3>(result));
		}

		if (IsKeyReleased(KEY_F) && selected_settlement != -1 && last_recipe_index < create_resource_types.size()) {
			array<int, MAX_TYPE> recipe = {};
			array<int, MAX_TYPE> produce = {};

			for (int i = 0; i < create_resource_types.size(); i++) {
				if (i < last_recipe_index) {
					recipe[create_resource_types[i]]++;
				}
				else {
					produce[create_resource_types[i]]++;
				}
			}
			create_resource_types.clear();

			tuple<shared_ptr<Storage>, shared_ptr<Construction>, shared_ptr<Target>, shared_ptr<Forge>> result = createForge(recipe, produce, GetMousePosition());
			insertStorageShared(storages, get<0>(result));
			//insertStorageWeak(storages, get<0>(result));

			tribes[selected_tribe]->settlements[selected_settlement]->constructions.emplace_back(get<1>(result));
			tribes[selected_tribe]->targets.emplace_back(get<2>(result));
			tribes[selected_tribe]->settlements[selected_settlement]->forges.emplace_back(get<3>(result));
			drawings.emplace_back(get<3>(result));
		}

		//if (IsKeyReleased(KEY_R) && selected_tribe != -1) {
		//	Vector2 mouse_pos = GetMousePosition();

		//	//target
		//	shared_ptr<Target> new_target = make_shared<Target>(target_id++, selected_tribe, &mouse_pos, 15, 10, 2, 3);
		//	targets.emplace_back(new_target);
		//	tribes[selected_tribe]->targets.emplace_back(new_target);

		//	//raider
		//	shared_ptr<Raider> new_raider = make_shared<Raider>(raider_id++, mouse_pos, new_target);
		//	new_raider->target.lock()->pos = &new_raider->pos;
		//	raiders.emplace_back(new_raider);
		//	tribes[selected_tribe]->raiders.emplace_back(new_raider);
		//}

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
					cout << "here\n";
					tribe->targets.erase(tribe->targets.begin() + target);
					target--;
					continue;
				}
			}

			for (shared_ptr<Settlement> settlement : tribe->settlements) {

				for (int worker = 0; worker < settlement->workers.size(); worker++) {
					/*if (settlement->workers[worker]->target.expired()) {
						vector<int> types = settlement->workers[worker]->die();
						for (int type : types) {
							shared_ptr<Resource> new_resource = make_shared<Resource>(resource_id++, settlement->workers[worker]->pos, type);
							resources.emplace_back(new_resource);
						}
						settlement->workers.erase(settlement->workers.begin() + worker);
						worker--;
						continue;
					}*/
					settlement->workers[worker]->update(storages, settlement->tasks);
				}

				for (int raider = 0; raider < tribe->raiders.size(); raider++) {
					if (tribe->raiders[raider]->target.expired()) {
						tribe->raiders.erase(tribe->raiders.begin() + raider);
						raider--;
						continue;
					}
					tribe->raiders[raider]->update(targets);
				}

				/*for (int task = 0; task < settlement->tasks.size(); task++) {
					if (settlement->tasks[task]->finished && settlement->tasks[task]->hasWorkers() == 0) {
						settlement->tasks[task]->destroy = true;
					}

					if (settlement->tasks[task]->destroy) {
						settlement->tasks.erase(settlement->tasks.begin() + task);
						task--;
					}

				}*/

				for (int c = 0; c < settlement->constructions.size(); c++) {
					if (!settlement->constructions[c]->task.expired() && settlement->constructions[c]->task.lock()->isCompleted()) {
						//settlement->constructions[c]->task.lock()->finished = true;
						settlement->constructions.erase(settlement->constructions.begin() + c);
						c--;
						continue;
					}

					if (!settlement->constructions[c]->storage.expired() && settlement->constructions[c]->storage.lock()->isFull(-1) && !settlement->constructions[c]->is_all_delivered) {
						settlement->constructions[c]->is_all_delivered = true;
						shared_ptr<Task> new_task = make_shared<Task>(task_id++, settlement->constructions[c]->pos, 1, 5.0f, 2);
						insertTaskShared(settlement->tasks, new_task);
						//insertTaskWeak(tasks, new_task);
						settlement->constructions[c]->task = new_task;
					}
				}

				//for (int storage = 0; storage < settlement->storages.size(); storage++) {

				//	if (settlement->storages[storage]->destroy) {
				//		for (shared_ptr<Worker> worker : settlement->workers) {
				//			worker->forgetStorage(settlement->storages[storage], resources);
				//		}

				//		for (int i = 0; i < MAX_TYPE; i++) {
				//			/*for (int j = 0; j < settlement->storages[storage]->is[i]; j++) {
				//				shared_ptr<Resource> new_resource = make_shared<Resource>(resource_id++, settlement->storages[storage]->pos, i);
				//				resources.emplace_back(new_resource);
				//			}*/
				//		}
				//		settlement->storages[storage]->remove = true;
				//	}

				//	if (settlement->storages[storage]->remove) {
				//		settlement->storages.erase(settlement->storages.begin() + storage);
				//		storage--;
				//	}
				//}

				for (int stockpile = 0; stockpile < settlement->stockpiles.size(); stockpile++) {
					if (settlement->stockpiles[stockpile]->target.expired() && settlement->stockpiles[stockpile]->destroyable) {
						if (settlement->stockpiles[stockpile]->construction.expired()) {
							//settlement->stockpiles[stockpile]->storage.lock()->destroy = true;
						}
						else if (!settlement->stockpiles[stockpile]->construction.lock()->storage.expired()) {
							//settlement->stockpiles[stockpile]->construction.lock()->storage.lock()->destroy = true;
						}

						else if (!settlement->stockpiles[stockpile]->construction.lock()->task.expired()) {
							//settlement->stockpiles[stockpile]->construction.lock()->task.lock()->destroy = true;
						}

						settlement->stockpiles.erase(settlement->stockpiles.begin() + stockpile);
						stockpile--;
						continue;
					}

					if (settlement->stockpiles[stockpile]->construction.expired() && settlement->stockpiles[stockpile]->storage.expired() && settlement->stockpiles[stockpile]->destroyable) {
						array<int, MAX_TYPE> limits = { 0 };
						limits.fill(STOCKPILE_CAPACITY);

						shared_ptr<Storage> new_storage = make_shared<Storage>(storage_id++, settlement->stockpiles[stockpile]->tribe, settlement->stockpiles[stockpile]->pos, settlement->stockpiles[stockpile]->r, 1, STOCKPILE_CAPACITY, limits, true);
						/*insertStorageWeak(storages, new_storage);*/
						insertStorageShared(storages, new_storage);
						settlement->stockpiles[stockpile]->storage = new_storage;

					}
				}

				for (int forge = 0; forge < settlement->forges.size(); forge++) {

					if (settlement->forges[forge]->target.expired()) {
						if (settlement->forges[forge]->construction.expired()) {
							//settlement->forges[forge]->storage.lock()->destroy = true;
							if (!settlement->forges[forge]->task.expired()) {
							//	settlement->forges[forge]->task.lock()->destroy = true;
							}
						}
						else if (!settlement->forges[forge]->construction.lock()->storage.expired()) {
							//settlement->forges[forge]->construction.lock()->storage.lock()->destroy = true;

						}
						else if (!settlement->forges[forge]->construction.lock()->task.expired()) {
							//settlement->forges[forge]->construction.lock()->task.lock()->destroy = true;
						}

						settlement->forges.erase(settlement->forges.begin() + forge);
						forge--;
						continue;
					}

					if (settlement->forges[forge]->construction.expired() && settlement->forges[forge]->storage.expired()) {
						array<int, MAX_TYPE> limits = { 0 };
						shared_ptr<Storage> new_storage = make_shared<Storage>(stockpile_id++, settlement->forges[forge]->tribe, settlement->forges[forge]->pos, settlement->forges[forge]->r, 2, 2, settlement->forges[forge]->recipe, true);
						insertStorageShared(storages, new_storage);
						/*insertStorageWeak(storages, new_storage);*/
						settlement->forges[forge]->storage = new_storage;
					}

					//if (!settlement->forges[forge]->task.expired() && settlement->forges[forge]->task.lock()->isCompleted() && !settlement->forges[forge]->task.lock()->finished) {
					//	/*settlement->forges[forge]->storage.lock()->is = { 0 };
					//	settlement->forges[forge]->storage.lock()->will_be = { 0 };
					//	resources.emplace_back(make_shared<Resource>(resource_id++, Vector2Add(settlement->forges[forge]->pos, { (float)(rand() % 60 - 30),(float)(rand() % 60 - 30) }), 3));
					//	settlement->forges[forge]->task.lock()->finished = true;*/
					//}

					/*if ((!settlement->forges[forge]->task.expired() && settlement->forges[forge]->storage.lock()->hasSpace(-1))) {
						settlement->forges[forge]->task.lock()->finished = true;
					}*/

					if (settlement->forges[forge]->task.expired() && !settlement->forges[forge]->storage.expired() && settlement->forges[forge]->storage.lock()->isFull(-1)) {
						shared_ptr<Task> new_task = make_shared<Task>(task_id++, settlement->forges[forge]->pos, 2, 3.0f, 1);
						insertTaskShared(settlement->tasks, new_task);
						//insertTaskWeak(tasks, new_task);
						settlement->forges[forge]->task = new_task;
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

		for (shared_ptr<Node> node : nodes) {
			node->update();
		}
		
		//std::erase_if(tasks, [](weak_ptr<Task> t) {return t.expired(); });

		std::erase_if(storages, [](weak_ptr<Storage> s) {return s.expired(); });

		//std::erase_if(constructions, [](weak_ptr<Construction> c) {return c.expired(); });

		std::erase_if(targets, [](weak_ptr<Target> t) {return t.expired(); });


		std::erase_if(drawings, [](weak_ptr<Drawing> d) {return d.expired(); });
		std::sort(drawings.begin(), drawings.end(), [](weak_ptr<Drawing> left, weak_ptr<Drawing> right) {return left.lock()->z < right.lock()->z; });


		for (weak_ptr<Drawing> drawing : drawings) {
			drawing.lock()->draw();
		}

		EndDrawing();
	}
	CloseWindow();
	return 0;
}

// kubino<3astrid weeeeeeeeeeeeeee /(- 3-)/  