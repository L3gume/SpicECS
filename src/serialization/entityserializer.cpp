#include <string>
#include <iostream>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "entityserializer.h"
#include "componentserializer.h"

using namespace hserialization;

void hentityserializer::serializeEntity(ecs::Entity* entity, rapidjson::Document& root,
                                        rapidjson::Document::AllocatorType& alloc) {
    rapidjson::Document entDoc(&alloc);
    entDoc.SetObject();
    
    auto s = rapidjson::Value(entity->getName().c_str(), alloc);
    entDoc.AddMember("Name", s, alloc);
    std::stringstream ss;
    ss << entity->getUuid();
    auto u = rapidjson::Value(ss.string(), alloc);
    entDoc.AddMember("Uuid", u, alloc);
    
    rapidjson::Document componentsDoc(&alloc);
    componentsDoc.SetObject();
    const auto& vec = entity->getComponents();
    for (const auto& comp : vec) {
        hcomponentserializer::serializeComponent(comp.get(), componentsDoc, alloc);
    }
    entDoc.AddMember("Components", componentsDoc, alloc);

    rapidjson::Document childrenDoc(&alloc);
    childrenDoc.SetObject();
    for (const auto& ent : entity->getChildren()) {
        serializeEntity(ent, childrenDoc, alloc);
    }
    entDoc.AddMember("Children", childrenDoc, alloc);
    
    root.AddMember("Entity", entDoc, alloc);
}

void hentityserializer::deserializeEntity(ecs::Entity* parent, ecs::EntityManager& entMgr, rapidjson::Document& root) {
    deserializeEntity(nullptr, entMgr, root["Entity"]);
}

void hentityserializer::deserializeEntity(ecs::Entity* parent, ecs::EntityManager& entMgr, const rapidjson::Value& root) {
    
    assert(root.HasMember("Name"));
    assert(root["Name"].IsString());
    auto& ent = entMgr.addEntity(root["Name"].GetString(), false);
    
    assert(root.HasMember("Uuid"));
    assert(root["Uuid"].IsString());
    boost::uuids::uuid uid;
    std::stringstream ss;
    ss << root["Uuid"].GetString();
    ss >> uid;
    ent.setUuid(uid);
    
    if (parent != nullptr) {
        ent.setParent(parent);
    }
    
    assert(root.HasMember("Components"));
    assert(root["Components"].IsObject());
    const auto& compsDoc = root["Components"];
    for (rapidjson::Value::ConstMemberIterator iter = compsDoc.MemberBegin(); iter != compsDoc.MemberEnd(); ++iter){
        hcomponentserializer::deserializeComponent(&ent, iter->name.GetString(), iter->value);
    }
    
    assert(root.HasMember("Children"));
    assert(root["Children"].IsObject());
    const auto& childrenDoc = root["Children"];
    for (rapidjson::Value::ConstMemberIterator iter = childrenDoc.MemberBegin(); iter != childrenDoc.MemberEnd(); ++iter){
        deserializeEntity(&ent, entMgr, iter->value);
    }
}
