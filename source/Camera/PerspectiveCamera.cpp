#include <Camera\PerspectiveCamera.h>

PerspectiveCamera::PerspectiveCamera(int width, int height, float aspect, glm::vec3 pos, glm::vec3 up, glm::vec3 direction)
{
	position = pos;
	upDir = up;
	frontDir = direction;
	previousX = -1;
	previousY = -1;
	orientation = glm::quat(1.0, 0.0, 0.0, 0.0);
	type = CameraClass::PERSPECTIVE;
	projection = glm::perspective(glm::radians(aspect), (float)width / (float)height, 0.1f, 1000.0f);
	lookAt(direction);
}

void PerspectiveCamera::update()
{
	view = glm::mat4_cast(orientation);
	//Extract the camera coordinate axes from this matrix
	rightDir = glm::vec3(view[0][0], view[1][0], view[2][0]);
	upDir = glm::vec3(view[0][1], view[1][1], view[2][1]);
	frontDir = glm::vec3(view[0][2], view[1][2], view[2][2]);
	//And use this and current camera position to translate the view matrix
	view[3][0] = -glm::dot(rightDir, position); //Translation x
	view[3][1] = -glm::dot(upDir, position); //Translation y
	view[3][2] = -glm::dot(frontDir, position); //Translation z

}

void PerspectiveCamera::lookAt(float x, float y, float z)
{
	lookAt(glm::normalize(glm::vec3(x, y, z)));
}

void PerspectiveCamera::lookAt(glm::vec3 target)
{
	view = glm::lookAt(position, position+target, upDir);
	orientation = glm::quat_cast(view);
}


bool PerspectiveCamera::handle(MouseEvent& event)
{
	switch (event.eventType)
	{
		case MouseEventType::MOUSE_MOVE:
		{
			/*if (previousX < 0) previousX = event.x;
			if (previousY < 0) previousY = event.y;
			float yaw = (event.x - previousX); 
			float pitch = (event.y - previousY);

			//pitch rotation
			glm::quat pitchRotation = getRotation(worldX, glm::radians(pitch));
			orientation = glm::normalize(pitchRotation * orientation);
			//yaw rotation
			glm::quat yawRotation = getRotation(worldY, glm::radians(yaw));
			orientation = glm::normalize(orientation * yawRotation);
			previousX = event.x;
			previousY = event.y;
			update();*/
			break;
		}
		default:
		{
			break;
		}
	}
	return false;
}

bool PerspectiveCamera::handle(KeyEvent& event)
{
	if (event.type == KeyEventType::KEY_REPEATED || event.type == KeyEventType::KEY_PRESSED) {
		if (event.key == 87) //W
		{
			position -= 0.5f * frontDir;
		}
		else if (event.key == 83) //S
		{
			position += 0.5f * frontDir;
		}
		else if (event.key == 65) //A
		{
			position -= rightDir * 0.5f;
		}
		else if (event.key == 68) //D
		{
			position += rightDir * 0.5f;
		}
		update();
	}
	return false;
}

glm::quat PerspectiveCamera::getRotation(glm::vec3 axis, float angle)
{
	glm::quat rotation;
	float halfAngle = angle / 2;
	rotation.w = glm::cos(halfAngle);
	rotation.x = glm::sin(halfAngle)*axis.x;
	rotation.y = glm::sin(halfAngle)*axis.y;
	rotation.z = glm::sin(halfAngle)*axis.z;

	return rotation;
}
