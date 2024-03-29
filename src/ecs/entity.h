#pragma once
#ifndef ECS_ENTITY_H
#define ECS_ENTITY_H

#include "ecs.h"
#include "component.h"
#include "componentfactory/factory.h"

namespace ecs
{
	
class EntityManager;
	
using ComponentBitset = std::bitset<MAX_COMPS>;
using ComponentArray = std::array<Component *, MAX_COMPS>;
	
class Entity {
	friend class EntityManager;
public:
    
	std::string getName() const { return m_sName; }
    
    Uuid getUuid() const { return m_uuid; }
    void setUuid(Uuid uuid) { m_uuid = uuid; }
    
	const Entity* getParent() const { return m_parent; }
    
	const std::vector<Entity*>& getChildren() const { return m_children; };
    
	void destroy() { m_bDestroyed = true; }
	
    void addChild(Entity* child) {
        m_children.emplace_back(child);
    }
    
    void setParent(Entity* parent) {
        m_parent = parent;
        parent->addChild(this);
    }
    
	/*
	 * Check if the entity already has an instance of component T
	 */
	template<typename T>
	bool hasComponent() const {
        return m_componentBitset[getComponentTypeID<T>()];
	}
    
    bool hasComponent(const ComponentID& _id) const {
        return m_componentBitset[_id];
	}

	/*
	 * Check if entity belongs to a group
	 */
	bool hasGroup(Group _group) const noexcept {
		return m_groupBitset[_group];
	}

	/*
	 * Add entity to group
	 */
	void addGroup(Group _group) noexcept;
        
	/*
	 * Remove entity from group, the manager will automatically get rid of it
	 */
	void deleteFromGroup(Group _group) noexcept {
		m_groupBitset[_group] = false;
	}

	/*
	 * Add a component to the entity
	 */
	template<typename T, typename... TArgs>
	T &addComponent(TArgs &&... _args) {
		assert(!hasComponent<T>()); // Make sure the entity doesn't already have this type of component.
		const auto id = getComponentTypeID<T>();

		auto *c(new T(std::forward<TArgs>(_args)...));
		c->m_entity = this;
        c->id = id;
		std::unique_ptr<Component> uPtr(std::move(c));
		m_components.emplace_back(std::move(uPtr));

		m_componentArray[id] = c; // NOLINT(bugprone-use-after-move)
		m_componentBitset[id] = true;

		this->addGroup(id);
		c->m_bEnabled = true;
		c->m_bInitialized = false;

		return *c;
	}

    Component &addComponentFromFactory(const IComponentFactory *_factory) {
        const auto id = _factory->getID();
        assert(!hasComponent(id));

        auto *c(_factory->make());
        std::unique_ptr<Component> uPtr(c);
        m_components.emplace_back(std::move(uPtr));

        m_componentArray[id] = c; // NOLINT(bugprone-use-after-move)
		m_componentBitset[id] = true;  
        
        this->addGroup(id);
        c->m_bEnabled = true;
		c->m_bInitialized = false;
        
        return *c;
    }
    
	/*
	 * Get the entity's component T, if it has one.
	 */
	template<typename T>
	T &getComponent() const {
		assert(hasComponent<T>()); // will throw an error
		auto ptr(m_componentArray[getComponentTypeID<T>()]);
		return *static_cast<T *>(ptr);
	}
    
    std::vector<std::unique_ptr<Component>>& getComponents() {
        return m_components;
    }
    
private:
	EntityManager* m_manager;
	/*
	 * Unique_ptrs make it so we don't leak any memory
	 */
	std::vector<std::unique_ptr<Component>> m_components;
	ComponentBitset m_componentBitset;
	ComponentArray m_componentArray;
	GroupBitset m_groupBitset;

	std::vector<Entity*> m_children;
	Entity* m_parent;

	std::string m_sName;
    Uuid m_uuid;

	bool m_bDestroyed = false; // entity will be removed from manager when this is true. Components are unique_ptr
							   // so no need to worry about them
};
	
}

#endif
