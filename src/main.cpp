#include "ecs/entitymanager.h"
#include "serialization/sceneserializer.h"
#include "example_components/scenecomponent.h"

int main() {
    ecs::EntityManager entMgr;
    auto& ent = entMgr.addEntity("SceneEntity", true);
    ent.addComponent<SceneComponent>("SceneName", 0x00);
    
    return 0;
}
