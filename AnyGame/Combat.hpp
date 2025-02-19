#pragma once
#include "any_game.hpp"

class Combat {
public:
	int id;
	Vector2 pos;
	float r;

	int max_health;
	int health;

	int defense;

	int damage;

	float time_to_attack = 0.0f;
	float attack_cooldown = 3.0f;

	Combat(int id, Vector2 pos, float r, int max_health, int defence, int damage);

	void attack(weak_ptr<Combat>& target);

	bool canAttack() const;
	bool isDead() const;
};