#pragma once
#ifndef TRANSFORM_H
#define TRANSFORM_H
#include <gl\glm\glm\glm.hpp>
#include <gl\glm\glm\gtc\quaternion.hpp>
#include <gl\glm\glm\gtx\quaternion.hpp>
#include <Components\Component.h>

/*
	Represents a GameObjects position, scale and orientation.
*/
class Transform : public Component {
public:
	/*
		Default Constructor
	*/
	Transform() : Component(ComponentType::TRANSFORM) {
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		scale = glm::vec3(1.0f, 1.0f, 1.0f);
		orientation = glm::quat();
	};
	/*
		Constructor
		glm::vec3 p, The position vector.
		glm::vec3 s, The scale vector.
		glm::quat o, The orientation quaternion.
	*/
	Transform(glm::vec3 p, glm::vec3 s, glm::quat o) : Component(ComponentType::TRANSFORM) {
		position = p;
		scale = s;
		orientation = o;
	};

	/*
		Empty implementation of Componet::update()
	*/
	void update(double dt) {};
	/*
		Empty implementation of Componet::RecieveMessage()
	*/
	void RecieveMessage(Message* msg) {};

	void dispose() {};

	//public members
	glm::vec3 position;
	glm::vec3 scale;
	glm::quat orientation;

};

#endif // !TRANSFORM_H
