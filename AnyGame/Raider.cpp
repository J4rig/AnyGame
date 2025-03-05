#pragma once
#include "Raider.hpp"
#include "Target.hpp"

Raider::Raider(int id, Vector2 pos, weak_ptr<Target> target) :
	id(id), pos(pos), target(target) {
};

void Raider::draw() const {
	float r = target.lock()->r;
	float angle = ((float)target.lock()->health / target.lock()->max_health) * 360;
	DrawCircleLinesV(pos, 15, tribe_color[target.lock()->tribe]);
	DrawRing(pos, 0, r - 1, 0.0f, angle, 0, RED);
}

void Raider::update(vector<weak_ptr<Target>> targets) {
	if (prey.expired()) {
		prey = findTarget(pos, target, targets);
	}
	else {
		if (Vector2Distance(pos, (*prey.lock()->pos)) < target.lock()->r + prey.lock()->r) {
			if (!target.lock()->canAttack()) {
				target.lock()->time_to_attack += GetFrameTime();
			}
			else {
				target.lock()->attack(prey);
				target.lock()->time_to_attack = 0.0f;
			}
		}
		else {
			pos = Vector2MoveTowards(pos, (*prey.lock()->pos), SPEED_MOD * GetFrameTime());
			target.lock()->time_to_attack = 0.0f;
		}
	}
}