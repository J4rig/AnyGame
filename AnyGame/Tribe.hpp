#pragma once
#include "any_game.hpp"

class Tribe {
public:
	int id;

	vector<shared_ptr<Settlement>> settlements = vector<shared_ptr<Settlement>>();
	vector<shared_ptr<Raider>> raiders = vector<shared_ptr<Raider>>();

	//vector<Settler> settlers;

	Tribe(int id,  shared_ptr<Settlement> settlement);

};