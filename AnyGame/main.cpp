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
createNode(array<int,MAX_TYPE> types, Vector2 pos) {
	shared_ptr<float> r = make_shared<float>(NODE_R);

	shared_ptr<Storage> new_storage = make_shared<Storage>(storage_id++, -1, pos, r, 0, resourceCount(types), types, true);

	new_storage->is = types;
	new_storage->will_be = types;

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
	shared_ptr<Target> new_target = make_shared<Target>(target_id++, selected_tribe, nullptr, 10.0f, 30, 0, 0);

	//stockpile
	shared_ptr<Stockpile> new_stockpile = make_shared<Stockpile>(2,stockpile_id++, selected_tribe, pos, r, new_construction, weak_ptr<Storage>(), new_target);
	new_stockpile->target.lock()->pos = &new_stockpile->pos;
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
	shared_ptr<Target> new_target = make_shared<Target>(target_id++, selected_tribe, nullptr, 10, 30, 0, 0);

	//forge
	shared_ptr<Forge> new_forge = make_shared<Forge>(3,forge_id++, selected_tribe, pos, r, recipe, produce, new_construction, new_target);
	new_forge->target.lock()->pos = &new_forge->pos;
	return make_tuple(new_storage, new_construction, new_target, new_forge);
}

int main() {

	bool pause = false;

	srand((unsigned int)time(nullptr)); // time based seed for RNG


	vector<shared_ptr<Storage>> storages;
	vector<shared_ptr<Node>> nodes;
	vector<shared_ptr<Tribe>> tribes;
	vector<weak_ptr<Drawing>> drawings;

	vector<weak_ptr<Target>> targets;

	//vector<weak_ptr<Task>> tasks;

	

	//vector<weak_ptr<Construction>> constructions;

	vector<weak_ptr<Generator>> generators;

	//vector<weak_ptr<Raider>> raiders;

	//vector<weak_ptr<Settlement>> settlements;

	
	int type_cnt = 0;
	array<int,MAX_TYPE> node_types = {};

	array<int, MAX_TYPE> forge_recipe_types = {};
	array<int, MAX_TYPE> forge_produce_types = {};

	bool recipe_or_produce = false;

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

		if (IsKeyReleased(KEY_ENTER) && resourceCount(forge_recipe_types) > 0) {
			recipe_or_produce = true;
		}

		if (IsKeyReleased(KEY_ONE)) {
			node_types[0]++;
			type_cnt++;

			if (!recipe_or_produce) {
				forge_recipe_types[0]++;
			}
			else {
				forge_produce_types[0]++;
			}
		}
		if (IsKeyReleased(KEY_TWO)) {
			node_types[1]++;
			type_cnt++;

			if (!recipe_or_produce) {
				forge_recipe_types[1]++;
			}
			else {
				forge_produce_types[1]++;
			}
		}
		if (IsKeyReleased(KEY_THREE)) {
			node_types[2]++;
			type_cnt++;

			if (!recipe_or_produce) {
				forge_recipe_types[2]++;
			}
			else {
				forge_produce_types[2]++;
			}
		}
		if (IsKeyReleased(KEY_FOUR)) {
			node_types[3]++;
			type_cnt++;

			if (!recipe_or_produce) {
				forge_recipe_types[3]++;
			}
			else {
				forge_produce_types[3]++;
			}
		}

		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && type_cnt > 0) {
			tuple<shared_ptr<Storage>, shared_ptr<Node>> result = createNode(node_types, GetMousePosition());
			insertStorageShared(storages, get<0>(result));
			//insertStorageWeak(storages, get<0>(result));
			nodes.emplace_back(get<1>(result));


			shared_ptr<Drawing> node = get<1>(result);
			drawings.push_back(node);
			node_types.fill(0);
			forge_recipe_types.fill(0);
			forge_produce_types.fill(0);
			recipe_or_produce = false;
			type_cnt = 0;
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
			targets.emplace_back(get<2>(result));
			tribes[selected_tribe]->settlements[selected_settlement]->stockpiles.emplace_back(get<3>(result));
			drawings.emplace_back(get<3>(result));
		}

		if (IsKeyReleased(KEY_F) && selected_settlement != -1 && resourceCount(forge_recipe_types) > 0 && resourceCount(forge_produce_types) > 0) {
			array<int, MAX_TYPE> recipe = {};
			array<int, MAX_TYPE> produce = {};
			

			tuple<shared_ptr<Storage>, shared_ptr<Construction>, shared_ptr<Target>, shared_ptr<Forge>> result = createForge(forge_recipe_types, forge_produce_types, GetMousePosition());
			insertStorageShared(storages, get<0>(result));
			//insertStorageWeak(storages, get<0>(result));
			tribes[selected_tribe]->settlements[selected_settlement]->constructions.emplace_back(get<1>(result));
			tribes[selected_tribe]->targets.emplace_back(get<2>(result));
			tribes[selected_tribe]->settlements[selected_settlement]->forges.emplace_back(get<3>(result));
			drawings.emplace_back(get<3>(result));

			node_types.fill(0);
			forge_recipe_types.fill(0);
			forge_produce_types.fill(0);
			recipe_or_produce = false;
			type_cnt = 0;
		}

		if (IsKeyReleased(KEY_R) && selected_tribe != -1) {
			Vector2 mouse_pos = GetMousePosition();

			//target
			shared_ptr<Target> new_target = make_shared<Target>(target_id++, selected_tribe, &mouse_pos, 15, 10, 2, 3);
			targets.emplace_back(new_target);
			tribes[selected_tribe]->targets.emplace_back(new_target);

			//raider
			shared_ptr<Raider> new_raider = make_shared<Raider>(2, raider_id++, mouse_pos, new_target);
			new_raider->target.lock()->pos = &new_raider->pos;
			tribes[selected_tribe]->raiders.emplace_back(new_raider);
			drawings.emplace_back(new_raider);
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

			for (int raider = 0; raider < tribe->raiders.size(); raider++) {
				if (!tribe->raiders[raider]->target.expired() && tribe->raiders[raider]->target.lock()->isDead()) {

					//delete target


					tribe->raiders.erase(tribe->raiders.begin() + raider);
					raider--;
					continue;
				}
				tribe->raiders[raider]->update(targets);
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
						Vector2 pos = settlement->workers[worker]->pos;
						settlement->workers.erase(settlement->workers.begin() + worker);
						worker--;

						tuple<shared_ptr<Storage>,shared_ptr<Node>> result = createNode(arrangeTypes(types), pos);
						insertStorageShared(storages, get<0>(result));
						nodes.emplace_back(get<1>(result));

						
						continue;
					}
					settlement->workers[worker]->update(storages, settlement->tasks);
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
						settlement->constructions[c]->task = new_task;
					}
				}

				for (int stockpile = 0; stockpile < settlement->stockpiles.size(); stockpile++) {
					if (settlement->stockpiles[stockpile]->target.expired() && settlement->stockpiles[stockpile]->destroyable) {
						if (settlement->stockpiles[stockpile]->construction.expired()) {
							for (shared_ptr<Worker> worker : settlement->workers) {
								worker->forgetStorage(settlement->stockpiles[stockpile]->storage.lock());
							}

							tuple<shared_ptr<Storage>, shared_ptr<Node>> result = createNode(settlement->stockpiles[stockpile]->storage.lock()->is, settlement->stockpiles[stockpile]->pos);
							insertStorageShared(storages, get<0>(result));
							nodes.emplace_back(get<1>(result));

							int id = settlement->stockpiles[stockpile]->storage.lock()->id;
							erase_if(storages, [id](shared_ptr<Storage> s) {return s->id == id; });
						}
						else if (!settlement->stockpiles[stockpile]->construction.lock()->storage.expired()) {
							for (shared_ptr<Worker> worker : settlement->workers) {
								worker->forgetStorage(settlement->stockpiles[stockpile]->construction.lock()->storage.lock());
							}

							tuple<shared_ptr<Storage>, shared_ptr<Node>> result = createNode(settlement->stockpiles[stockpile]->construction.lock()->storage.lock()->is, settlement->stockpiles[stockpile]->pos);
							insertStorageShared(storages, get<0>(result));
							nodes.emplace_back(get<1>(result));

							int id = settlement->stockpiles[stockpile]->construction.lock()->storage.lock()->id;
							erase_if(storages, [id](shared_ptr<Storage> s) {return s->id == id; });
						}

						else if (!settlement->stockpiles[stockpile]->construction.lock()->task.expired()) {
							int id = settlement->stockpiles[stockpile]->construction.lock()->task.lock()->id;
							erase_if(settlement->tasks, [id](shared_ptr<Task> t) {return t->id == id; });
						}

						settlement->stockpiles.erase(settlement->stockpiles.begin() + stockpile);
						stockpile--;
						continue;
					}

					if (settlement->stockpiles[stockpile]->construction.expired() && settlement->stockpiles[stockpile]->storage.expired() && settlement->stockpiles[stockpile]->destroyable) {
						array<int, MAX_TYPE> limits = { 0 };
						limits.fill(STOCKPILE_CAPACITY);

						shared_ptr<Storage> new_storage = make_shared<Storage>(storage_id++, settlement->stockpiles[stockpile]->tribe, settlement->stockpiles[stockpile]->pos, settlement->stockpiles[stockpile]->r, 1, STOCKPILE_CAPACITY, limits, true);
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

					if (settlement->forges[forge]->construction.expired() && settlement->forges[forge]->storage_in.expired()) {
						shared_ptr<Storage> new_storage_in = make_shared<Storage>(stockpile_id++, settlement->forges[forge]->tribe, settlement->forges[forge]->pos, settlement->forges[forge]->r, 2,resourceCount(settlement->forges[forge]->recipe), settlement->forges[forge]->recipe, true);
						insertStorageShared(storages, new_storage_in);
						
						shared_ptr<Storage> new_storage_out = make_shared<Storage>(stockpile_id++, settlement->forges[forge]->tribe, settlement->forges[forge]->pos, settlement->forges[forge]->r, 0, resourceCount(settlement->forges[forge]->produce), settlement->forges[forge]->produce, true);
						insertStorageShared(storages, new_storage_out);


						settlement->forges[forge]->storage_in = new_storage_in;
						settlement->forges[forge]->storage_out = new_storage_out;
					}

					if (!settlement->forges[forge]->task.expired() && settlement->forges[forge]->task.lock()->isCompleted()) {
						settlement->forges[forge]->storage_in.lock()->is = { 0 };
						settlement->forges[forge]->storage_in.lock()->will_be = { 0 };

						addToStorage(settlement->forges[forge]->storage_out, settlement->forges[forge]->produce);

						int id = settlement->forges[forge]->task.lock()->id;
						erase_if(settlement->tasks, [id](shared_ptr<Task> t) {return t->id == id; });
					}

					if ((!settlement->forges[forge]->task.expired() && settlement->forges[forge]->storage_in.lock()->hasSpace(-1))) {
						int id = settlement->forges[forge]->task.lock()->id;
						erase_if(settlement->tasks, [id](shared_ptr<Task> t) {return t->id == id; });
					}

					if (settlement->forges[forge]->task.expired() && !settlement->forges[forge]->storage_in.expired() && settlement->forges[forge]->storage_in.lock()->isFull(-1) &&
						settlement->forges[forge]->storage_out.lock()->capacity >= settlement->forges[forge]->storage_out.lock()->isStored() + resourceCount(settlement->forges[forge]->produce)) {
						shared_ptr<Task> new_task = make_shared<Task>(task_id++, settlement->forges[forge]->pos, 2, 3.0f, 1);
						insertTaskShared(settlement->tasks, new_task);
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

		std::erase_if(storages, [](weak_ptr<Storage> s) {return s.expired(); });

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
// milujem ta <3