#pragma once
#ifndef ECS_FACTORYMANAGER_H
#define ECS_FACTORYMANAGER_H

#include <unordered_map>

#include "../../util/singleton.h"
#include "../../ecs/ecs.h"
#include "../../ecs/component.h"
#include "factory.h"

#define H_REGISTER_COMPONENT_FACTORY(name, class_name)\
    ecs::HComponentFactoryManager::getInstance().addComponentFactory<class_name>(name);

namespace ecs
{
    
class HComponentFactoryManager {
    HSINGLETON(HComponentFactoryManager) // Kinda evil, dunno
        
public:
    template <typename T>
    void addComponentFactory(const std::string& name) {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from ecs::Component"); 
        assert(m_factories.find(name) == m_factories.end());
        
        m_factories[name] = new HComponentFactory<T>();
    }
    
    IComponentFactory* getComponentFactory(const std::string& name) {
        assert(m_factories.find(name) != m_factories.end());
        return m_factories[name];
    }
    
private:
    std::unordered_map<std::string, IComponentFactory*> m_factories;
};

}
#endif
