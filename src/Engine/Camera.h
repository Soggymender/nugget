#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class Camera
{
public:

	glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);

	glm::vec3 pos;

	glm::mat4 view = glm::mat4(1.0f);

	Camera(float x, float y, float z)
		: pos(x, y, z)
	{
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -1.0f));


	}

	void ProcessInput(glm::vec3 dir, float zoomDelta, float dt);

	void Update()
	{
		glm::mat4 identity(1.0f);
		view = glm::translate(identity, -pos);
	}
};