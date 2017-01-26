#pragma once
#ifndef LIGHT_H
#define LIGHT_H
#include <gl\glm\glm\glm.hpp>

/*
	Class representing a light within the environment.
*/
class Light
{
public:
	/*
		Default Constructor.
	*/
	Light() {
		pos = glm::vec3(0.0,0.0,0.0);
		ambient = glm::vec3(0.0, 0.0, 0.0);
		diffuse = glm::vec3(0.0, 0.0, 0.0);
		specular = glm::vec3(0.0, 0.0, 0.0);
	};
	/*
		Constructor.
		glm::vec3 position, The position in world coordinates of the light.
		glm::vec3 ambient, The ambient RGB intensity of the light. Values must be normalised to 0..1.
		glm::vec3 diffuse, The diffuse RGB intensity of the light. Values must be normalised to 0..1.
		glm::vec3 specular, The specular RGB intensity of the light. Values must be normalised to 0..1.
		bool enabled = false, Boolean flag specifying whether the light in enabled in the scene. Defaulted to false.
	*/
	Light(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, bool enabled = false)
	{
		this->pos = position;
		this->ambient = ambient;
		this->diffuse = diffuse;
		this->specular = specular;
		this->enabled = enabled;
	};
	//Public member
	glm::vec3 pos;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	bool enabled = false;
};

#endif // !LIGHT_H
