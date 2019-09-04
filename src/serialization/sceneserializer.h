#pragma once
#ifndef H_SCENE_SERIALIZER
#define H_SCENE_SERIALIZER

#include "../3rdParty/rapidjson/allocators.h"

#include "../ecs/entity.h"
#include "../ecs/entitymanager.h"

namespace hserialization::hsceneserializer {

void serializeScene(ecs::Entity* sceneEntity, std::filesystem::path scenePath, const std::string& spath, bool pretty);
void deserializeScene(ecs::EntityManager& entityMgr, std::filesystem::path scenePath, const std::string& spath);

}

#endif
