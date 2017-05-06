#include "..\..\include\Camera\FollowCamera.h"

FollowCamera::FollowCamera(int width, int height, float aspect, glm::vec3 targetDistance, glm::vec3 pos, glm::vec3 up, glm::vec3 direction)
{
	distanceToTarget = targetDistance;
	pos -= targetDistance;
	position = pos;
	upDir = up;
	frontDir = direction;
	orientation = glm::quat(1.0, 0.0, 0.0, 0.0);
	type = CameraClass::FOLLOW;
	projection = glm::perspective(glm::radians(aspect), (float)width / (float)height, 0.1f, 1000.0f);
	lookAt(direction);
}
void FollowCamera::lookAt(float x, float y, float z)
{
	glm::vec3 xyz = glm::vec3(x, y, z);
	lookAt(xyz);
}

void FollowCamera::lookAt(glm::vec3 target)
{
	glm::vec3 followCamTilt = glm::vec3(0.0f, 0.25f, 0.0f);
	target -= followCamTilt;
	view = glm::lookAt(position, position + target, upDir);
	orientation = glm::quat_cast(view);
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

void FollowCamera::setFollowDist(glm::vec3 dist)
{
	distanceToTarget = dist;
}
