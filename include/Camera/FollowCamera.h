#pragma once
#ifndef FOLLOWCAMERA_H
#define FOLLOWCAMERA_H
#define GLM_FORCE_RADIANS

#include "PerspectiveCamera.h"

/*
Follow camera based implementation of perspective camera class.
TODO: perhaps a floating camera, swerves in figure of 8?
*/
class FollowCamera : public PerspectiveCamera
{
public:
	/*
	Constructor
	int width, The viewport width in screen pixels.
	int height, The viewport height in screen pixels.
	float aspect, The viewport aspect ration in degrees. This value is doubled thus a value of 45 produces an FoV of 90.
	glm::vec3 targetDistance, the distance between the camera and the target.
	glm::vec3 pos, The position of the camera in world space. Defaulted to 0.0, 0.0, 1.0.
	glm::vec3 up, The up direction vector of the camera. Assumes vector has been normaised. Defaulted to 0.0, 1.0, 0.0.
	glm::vec3 direction, The forward direction vector of the camera. Assumes vector has been normaised. Defaulted to 0.0, 0.0, -1.0.
	*/
	FollowCamera(int width, int height, float aspect, glm::vec3 targetDistance, glm::vec3 pos = glm::vec3(0.0, 0.0, 0.0), glm::vec3 up = glm::vec3(0.0, 1.0, 0.0), glm::vec3 direction = glm::vec3(0.0, 0.0, -1.0));

	/*
	Points the camera at the specified direction vector.
	Assumes the vector has been normalised.
	float x, The x component of the vector.
	float y, The y component of the vector.
	float z, The z component of the vector.
	*/
	void lookAt(float x, float y, float z);

	/*
	Points the camera at the specified direction vector.
	Assumes the vector has been normalised.
	glm::vec3 target, The 3 component target vector.
	*/
	void lookAt(glm::vec3 target);
	
	/*
		Override parent move
		We want to have a distance to target.
	*/
	void move(glm::vec3 Position);

	/*
	TODO: Possible future handling required

	Method to process mouse events. See EventListener interface.
	Camera tracks the mouse movement similar to first person cameras.
	*/
	//void handle(MouseEvent event);
	/*
	Method to process key events. See EventListener interface.
	The 'W' and 'S' keys move the camera forward and backwards, relative to it's current direction, respectively.
	The 'A' and 'D' keys pan the camera left and right, relative to it's current direction, respectively.
	*/
	//void handle(KeyEvent event);
protected:
	
private:
	glm::vec3 distanceToTarget;

};

#endif