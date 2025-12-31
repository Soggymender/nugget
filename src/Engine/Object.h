#ifndef OBJECT_H
#define OBJECT_H

#include <vector>

#include "engine/ObjectComponent.h"

using namespace std;

class NObject
{
public:

	void AttachComponent(NObjectComponent* component)
	{
		m_components.push_back(component);

		component->SetOwner(this);
	}

	template<typename T>
	T* GetFirstComponentOfType()
	{
		for (NObjectComponent* component: m_components)
		{
			if (auto casted = dynamic_cast<T*>(component))
				return casted;
		}

		return nullptr;
	}

protected:

	vector<NObjectComponent*>m_components;
};

#endif