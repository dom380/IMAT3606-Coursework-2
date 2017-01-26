#pragma once
#ifndef MATERIAL_H
#define MATERIAL_H
#include<gl\glm\glm\glm.hpp>
/*
	Class representing a model's Material properties.
*/
class Material 
{
public:
	/*
		Default constructor.
	*/
	Material() {};
	/*
		Constructor.
		glm::vec3 Ka, The ambient RGB refection intensity. Values must be normalised to 0..1.
		glm::vec3 Kd, The diffuse RGB refection intensity. Values must be normalised to 0..1.
		glm::vec3 Ks, The specular RGB refection intensity. Values must be normalised to 0..1.
		float d, The transparency of the material. Value must be normalised to 0..1.
		float Ns The specular exponent. 
	*/
	Material(glm::vec3 Ka, glm::vec3 Kd, glm::vec3 Ks, float d, float Ns)
	{
		this->Ka = Ka;
		this->Kd = Kd;
		this->Ks = Ks;
		this->d = d;
		this->Ns = Ns;
		this->used = true;
	}
	//public memeber
	glm::vec3 Ka;
	glm::vec3 Kd;
	glm::vec3 Ks;
	float d;
	float Ns;
	bool used = false; //Defaulted to not being used.
};
#endif // !MATERIAL_H
