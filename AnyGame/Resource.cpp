#pragma once
#include "Resource.hpp"

Resource::Resource(int type) :
	type(type) {
	occupied.fill(0);
};