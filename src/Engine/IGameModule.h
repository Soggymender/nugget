#ifndef IGAMEMODULE_H
#define IGAMEMODULE_H

class IGameModule
{
public:

    virtual void Create() = 0;
    virtual void Destroy() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Render() = 0;

    virtual ~IGameModule() = default;
};

#endif