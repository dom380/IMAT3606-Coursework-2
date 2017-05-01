#pragma once
#ifndef CAMERA_H
#define CAMERA_H
#include <gl\glm\glm\glm.hpp>
#include <GUI\EventListener.h>
#include <string>
/*
	virtual base class for camera implementations.
*/
class Camera : public EventListener
{
public:
	/*
		Points the camera at the specified direction vector.
		Assumes the vector has been normalised.
		float x, The x component of the vector.
		float y, The y component of the vector.
		float z, The z component of the vector.
	*/
	virtual void lookAt(float x, float y, float z) = 0;
	/*
		Points the camera at the specified direction vector.
		Assumes the vector has been normalised.
		glm::vec3 target, The 3 component target vector.
	*/
	virtual void lookAt(glm::vec3 target) = 0;
	/*
		Moves the camera's position in world space and update the view matrix.
		glm::vec3 newPos, The cameras new position vector.
	*/
	virtual void move(glm::vec3 newPos)
	{
		position = newPos;
		update();
	}
	/*
		Moves the camera's position in world space and update the view matrix.
		float x, The x component of the vector.
		float y, The y component of the vector.
		float z, The z component of the vector.
	*/
	virtual void move(float x, float y, float z)
	{
		position = glm::vec3(x,y,z);
		update();
	}
	/*
		Return the current view matrix.
	*/
	glm::mat4 getView()
	{
		return view;
	}
	/*
		Return the current position in world coordinates as a vector.
	*/
	glm::vec3 getPosition()
	{
		return position;
	}
	/*
		Returns the projection matrix.
	*/
	glm::mat4 getProjection()
	{
		return projection;
	}
	/*
		Returns the camera's ID.
	*/
	std::string getId()
	{
		return id;
	}
	/*
		Sets the camera's ID.
	*/
	void setId(std::string newID)
	{
		id = newID;
	}
	/*
		Enum specifying the Camera type.
	*/
	enum CameraClass
	{
		PERSPECTIVE,
		ORTHOGRAPHIC,
		ENGINE,
		FOLLOW
	};
	CameraClass getCameraType()
	{
		return type;
	}
protected:
	//protected functions
	/*
		Update camera's view matrix after any property has been changed.
	*/
	virtual void update() = 0;
	//protected members
	glm::mat4 view;
	glm::vec3 frontDir;
	glm::vec3 upDir;
	glm::vec3 rightDir;
	glm::vec3 position;
	glm::mat4 projection;
	CameraClass type;
	std::string id;

};
#endif // !CAMERA_H
