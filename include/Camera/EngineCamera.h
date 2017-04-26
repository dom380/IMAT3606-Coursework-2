#pragma once
#ifndef ENGINECAMERA_H
#define ENGINECAMERA_H
#define GLM_FORCE_RADIANS

#include "PerspectiveCamera.h"

/*
Engine camera based around implementation of perspective camera class.
Controls:
WASD to translate
SHIFT to speed up
Left click + MouseMove to move viewpoint.
*/
class EngineCamera : public PerspectiveCamera
{
public:
	/*
	Constructor
	int width, The viewport width in screen pixels.
	int height, The viewport height in screen pixels.
	float aspect, The viewport aspect ration in degrees. This value is doubled thus a value of 45 produces an FoV of 90.
	glm::vec3 pos, The position of the camera in world space. Defaulted to 0.0, 0.0, 1.0.
	glm::vec3 up, The up direction vector of the camera. Assumes vector has been normaised. Defaulted to 0.0, 1.0, 0.0.
	glm::vec3 direction, The forward direction vector of the camera. Assumes vector has been normaised. Defaulted to 0.0, 0.0, -1.0.
	*/
	EngineCamera(int width, int height, float aspect, glm::vec3 pos = glm::vec3(0.0, 0.0, 0.0), glm::vec3 up = glm::vec3(0.0, 1.0, 0.0), glm::vec3 direction = glm::vec3(0.0, 0.0, -1.0));

	/*
	Method to process mouse events. See EventListener interface.
	Camera tracks the mouse movement similar to first person cameras.
	Left click required to move using canMove bool
	*/
	void handle(MouseEvent event);
	/*
	Method to process key events. See EventListener interface.
	The 'W' and 'S' keys move the camera forward and backwards, relative to it's current direction, respectively.
	The 'A' and 'D' keys pan the camera left and right, relative to it's current direction, respectively.
	The SHIFT key speeds up translation using cameraSpeed.
	*/
	void handle(KeyEvent event);
protected:

private:
	bool		canMove;
	float		cameraSpeed;
	const float normalCameraSpeed = 0.5f;
};

#endif