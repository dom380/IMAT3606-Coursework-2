#include "..\..\include\Camera\FollowCamera.h"

FollowCamera::FollowCamera(int width, int height, float aspect, glm::vec3 targetDistance, glm::vec3 pos, glm::vec3 up, glm::vec3 direction)
{
	distanceToTarget = targetDistance;
	pos -= targetDistance;
	position = pos;
	upDir = up;
	frontDir = direction;
	orientation = glm::quat(1.0, 0.0, 0.0, 0.0);
	type = CameraClass::PERSPECTIVE;
	projection = glm::perspective(glm::radians(aspect), (float)width / (float)height, 0.1f, 1000.0f);
	lookAt(direction);
}
/*
void FollowCamera::handle(MouseEvent event)
{
}

void FollowCamera::handle(KeyEvent event)
{
}
*/
void FollowCamera::move(glm::vec3 newPosition)
{
	position = newPosition - distanceToTarget;
	update();
}
