//
// Created by notjustin on 2/8/18.
//

/*
 * TODO: implement allocators
 */
#pragma once
#ifndef ECS_ECS_H
#define ECS_ECS_H

#include <vector>
#include <memory>
#include <bitset>
#include <cassert>
#include <algorithm>
#include <fstream>

namespace ecs
{

/*
 * Constexpr and typedef
 */
constexpr std::size_t MAX_COMPS{32};
constexpr std::size_t MAX_GROUPS{32};

using ComponentID = std::size_t;
using Group = std::size_t;
using GroupBitset = std::bitset<MAX_GROUPS>;

/*
 * Stuff
 */
inline ComponentID getUniqueComponentID() noexcept {
	static ComponentID lastID{0u};
	return lastID++;
}

// Template Magic, have fun trying to figure it out!
template<typename T>
ComponentID getComponentTypeID() noexcept {
	static auto typeID{getUniqueComponentID()};
	return typeID;
}
}
#endif //ECS_ECS_H
