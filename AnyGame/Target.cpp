#pragma once
#include "Target.hpp"

Target::Target(int id, int tribe, Vector2* pos, float r, int max_health, int defense, int damage) :
	id(id), tribe(tribe), pos(pos), r(r), max_health(max_health), health(max_health), defense(defense), damage(damage) {
};

void Target::attack(weak_ptr<Target> target) {
	target.lock()->health -= damage - target.lock()->defense;
}

bool Target::canAttack() const {
	return attack_cooldown <= time_to_attack;
}

bool Target::isDead() const {
	return health <= 0;
}