#include "..\..\include\Camera\EngineCamera.h"
#include <iostream>

EngineCamera::EngineCamera(int width, int height, float aspect, glm::vec3 pos, glm::vec3 up, glm::vec3 direction)
{
	canMove = false;
	cameraSpeed = normalCameraSpeed;
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

void EngineCamera::handle(MouseEvent event)
{
	switch (event.eventType)
	{
		case MouseEventType::LEFT_CLICK:
		{
			if (event.actionType == MouseActionType::MOUSE_PRESS)
			{
				canMove = true;
			}
			else if (event.actionType == MouseActionType::MOUSE_RELEASE)
			{
				canMove = false;
				previousX = -1;
				previousY = -1;
			}
			
			break;
		}
		
		case MouseEventType::MOUSE_MOVE:
		{
			if (canMove)
			{
				if (previousX < 0) previousX = event.x;
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
				update();
			}
			
			break;
		}
		default:
		{
			break;
		}
	}
}

void EngineCamera::handle(KeyEvent event)
{
	if (event.type == KeyEventType::KEY_REPEATED || event.type == KeyEventType::KEY_PRESSED) {
		if (event.key == KeyCodes::LEFT_SHIFT)
		{
			cameraSpeed = normalCameraSpeed * 2;
		}
		if (event.key == KeyCodes::W) //W
		{
			position -= cameraSpeed * frontDir;
		}
		else if (event.key == KeyCodes::S) //S
		{
			position += cameraSpeed * frontDir;
		}
		else if (event.key == KeyCodes::A) //A
		{
			position -= rightDir * cameraSpeed;
		}
		else if (event.key == KeyCodes::D) //D
		{
			position += rightDir * cameraSpeed;
		}
		
		update();
	}
	if (event.type == KeyEventType::KEY_RELEASED)
	{
		if (event.key == KeyCodes::LEFT_SHIFT) //shift
		{
			cameraSpeed = normalCameraSpeed;
		}
	}
}
