#pragma once
#ifndef H_ENTITY_SERIALIZER
#define H_ENTITY_SERIALIZER

#include "../3rdParty/rapidjson/rapidjson.h"
#include "../3rdParty/rapidjson/document.h"
#include "../3rdParty/rapidjson/allocators.h"

#include "../ecs/entitymanager.h"

namespace hserialization::hentityserializer {

void serializeEntity(ecs::Entity* entity, rapidjson::Document& root, rapidjson::Document::AllocatorType& alloc);
void deserializeEntity(ecs::Entity* parent, ecs::EntityManager& entMgr, rapidjson::Document& root);
void deserializeEntity(ecs::Entity* parent, ecs::EntityManager& entMgr, const rapidjson::Value& root);

}

#endif
