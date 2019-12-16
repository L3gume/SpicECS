#pragma once

#include "../ecs/component.h"
#include "../ecs/reflection/reflection.h"
#include "../serialization/componentserializer.h"

struct SceneComponent : ecs::Component {
    H_DECLARE_AS_REFLECTABLE(SceneComponent)
        
    SceneComponent() = default;
    SceneComponent(const std::string& _name, const uint8_t flags) : name(_name), scene_flags(flags) 
    { /* empty body */ }
    
    void serialize(rapidjson::Document& root, rapidjson::Document::AllocatorType& alloc) override;
    void deserialize(const rapidjson::Value& root) override;

    std::string name;
    uint8_t scene_flags;
    // you may add more information on your scene here, this is just an example
};

H_SERIALIZE_IMPL(SceneComponent)
H_DESERIALIZE_IMPL(SceneComponent)
    
H_DEFINE_NAMED_REFLECTION_DATA("SceneComponent", SceneComponent,
        H_ADD_NAMED_PROPERTY(name, "name"),
        H_ADD_NAMED_PROPERTY(scene_flags, "flags")
)

