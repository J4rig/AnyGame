#pragma once
#include "Raider.hpp"
#include "Combat.hpp"

Raider::Raider(int id, Vector2 pos, weak_ptr<Combat> combat) :
	id(id), pos(pos), combat(combat) {
};

void Raider::draw() const {
	float r = combat.lock()->r;
	float angle = ((float)combat.lock()->health / combat.lock()->max_health) * 360;
	DrawCircleLinesV(pos, 15, PURPLE);
	DrawRing(pos, 0, r, 0.0f, angle, 0, RED);
}

void Raider::update(vector<shared_ptr<Combat>> targets) {
	if (target.expired()) {
		target = findTarget(pos, combat, targets);
	}
	else {
		if (Vector2Distance(pos, target.lock()->pos) < combat.lock()->r + target.lock()->r) {
			if (!combat.lock()->canAttack()) {
				combat.lock()->time_to_attack += GetFrameTime();
			}
			else {
				combat.lock()->attack(target);
				combat.lock()->time_to_attack = 0.0f;
			}
		}
		else {
			pos = Vector2MoveTowards(pos, target.lock()->pos, SPEED_MOD * 5 * GetFrameTime());
			combat.lock()->pos = pos;
			combat.lock()->time_to_attack = 0.0f;
		}
	}
}