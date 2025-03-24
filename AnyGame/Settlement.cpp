#pragma once
#include "Settlement.hpp"
#include "Mine.hpp"
#include "Stockpile.hpp"
#include "Forge.hpp"

Settlement::Settlement(DEPTH z, int tribe, int id, Vector2 pos, float r) :
	Drawing(z), tribe(tribe), id(id), pos(pos), r(r) {
};

void Settlement::update() {
	vector<Vector2> positions = {};

	Vector2 sum = { 0,0 };
	int cnt = 0;

	for (shared_ptr<Mine> mine : mines) {
		positions.emplace_back(mine->pos);
		cnt++;
		sum = Vector2Add(sum, mine->pos);
	}

	for (shared_ptr<Stockpile> stockpile : stockpiles) {
		positions.emplace_back(stockpile->pos);
		cnt++;
		sum = Vector2Add(sum, stockpile->pos);
	}

	for (shared_ptr<Forge> forge : forges) {
		positions.emplace_back(forge->pos);
		cnt++;
		sum = Vector2Add(sum, forge->pos);
	}

	pos = Vector2Divide(sum, { (float)cnt,(float)cnt });
	r = 0;
	for (Vector2 p : positions) {
		if (r < Vector2Distance(pos, p)) {
			r = Vector2Distance(pos, p);
		}
	}
	r += 50.0f;
}

void Settlement::draw() const {
	DrawCircleLinesV(pos, r, tribe_color[tribe]);
}