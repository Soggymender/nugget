#include "npc.h"

bool Npc::Create(Sprite* sprite)
{
    
    this->sprite = sprite;
    
    model = glm::mat4(1.0f);

    /*
    auto sequence = std::make_shared<BrainTree::Sequence>();
    auto sayHello = std::make_shared<Action>();
    auto sayHelloAgain = std::make_shared<Action>();
    
    sequence->addChild(sayHello);
    sequence->addChild(sayHelloAgain);
    
    behaviorTree.setRoot(sequence);
    behaviorTree.update();
    */



    return true;
}

void Npc::Destroy()
{

}

void Npc::Update(float deltaTime)
{
    static glm::mat4 identity(1.0f);

    glm::vec3 adjustedPos(pos);
    adjustedPos.z += depthOffset;

    model = glm::translate(identity, adjustedPos);
}

void Npc::Render()
{
    if (sprite == nullptr)
        return;

    sprite->model = model;
    sprite->Render();
}