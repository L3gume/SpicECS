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

// one of these headers uses a deprecated boost header, sad
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

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
using Uuid = boost::uuids::uuid;


/*
 * Stuff
 */
inline ComponentID getUniqueComponentID() noexcept {
	static ComponentID lastID{0u};
	return lastID++;
}

// Template Magic :)
template<typename T>
ComponentID getComponentTypeID() noexcept {
	static auto typeID{getUniqueComponentID()};
	return typeID;
}

// Generate a new random Uuid
Uuid generateUuid() noexcept {
    return boost::uuids::random_generator()();
}

}
#endif //ECS_ECS_H

