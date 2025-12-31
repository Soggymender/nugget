#ifndef ENTITY_H
#define ENTITY_H

#include "engine/Object.h"

#include "engine/SceneComponent.h"

using namespace std;

class NEntity : public NObject
{
public:

	NSceneComponent m_sceneComponent;

	NEntity();
};

#endif