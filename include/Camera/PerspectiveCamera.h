#pragma once
#ifndef PERSPECTIVECAMERA_H
#define PERSPECTIVECAMERA_H
#define GLM_FORCE_RADIANS

#include "Camera.h"
#include <gl\glm\glm\gtc\matrix_transform.hpp>
#include <gl\glm\glm\glm.hpp>
#include <gl\glm\glm\gtc\type_ptr.hpp>

/*
	Perspective projection based implementation of Camera class.
*/
class PerspectiveCamera : public Camera
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
	PerspectiveCamera(int width, int height, float aspect, glm::vec3 pos = glm::vec3(0.0, 0.0, 1.0), glm::vec3 up = glm::vec3(0.0, 1.0, 0.0), glm::vec3 direction = glm::vec3(0.0, 0.0, -1.0));
	/*
		Updates the view matrix.
	*/
	void update();
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
		Method to process mouse events. See EventListener interface.
		Camera tracks the mouse movement similar to first person cameras.
	*/
	void handle(MouseEvent event);
	/*
		Method to process key events. See EventListener interface.
		The 'W' and 'S' keys move the camera forward and backwards, relative to it's current direction, respectively. 
		The 'A' and 'D' keys pan the camera left and right, relative to it's current direction, respectively.
	*/
	void handle(KeyEvent event);
protected:
	//protected members
	glm::vec3 worldY = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 worldX = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 worldZ = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::quat orientation;

private:
	//private methods
	/*
		Build a quaternion rotation.
		glm::vec3 axis, The arbitrary axis to rotate around.
		float angle, The amount to rotate by in radians.
	*/
	glm::quat getRotation(glm::vec3 axis, float angle);
	//private members
	int previousX, previousY;

};

#endif