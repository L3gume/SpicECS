#pragma once
#ifndef ECS_COMPONENT_H
#define ECS_COMPONENT_H

#include "ecs.h"

#include "../3rdParty/rapidjson/rapidjson.h"
#include "../3rdParty/rapidjson/document.h"
#include "../3rdParty/rapidjson/allocators.h"

namespace ecs
{
	
class Entity;
	
/*
 * Component Implementation
 *
 * A component is a logic-less data object. it only contains data that will be handled by a system.
 */
struct Component {
	Entity *m_entity{ nullptr };

	// ... stuff ...
	bool m_bEnabled;
    bool m_bInitialized;

    virtual ~Component() = default;
    virtual void serialize(rapidjson::Document& root, rapidjson::Document::AllocatorType& alloc) = 0;
    virtual void deserialize(const rapidjson::Value& root) = 0;
    
    ecs::ComponentID id;
};

} // namespace ecs

#endif
