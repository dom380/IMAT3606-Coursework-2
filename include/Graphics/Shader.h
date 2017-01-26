#pragma once
#ifndef SHADER_H
#define SHADER_H
#include <gl\glew.h>
#include <GL\glm\glm\glm.hpp>
#include <gl\glm\glm\gtc\type_ptr.hpp>
#include <Graphics\Light.h>
#include <Graphics\Material.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
using glm::vec2; using glm::vec3; using glm::vec4; using glm::mat3; using glm::mat4;
using std::fstream; using std::ifstream; using std::string; using std::vector;

/*
	Class to wrap a GLSL Shader.
	TODO - Make abstract and offer GLSL ans HLSL implementations.
*/
class Shader {
public:
	/*
		Constructor
	*/
	Shader();
	~Shader();
	/*
		Compiles and attaches the specified shader to the program.
		const char* filePath, File Path to the shader.
		GLenum shaderType, Enum specifying the type of shader.
		Returns true if shader compiled successfully.
	*/
	bool compileShader(const char* filePath, GLenum shaderType);
	/*
		Links the shader program.
		Returns true if shader linked successfully.
	*/
	bool link();

	/*
		Sets this shader program to the currently active one.
	*/
	void bindShader();
	/*
		Method to pass data through to the shader program.
		const char *name, Name of the uniform in the shader.
		const vec2 & v, Value to pass through.
	*/
	void   setUniform(const char *name, const vec2 & v);
	/*
		Method to pass data through to the shader program.
		const char *name, Name of the uniform in the shader.
		const vec3 & v, Value to pass through.
	*/
	void   setUniform(const char *name, const vec3 & v);
	/*
		Method to pass data through to the shader program.
		const char *name, Name of the uniform in the shader.
		const vec4 & v, Value to pass through.
	*/
	void   setUniform(const char *name, const vec4 & v);
	/*
		Method to pass data through to the shader program.
		const char *name, Name of the uniform in the shader.
		const mat4 & v, Value to pass through.
	*/
	void   setUniform(const char *name, const mat4 & m);
	/*
		Method to pass data through to the shader program.
		const char *name, Name of the uniform in the shader.
		const mat3 & v, Value to pass through.
	*/
	void   setUniform(const char *name, const mat3 & m);
	/*
		Method to pass data through to the shader program.
		const char *name, Name of the uniform in the shader.
		const float val, Value to pass through.
	*/
	void   setUniform(const char *name, float val);
	/*
		Method to pass data through to the shader program.
		const char *name, Name of the uniform in the shader.
		const int val, Value to pass through.
	*/
	void   setUniform(const char *name, int val);
	/*
		Method to pass data through to the shader program.
		const char *name, Name of the uniform in the shader.
		const bool val, Value to pass through.
	*/
	void   setUniform(const char *name, bool val);
	/*
		Method to pass data through to the shader program.
		const char *name, Name of the uniform in the shader.
		const GLuint val, Value to pass through.
	*/
	void   setUniform(const char *name, GLuint val);
	/*
		Method to pass data through to the shader program.
		const char *name, Name of the uniform in the shader.
		const Light val, Value to pass through.
	*/
	void   setUniform(const char *name, Light val);
	/*
		Method to pass data through to the shader program.
		const char *name, Name of the uniform in the shader.
		const vector<Light> vals, Values to pass through.
	*/
	void   setUniform(const char *name, vector<Light> vals);
	/*
		Method to pass data through to the shader program.
		const char *name, Name of the uniform in the shader.
		const Material & val, Value to pass through.
	*/
	void   setUniform(const char *name, Material val);
	
	/*
		Binds the specified uniform block to the supplied binding point.
		const char* blockName, The name of the uniform block.
		unsigned int& bindingPoint, The binding point. 
	*/
	void   bindUniformBlock(const char* blockName, unsigned int& bindingPoint);

private:
	GLint programHandle = 0;
	bool linked;

};

#endif // !SHADER_H
