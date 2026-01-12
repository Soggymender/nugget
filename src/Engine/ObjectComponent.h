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

	NObject* GetParent() { return m_pParent;  }

	virtual void Update(float deltaTime) { };

protected:
	void SetParent(NObject* pParent) { m_pParent = pParent; }

private:

	NObject* m_pParent = nullptr;
};

#endif