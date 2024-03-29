#pragma once
#ifndef ECS_ENTITYMANAGER_H
#define ECS_ENTITYMANAGER_H

#include "entity.h"

namespace ecs
{
class EntityManager {
private:
	std::vector<std::shared_ptr<Entity>> m_entities;
	std::array<std::vector<Entity *>, MAX_GROUPS> m_groupedEntities;

	// This is used in the case where an invalid group is passed to
	// getEntitiesByGroup. Since that method return a reference, it's best
	// to return a reference to this empty vector (which will remain in scope)
	// than returning a reference to a local variable.
	// TODO: once there's a way to properly terminate the program, replace the use
	// of this vector with a program termination
	std::vector<Entity*> m_emptyVector;
    
public:
    EntityManager() = default; 
	/* Add a function to remove entities that have to be removed */
	void refresh() {
		/*
		 * Remove entites that don't belong to groups anymore and those that have been destroyed.
		 */
		for (auto i{0u}; i < MAX_GROUPS; i++) {
			auto &v{m_groupedEntities[i]};
			v.erase(std::remove_if(std::begin(v), std::end(v),
								   [i](Entity *_ent) {
									   return !_ent->hasGroup(i) || _ent->m_bDestroyed;
								   }), std::end(v));
		}
	}

	/*
	 * Add an entity to the manager.
     * the second argument dictates whether or not to generate a Uuid,
     * not required when loading serialized entities and scenes, as the Uuids
     * need to be persisted
	 */
	Entity &addEntity(std::string _name, bool _genUuid = true) {
		Entity *e {new Entity{}};

		e->m_manager = this;
		e->m_sName = std::move(_name); // since we're passing by copy and only copying once.
        if (_genUuid) {
            e->m_uuid = std::move(generateUuid());
        }

		std::shared_ptr<Entity> sPtr(e);
		m_entities.emplace_back(std::move(sPtr));
		return *e;
	}

	/*
	 * Add an Entity to a group
	 */
	void addToGroup(Entity *_ent, Group _group) {
		m_groupedEntities[_group].emplace_back(_ent);
	}

	/*
	 * Helper method to check if any entities of a given group exist
	*/
	bool hasEntitiesInGroup(Group _group) {
		return (_group < MAX_GROUPS) && !m_groupedEntities[_group].empty();
	}

	/*
	 * Get a group of entities, this will be handy for separating entites by the components they have.
	 */
	const std::vector<Entity *> &getEntitiesByGroup(Group _group) {
		if (_group >= MAX_GROUPS) {
			/*LogManager::getInstance().printMessage(
				LogMessage(
					LogTag::ECS, LogType::BUG, LogSeverity::CRITICAL,
					"Group given to getEntitiesByGroup is larger than max group"));*/
			
			return m_emptyVector;
		}

		return m_groupedEntities[_group];
	}

	std::vector<Entity *> *getMutableEntitiesByGroup(Group _group) {
		if (_group >= MAX_GROUPS) {
			/*LogManager::getInstance().printMessage(
				LogMessage(
					LogTag::ECS, LogType::BUG, LogSeverity::CRITICAL,
					"Group given to getMutableEntitiesByGroup is larger than max group"));*/
			return nullptr;
		}
		return &m_groupedEntities[_group];
	}

	std::vector<std::shared_ptr<Entity>> &getEntities() { return m_entities; }
};
 
// This is to avoid a cyclical dependency issue and to work around the forward declared EntityManager in entity.h
inline void ecs::Entity::addGroup(Group _group) noexcept {
   m_groupBitset[_group] = true;
   m_manager->addToGroup(this, _group);
}

} // namespace ecs

#endif
