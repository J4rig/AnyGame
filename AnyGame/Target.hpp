#pragma once
#include "any_game.hpp"

class Target {
public:
	int id;
	int tribe;

	Vector2* pos;
	float r;

	int max_health;
	int health;

	int defense;

	int damage;

	float time_to_attack = 0.0f;
	float attack_cooldown = 0.1f;

	function<void()> expired;

	Target(int id, int tribe, Vector2* pos, float r, int max_health, int defence, int damage);

	void attack(weak_ptr<Target> target);

	bool canAttack() const;
	bool isDead() const;
};