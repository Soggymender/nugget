#include "Camera.h"

void Camera::ProcessInput(glm::vec3 dir, glm::vec3 euler, float zoomDelta, float dt)
{
    // Accumulate orientation.
    orient = glm::quat(euler);

    // Accumulate translation.
    pos += dir * (0.1f * dt);

    if (zoomDelta < 0.0f && pos.z > 0.5f) {
        pos.z += zoomDelta * 3.0f * dt;
    }
    else if (zoomDelta > 0.0f && pos.z < 2.5f) {
        pos.z += zoomDelta * 3.0f * dt;
    }
}