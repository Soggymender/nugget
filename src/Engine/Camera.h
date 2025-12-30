#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "glm/gtx/quaternion.hpp"

class Camera
{
public:

	glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);

	glm::vec3 pos;
	glm::quat orient;

	glm::mat4 view = glm::mat4(1.0f);

	Camera(float x, float y, float z)
		: pos(x, y, z)
	{
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -1.0f));

		orient = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	}

	void RotateEuler(float x, float y, float z)
	{
		glm::vec3 eulerAngles(x, y, z);
		glm::quat rot = glm::quat(eulerAngles);

		orient = orient * rot;
	}

	void ProcessInput(glm::vec3 dir, glm::vec3 euler, float zoomDelta, float dt);

	void Update()
	{
		glm::mat4 identity(1.0f);
		
		view = glm::toMat4(-orient);
		view = glm::translate(view, -pos);
	}

	glm::vec3 GetForward()
	{
		return forward * orient;// *forward;
	}

	glm::vec3 GetRight()
	{
		return right * orient;
	}

	glm::vec3 GetUp()
	{
		return up * orient;
	}
};