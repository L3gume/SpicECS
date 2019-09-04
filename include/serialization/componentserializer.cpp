#include <string>
#include <cstring>
#include <tuple>
#include <utility>

#include "../3rdParty/rapidjson/rapidjson.h"
#include "../3rdParty/rapidjson/document.h"
#include "../3rdParty/rapidjson/allocators.h"

#include "componentserializer.h"
#include "../components/factory/factorymanager.h"

using namespace hserialization;

void hcomponentserializer::serializeComponent(ecs::Component* comp, rapidjson::Document& root,
                                                rapidjson::Document::AllocatorType& alloc) {
    comp->serialize(root, alloc);
}


void hcomponentserializer::deserializeComponent(ecs::Entity* entity, const char* compName, const rapidjson::Value& root) {
    auto* fact = ecs::HComponentFactoryManager::getInstance().getComponentFactory(std::string(compName));
    auto& comp = entity->addComponentFromFactory(fact);
    comp.deserialize(root);
}
