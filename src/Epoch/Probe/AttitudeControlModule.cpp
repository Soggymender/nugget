#include "AttitudeControlModule.h"

#include "../vm/vm.h"

#include "glm/gtx/rotate_vector.hpp"

#include "probe.h"

#include "MemoryMap.h"

void AttitudeControlModule::Initialize(Probe* probe)
{
    this->probe = probe;
}

void AttitudeControlModule::Update(float deltaTime)
{
    // Read target gain if set by vm.
    unsigned short value;
	bool result = vm_read_memory(targetGainStatusAddr, targetGainAddr, value);
    if (result == 1) {
        // Read the ushort, force to the range of 0 - 360, and normalize to the range 0 to 1.
        targetGain = fmax(0.0f, fmin(float(value), 360.0f)) / 360.0f;
    }

    // Get the angle between the probe's antenna and earth.
    float headingRad = glm::radians(probe->heading);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    glm::vec3 forward(0.0f, 0.0f, 1.0f);

    forward = glm::rotateY(forward, headingRad);

    glm::vec3 toEarth = earth - probe->translation;
    toEarth = glm::normalize(toEarth);

    signalStrength = glm::dot(forward, toEarth);
    signalStrength = glm::clamp(signalStrength, 0.0f, 1.0f);

    // Convert the normalized gain to the range 0 to 360.
    value = (unsigned short)roundf(360.0f * signalStrength);

    // Write gain to VM.
    vm_write_memory(gainStatusAddr, gainAddr, value);

    // Lock on to target gain if set.
    if (probe->headingRate != 0.0f && targetGain > 0.0f) {
        float delta = fabs(signalStrength - targetGain);
        if (delta <= 0.0001f) {
            probe->headingRate = 0.0f;
        }
    }
}