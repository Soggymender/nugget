#pragma once

#include "../vm/vm.h"

#include "glm/glm.hpp"

using namespace std;

class Probe;

class AttitudeControlModule
{
/*
ACM

Continually monitors antenna gain by checking angle between rx/tx and Earth.

If Target Gain is set, manipulates thrusters to maintain Target Gain

Target Gain is read from 0x0F001 when Target Gain Status bit is set at 0xF000.
*/

public:

	Probe* probe = nullptr;

	float targetGain = 0.0f;

	glm::vec3 earth = glm::vec3(0.0f, 0.0f, 0.0f);

	float signalStrength = 0.0f;

	void Initialize(Probe* probe);
	void Update(float deltaTime);
};
