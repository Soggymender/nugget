#include "sprite.h"

#include "BehaviorTree.h"

class Npc
{
public:

    bool Create(Sprite* sprite);
    void Destroy();

    void Update(float deltaTime);
    void Render();
    
public:

    BehaviorTree behaviorTree;
    Sprite* sprite;

    glm::mat4 model;
    glm::vec3 pos;
    float depthOffset = 0.0f;
};