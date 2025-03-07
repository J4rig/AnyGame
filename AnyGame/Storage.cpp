#pragma once
#include "Storage.hpp"
#include "Worker.hpp"



Storage::Storage(int id, Vector2 pos, int priority, int capacity, array<int, MAX_TYPE> limits, bool can_take) :
	id(id), pos(pos), priority(priority), capacity(capacity), can_be(limits), can_take(can_take) {
};

//od kazdeho workera ktory ho ma v zozname
//ziska typy zdrojov ktore sa nemozu ulozit

int Storage::isStored() {
	int result = 0;
	for (int i = 0; i < MAX_TYPE; i++) {
		result += is[i];
	}
	return result;
}

int Storage::aboutToBeStored() {
	int result = 0;
	for (int i = 0; i < MAX_TYPE; i++) {
		result += will_be[i];
	}
	return result;
}

bool Storage::isFull(int type) {
	if (type >= 0 && type < MAX_TYPE) {
		return isStored() >= capacity || is[type] >= can_be[type];
	}
	return isStored() >= capacity;
}

bool Storage::isEmpty() {
		return isStored() == 0;
}

bool Storage::hasSpace(int type) {
	if (type >= 0 && type < MAX_TYPE) {
		return aboutToBeStored() < capacity && will_be[type] < can_be[type];
	}
	return aboutToBeStored() < capacity;
}

bool Storage::hasSpace(vector<int> types) {
	for (int type : types) {
		if (aboutToBeStored() < capacity && will_be[type] < can_be[type]) {
			return true;
		}
	}
	return false;
}

int Storage::spaceLeft(int type) {
	return min(capacity - aboutToBeStored(), can_be[type] - will_be[type]);
} 