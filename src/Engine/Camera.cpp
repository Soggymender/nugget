#include "Camera.h"

void Camera::ProcessInput(glm::vec3 dir, glm::vec3 euler, float zoomDelta, float dt)
{
    // Accumulate orientation.
    // orient = glm::quat(euler);
    //orient = glm::fquat(euler.x, euler.y, 0.0f);

    glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::quat pitchQuat = glm::angleAxis(euler.x, right);

    orient = pitchQuat;


    glm::vec3 worldUp = orient * glm::vec3(0.0f, 1.0f, 0.0f);
    glm::quat yawQuat = glm::angleAxis(euler.y, worldUp);

    orient = yawQuat *orient;





    // Accumulate translation.
    pos += dir;

    pos.z += zoomDelta * 9.0f * dt;

    /*
    if (zoomDelta < 0.0f && pos.z > 0.5f) {
        pos.z += zoomDelta * 3.0f * dt;
    }
    else if (zoomDelta > 0.0f && pos.z < 2.5f) {
        pos.z += zoomDelta * 3.0f * dt;
    }
    */
}