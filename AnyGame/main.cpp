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

	shared_ptr<Node> new_node = make_shared<Node>(DEPTH::NODE,node_id++,pos, r, new_storage);
	
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
	shared_ptr<Stockpile> new_stockpile = make_shared<Stockpile>(DEPTH::STOCKPILE,stockpile_id++, selected_tribe, pos, r, new_construction, weak_ptr<Storage>(), new_target);
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
	shared_ptr<Forge> new_forge = make_shared<Forge>(DEPTH::FORGE,forge_id++, selected_tribe, pos, r, recipe, produce, new_construction, new_target);
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
			shared_ptr<Worker> new_worker = make_shared<Worker>(DEPTH::WORKER,worker_id++, selected_tribe, mouse_pos, new_target);
			new_worker->target.lock()->pos = &new_worker->pos;
			drawings.emplace_back(new_worker);

			tribes[selected_tribe]->settlements[selected_settlement]->workers.emplace_back(new_worker);

		}

		if (IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE) && selected_settlement != -1) {
			tuple<shared_ptr<Storage>, shared_ptr<Construction>, shared_ptr<Target>, shared_ptr<Stockpile>>
				result = createStockpile(GetMousePosition());

			insertStorageShared(storages, get<0>(result));

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
			tribes[selected_tribe]->settlements[selected_settlement]->constructions.emplace_back(get<1>(result));
			tribes[selected_tribe]->targets.emplace_back(get<2>(result));
			targets.emplace_back(targets.emplace_back(get<2>(result)));
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
			shared_ptr<Raider> new_raider = make_shared<Raider>(DEPTH::RAIDER, raider_id++, mouse_pos, new_target);
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
					int id = tribe->raiders[raider]->target.lock()->id;
					erase_if(tribe->targets, [id](shared_ptr<Target> t) {return t->id == id; });

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
						drawings.emplace_back(get<1>(result));

						
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

				//Kubincko 

				/*for (int c = 0; c < settlement->constructions.size(); c++) {
					if (!settlement->constructions[c]->task.expired() && settlement->constructions[c]->task.lock()->isCompleted()) {
						int id = settlement->constructions[c]->task.lock()->id;
						erase_if(settlement->tasks, [id](shared_ptr<Task> t) {return t->id == id; });
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
				}*/

				for (int stockpile_i = 0; stockpile_i < settlement->stockpiles.size(); stockpile_i++) {
					shared_ptr<Stockpile> stockpile = settlement->stockpiles[stockpile_i];
					if (!stockpile->construction.expired()) {
						shared_ptr<Construction> construction = stockpile->construction.lock();
						if (stockpile->target.expired()) {
							if (!construction->storage.expired()) {
								for (shared_ptr<Worker> worker : settlement->workers) {
									array<int,MAX_TYPE> types = arrangeTypes(worker->forgetStorage(construction->storage.lock()));
									if (!types.empty()) {
										tuple<shared_ptr<Storage>, shared_ptr<Node>> result = createNode(types, worker->pos);
										insertStorageShared(storages, get<0>(result));
										nodes.emplace_back(get<1>(result));
										drawings.emplace_back(get<1>(result));
									}
								}

								if (!stockpile->construction.lock()->storage.lock()->isEmpty()) {
									tuple<shared_ptr<Storage>, shared_ptr<Node>> result = createNode(construction->storage.lock()->is, stockpile->pos);
									insertStorageShared(storages, get<0>(result));
									nodes.emplace_back(get<1>(result));
									drawings.emplace_back(get<1>(result));
								}

								int id = construction->storage.lock()->id;
								erase_if(storages, [id](shared_ptr<Storage> s) {; return s->id == id; });
							}

							else if (!construction->task.expired()) {
								int id = construction->task.lock()->id;
								erase_if(settlement->tasks, [id](shared_ptr<Task> t) {return t->id == id; });
							}

							settlement->stockpiles.erase(settlement->stockpiles.begin() + stockpile_i);
							stockpile_i--;
							continue;
						}
						if (!construction->storage.expired()) {
							shared_ptr<Storage> storage = construction->storage.lock();
							if (storage->isFull(-1)) {
								int id = storage->id;
								erase_if(storages, [id](shared_ptr<Storage> s) {return s->id == id; });

								shared_ptr<Task> new_task = make_shared<Task>(task_id++, construction->pos, 1, 5.0f, 2);
								insertTaskShared(settlement->tasks, new_task);
								construction->task = new_task;
							}
						}

						if (!construction->task.expired() && construction->task.lock()->isCompleted()) {
							int id = construction->task.lock()->id;
							erase_if(settlement->tasks, [id](shared_ptr<Task> t) {return t->id == id; });
							id = construction->id;
							erase_if(settlement->constructions, [id](shared_ptr<Construction> c) {return c->id == id; });
							
							array<int, MAX_TYPE> limits = { 0 };
							limits.fill(STOCKPILE_CAPACITY);

							shared_ptr<Storage> new_storage = make_shared<Storage>(storage_id++, stockpile->tribe, stockpile->pos, stockpile->r, 1, STOCKPILE_CAPACITY, limits, true);
							insertStorageShared(storages, new_storage);
							stockpile->storage = new_storage;
						}
					}
					else {
						if (stockpile->target.expired()) {
							for (shared_ptr<Worker> worker : settlement->workers) {
								array<int, MAX_TYPE> types = arrangeTypes(worker->forgetStorage(stockpile->storage.lock()));
								if (!types.empty()) {
									tuple<shared_ptr<Storage>, shared_ptr<Node>> result = createNode(types, worker->pos);
									insertStorageShared(storages, get<0>(result));
									nodes.emplace_back(get<1>(result));
									drawings.emplace_back(get<1>(result));
								}
							}

							if (!stockpile->storage.lock()->isEmpty()) {
								tuple<shared_ptr<Storage>, shared_ptr<Node>> result = createNode(stockpile->storage.lock()->is, stockpile->pos);
								insertStorageShared(storages, get<0>(result));
								nodes.emplace_back(get<1>(result));
								drawings.emplace_back(get<1>(result));
							}

							int id = stockpile->storage.lock()->id;
							erase_if(storages, [id](shared_ptr<Storage> s) {return s->id == id; });

							settlement->stockpiles.erase(settlement->stockpiles.begin() + stockpile_i);
							stockpile_i--;
							continue;
						}
					}
				}

				for (int forge_i = 0; forge_i < settlement->forges.size(); forge_i++) {
					shared_ptr<Forge> forge = settlement->forges[forge_i];
					if (!forge->construction.expired()) {
						shared_ptr<Construction> construction = forge->construction.lock();
						if (forge->target.expired()) {
							if (!construction->storage.expired()) {
								for (shared_ptr<Worker> worker : settlement->workers) {
									array<int, MAX_TYPE> types = arrangeTypes(worker->forgetStorage(construction->storage.lock()));
									if (!types.empty()) {
										tuple<shared_ptr<Storage>, shared_ptr<Node>> result = createNode(types, worker->pos);
										insertStorageShared(storages, get<0>(result));
										nodes.emplace_back(get<1>(result));
										drawings.emplace_back(get<1>(result));
									}
								}

								if (!forge->construction.lock()->storage.lock()->isEmpty()) {
									tuple<shared_ptr<Storage>, shared_ptr<Node>> result = createNode(construction->storage.lock()->is, forge->pos);
									insertStorageShared(storages, get<0>(result));
									nodes.emplace_back(get<1>(result));
									drawings.emplace_back(get<1>(result));
								}

								int id = construction->storage.lock()->id;
								erase_if(storages, [id](shared_ptr<Storage> s) {; return s->id == id; });
							}

							else if (!construction->task.expired()) {
								int id = construction->task.lock()->id;
								erase_if(settlement->tasks, [id](shared_ptr<Task> t) {return t->id == id; });
							}

							settlement->forges.erase(settlement->forges.begin() + forge_i);
							forge_i--;
							continue;
						}
						if (!construction->storage.expired()) {
							shared_ptr<Storage> storage = construction->storage.lock();
							if (storage->isFull(-1)) {
								int id = storage->id;
								erase_if(storages, [id](shared_ptr<Storage> s) {return s->id == id; });

								shared_ptr<Task> new_task = make_shared<Task>(task_id++, construction->pos, 1, 5.0f, 2);
								insertTaskShared(settlement->tasks, new_task);
								construction->task = new_task;
							}
						}

						if (!construction->task.expired() && construction->task.lock()->isCompleted()) {
							int id = construction->task.lock()->id;
							erase_if(settlement->tasks, [id](shared_ptr<Task> t) {return t->id == id; });
							id = construction->id;
							erase_if(settlement->constructions, [id](shared_ptr<Construction> c) {return c->id == id; });

							array<int, MAX_TYPE> limits = { 0 };
							limits.fill(STOCKPILE_CAPACITY);

							shared_ptr<Storage> new_storage_in = make_shared<Storage>(stockpile_id++, forge->tribe, forge->pos, forge->r, 2, resourceCount(forge->recipe), forge->recipe, true);
							insertStorageShared(storages, new_storage_in);

							shared_ptr<Storage> new_storage_out = make_shared<Storage>(stockpile_id++, forge->tribe, forge->pos, forge->r, 0, resourceCount(forge->produce), forge->produce, true);
							insertStorageShared(storages, new_storage_out);

							forge->storage_in = new_storage_in;
							forge->storage_out = new_storage_out;
						}
					}

					else {
						if (forge->target.expired()) {
							for (shared_ptr<Worker> worker : settlement->workers) {
								array<int, MAX_TYPE> types = arrangeTypes(worker->forgetStorage(forge->storage_in.lock()));
								if (!types.empty()) {
									tuple<shared_ptr<Storage>, shared_ptr<Node>> result = createNode(types, worker->pos);
									insertStorageShared(storages, get<0>(result));
									nodes.emplace_back(get<1>(result));
									drawings.emplace_back(get<1>(result));
								}
								types = arrangeTypes(worker->forgetStorage(forge->storage_out.lock()));
								if (!types.empty()) {
									tuple<shared_ptr<Storage>, shared_ptr<Node>> result = createNode(types, worker->pos);
									insertStorageShared(storages, get<0>(result));
									nodes.emplace_back(get<1>(result));
									drawings.emplace_back(get<1>(result));
								}
							}

							if (!forge->storage_in.lock()->isEmpty() || !forge->storage_out.lock()->isEmpty()) {
								array<int,MAX_TYPE> types = addArrays(forge->storage_in.lock()->is, forge->storage_out.lock()->is);
								tuple<shared_ptr<Storage>, shared_ptr<Node>> result = createNode(types, forge->pos);
								insertStorageShared(storages, get<0>(result));
								nodes.emplace_back(get<1>(result));
								drawings.emplace_back(get<1>(result));
							}

							int id_in = forge->storage_in.lock()->id;
							int id_out = forge->storage_out.lock()->id;
							erase_if(storages, [id_in, id_out](shared_ptr<Storage> s) {return s->id == id_in || s->id == id_out; });

							settlement->forges.erase(settlement->forges.begin() + forge_i);
							forge_i--;
							continue;
						}

						if (!forge->task.expired()) {
							shared_ptr<Task> task = forge->task.lock();
							if (task->isCompleted()) {
								forge->storage_in.lock()->is = { 0 };
								forge->storage_in.lock()->will_be = { 0 };

								addToStorage(forge->storage_out, forge->produce);

								int id = task->id;
								erase_if(settlement->tasks, [id](shared_ptr<Task> t) {return t->id == id; });
							}

							if (forge->storage_in.lock()->hasSpace(-1)) {
								int id = task->id;
								erase_if(settlement->tasks, [id](shared_ptr<Task> t) {return t->id == id; });
							}
						}
						else {
							if (forge->storage_in.lock()->isFull(-1) && forge->storage_out.lock()->capacity >= forge->storage_out.lock()->isStored() + resourceCount(forge->produce)) {
								shared_ptr<Task> new_task = make_shared<Task>(task_id++, forge->pos, 2, 3.0f, 1);
								insertTaskShared(settlement->tasks, new_task);
								forge->task = new_task;
							}
						}
					}	
				}
			}
		}

		BeginDrawing();
		ClearBackground(BLACK);

		DrawText(("storages: " + to_string(storages.size())).c_str(), 0, 50, 20, WHITE);

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

		for (int node = 0; node < nodes.size(); node ++) {
			if (nodes[node]->storage.lock()->isEmpty()) {
				int id = nodes[node]->storage.lock()->id;
				erase_if(storages, [id](weak_ptr<Storage> s) {return s.lock()->id == id; });
				nodes.erase(nodes.begin() + node);
				node--;
				continue;
			}
			nodes[node]->update();
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