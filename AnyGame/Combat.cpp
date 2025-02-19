#pragma once
#include "Combat.hpp"

Combat::Combat(int id, Vector2 pos, float r, int max_health, int defense, int damage) :
	id(id), pos(pos), r(r), max_health(max_health), health(max_health), defense(defense), damage(damage) {
};

void Combat::attack(weak_ptr<Combat>& target) {
	target.lock()->health -= damage - target.lock()->defense;
}

bool Combat::canAttack() const {
	return attack_cooldown <= time_to_attack;
}

bool Combat::isDead() const {
	return health <= 0;
}