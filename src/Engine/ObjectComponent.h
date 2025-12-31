#ifndef OBJECT_COMPONENT_H
#define OBJECT_COMPONENT_H

using namespace std;

class NObject;

/*
	A component base class. The owning class must be derived from NObject.
*/

class NObjectComponent
{
	friend NObject;

public:

	NObject* GetOwner() { return m_pOwner;  }

protected:
	void SetOwner(NObject* owner) { m_pOwner = owner; }

private:

	NObject* m_pOwner = nullptr;
};

#endif