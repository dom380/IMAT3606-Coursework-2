#pragma once
#ifndef RENDERGL_H
#define RENDERGL_H
#include "Graphics.h"
#include <gl\glew.h>
#include "GL\glm\glm\gtc\matrix_transform.hpp"
#include "GL\glm\glm\gtx\transform.hpp"
#include <string>
#include <Graphics\Shader.h>
#include <Graphics\ModelComponent.h>
#include "Components/AnimatedModelComponent.h"
#include <vector>
using std::string; using std::vector;
#include <memory>
using std::shared_ptr;
/*
	The OpenGL implementation of the Graphics system.
*/
class RenderGL : public Graphics {
public:
	/*
		Constructor
		int width, Width of the window in pixels
		int height, Height of the window in pixels.
	*/
	RenderGL(int width, int height);
	~RenderGL();
	/*
		Initialises GLEW and sets up Depth and Colour buffers.
	*/
	bool init();
	/*
		Clears the Depth and Colour buffers,
	*/
	void prepare();
	/*
		Cleans up any resources.
	*/
	void exit();
	/*
		Returns the window width.
	*/
	int getWidth();
	/*
		Returns the window height.
	*/
	int getHeight();
	/*
		Builds a GLSL Shader to handle text rendering. See interface for details.
	*/
	void buildTextShader(unsigned int &vertArrayObj, unsigned int &vertBuffObj, shared_ptr<Shader> &textShader);
	/*
		Renders supplied text. See interface for details.
	*/
	void renderText(string& text, Font& font, shared_ptr<Transform>& transform, unsigned int VAO, unsigned int VBO, shared_ptr<Shader>& textShader, glm::vec3 colour = glm::vec3(1.0, 1.0, 1.0));
	/*
		Creates a texture for the requested font face. See interface for details.
	*/
	void buildFontTexture(FT_Face& fontFace, unsigned int& textureID);
	/*
		Bufferes the requested Model data. See interface for details.
	*/
	vector<unsigned int> bufferModelData(vector<glm::vec4>& vertices, vector<glm::vec3>& normals, vector<glm::vec2>& textures,
		vector<unsigned short>& indices, unsigned int& vaoHandle);
	/*
		Buffers the requrest Lighting data into a Uniform block. See interface for details.
	*/
	void bufferLightingData(vector<Light>& lights, shared_ptr<Shader> &shader, unsigned int& uniformBuffer,
		unsigned int& bindingPoint);
	/*
		Creates a VertexArrayObject for the supplied VertexBufferObjects. See interface for details.
	*/
	unsigned int createVertexArrayObject(vector<unsigned int>& vboHandles);
	/*
		Creates a VertexArrayObject for the supplied text VertexBufferObject. See interface for details.
	*/
	unsigned int createTextVertexArrayObject(unsigned int& vboHandle);
	/*
	
	*/
	unsigned int createUIVertextArrayObject(unsigned int& vboHandle, unsigned int& eboHandle, vector<GLfloat> vertices, vector<GLuint> indices);
	/*
		Renders the requested model. See interface for details.
	*/
	void renderModel(ModelComponent& model, shared_ptr<Shader>& shaderProgram, shared_ptr<Camera>& camera);
	/*
		Renders the requested model. See interface for details.
	*/
	void renderModel(ModelComponent& model, shared_ptr<Shader>& shaderProgram, shared_ptr<Camera>& camera, vector<Light>& lights);
	/*
		Renders the requested model. See interface for details.
	*/
	void renderModel(ModelComponent& model, shared_ptr<Shader>& shaderProgram, shared_ptr<Camera>& camera, unsigned int lightingBuffer, unsigned int lightingBlockId);


	//TO DO
	void renderModel(AnimatedModelComponent& model, shared_ptr<Shader>& shaderProgram, shared_ptr<Camera>& camera);
	//TO DO
	void renderModel(AnimatedModelComponent& model, shared_ptr<Shader>& shaderProgram, shared_ptr<Camera>& camera, vector<Light>& lights);
	//TO DO
	void renderModel(AnimatedModelComponent& model, shared_ptr<Shader>& shaderProgram, shared_ptr<Camera>& camera, unsigned int lightingBuffer, unsigned int lightingBlockId);

private:
	//private members
	int width;
	int height;
	int numOfLights;
	const int MAX_NUM_LIGHTS = 10;
	unsigned int currBindingPoint = 0;
	glm::mat4 modelMat = glm::mat4();

	GLuint shadowFBO, pass1Index, pass2Index;
	int shadowMapWidth, shadowMapHeight;
	glm::mat4 lightPV;
	glm::mat4 shadowBias;
};

#endif // !RENDERGL_H

