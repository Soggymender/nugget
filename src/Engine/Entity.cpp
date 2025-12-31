#include "Entity.h"

using namespace std;

NEntity::NEntity()
{
	AttachComponent(&m_sceneComponent);
}