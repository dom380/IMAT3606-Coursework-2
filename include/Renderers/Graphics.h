#pragma once
#ifndef GRAPHICS_H
#define GRAPHICS_H
#include <Graphics\Shader.h>
#include <memory>
#include <Camera\Camera.h>
using std::shared_ptr;
#include <GUI\Font.h>
#include <Graphics\Transform.h>
#include <vector>
using std::vector;
class Font;
class ModelComponent;
class UIElement;
class AnimatedModelComponent;
class ModelData;
/*
	Interface for the Graphics system.
*/
class Graphics
{
public:
	/*
		Method called prior to rendering each frame. 
		Implementations can use this for per frame initialisation 
		e.g. clearing colour buffers etc.
	*/
	virtual void prepare() = 0;
	/*	
		Initialisation method called on start up.
		Implementations should perform one off and time consuming initalisation here.
	*/
	virtual bool init() = 0;
	/*
		Return the current window width.
	*/
	virtual int getWidth() = 0;
	/*
		Return the current window height.
	*/
	virtual int getHeight() = 0;
	/*	
		Build the requested model vertex buffer(s) 
		vector<glm::vec4>& vertices, A std::vector containing all the model's vertices
		vector<glm::vec3>& normals,  A std::vector containing all the model's normals
		vector<glm::vec2>& textures, A std::vector containing all the model's texture coordinates.
		vector<unsigned short>& indices, A std::vector containing all the model's indices
		unsigned int& vaoHandle, Will be set to the handle of a VertexArrayObject containing the created VertexArrayBuffers. 
		**Note** VertexArrayObject's are context specific in OpenGL implementations, meaning if this method is called from a background thread vaoHandle will be set to an incorrect value. 
		Returns a std::vector containing the VertexBufferObject handles created in Vertex,Texture,Normal,Indice order.
	*/
	virtual vector<unsigned int> bufferModelData(vector<glm::vec4>& vertices, vector<glm::vec3>& normals, vector<glm::vec2>& textures,
		vector<unsigned short>& indices, unsigned int& vaoHandle) = 0;
	/*
		Build the requested vertex buffer(s)
		shared_ptr<ModelData> data - A std::shared_ptr to a ModelData object containing the vertex, indices and material data.
	*/
	virtual void bufferModelData(shared_ptr<ModelData> data) = 0;
	/*
		Build the requested lighting data buffer.
		vector<Light>& lights, A std::vector of Lights to buffered.
		shared_ptr<Shader> &shader, The shader to bind the data to,
		unsigned int& uniformBuffer, The uniform buffer handle. Implementations must set this value.
		unsigned int& bindingPoint, The uniform block binding point. Implementations must set this value.
	*/
	virtual void bufferLightingData(vector<Light>& lights, shared_ptr<Shader> &shader, unsigned int& uniformBuffer, 
		unsigned int& bindingPoint) = 0;
	/*
		Creates a VertexArrayObject containing the specified VertexBufferObjects.
		vector<unsigned int>& vboHandles, std::vector of the VBO handles. MUST be supplied in Vertex,Texture,Normal,Indice order.
		returns the Id of hte created VertexArrayObject.
	*/
	virtual unsigned int createVertexArrayObject(vector<unsigned int>& vboHandles) = 0;
	/*	
		Initialise a shader for text rendering and construct a VertexBufferObject for text rendering.
		unsigned int &vertArrayObj, VertexArrayObject handle. Implementations must set this value.
		**Note** VertexArrayObject's are context specific in OpenGL implementations, meaning if this method is called from a background thread vertArrayObj will be set to an incorrect value.
		unsigned int &vertBuffObj, VertexBufferObject handle. Implementations must set this value to the constructed buffer handle
		shared_ptr<Shader> &textShader
	*/
	virtual void buildTextShader(unsigned int &vertArrayObj, unsigned int &vertBuffObj, shared_ptr<Shader> &textShader) = 0;
	/*
		Build the texture data for the requested freetype2 font face.
		FT_Face& fontFace, Bitmap of the glyph to load as a texture.
		unsigned int& textureID, The ID for the new texture. Implementations must set this.
		**Note** freetype2 uses 1 byte (greyscale) colour information.
	*/
	virtual void buildFontTexture(FT_Face& fontFace, unsigned int& textureID) = 0;

	/*
		Creates a VertexArrayObject for the supplied Text vertex buffer object.
		unsigned int& vboHandle, Implementations must set this to the created VertexArrayObject's handle;
	*/
	virtual unsigned int createTextVertexArrayObject(unsigned int& vboHandle) = 0;
	/*
		Creates a VAO for a UI object.
	*/
	virtual unsigned int createUIVertextArrayObject(unsigned int& vboHandle, unsigned int& eboHandle, vector<GLfloat> vertices, vector<GLuint> indices) = 0;
	/*	
		Render text.
		string& text, The string to render.
		Font& font, The font to use when rendering.
		shared_ptr<Transform>& transform, The positon, scale and orientation of the text.
		unsigned int VAO, The VertexArrayObject handle of the text buffer.
		unsigned int VBO, The VertexBufferObject handle of the text buffer.
		shared_ptr<Shader>& textShader, The shader to use when rendering the text.
		glm::vec3 colour, THe colour of the texture, normalised to 0..1. Defaulted to white (1.0,1.0,1.0).
	*/
	virtual void renderText(string& text, Font& font, shared_ptr<Transform>& transform, unsigned int VAO, unsigned int VBO, shared_ptr<Shader>& textShader, glm::vec3 colour = glm::vec3(1.0,1.0,1.0)) = 0;

	/*
		Render model.
		ModelComponent& model, The model to render.
		shared_ptr<Shader>& shaderProgram, The shader to use when rendering.
		shared_ptr<Camera>& camera, The camera to use when rendering.
	*/
	virtual void renderModel(ModelComponent& model, shared_ptr<Shader>& shaderProgram, shared_ptr<Camera>& camera) = 0;
	/*
		Render model.
		ModelComponent& model, The model to render.
		shared_ptr<Shader>& shaderProgram, The shader to use when rendering.
		shared_ptr<Camera>& camera, The camera to use when rendering.
		vector<Light>& lights, A std::vector of Lights in the scene.
	*/
	virtual void renderModel(ModelComponent& model, shared_ptr<Shader>& shaderProgram, shared_ptr<Camera>& camera, vector<Light>& lights) = 0;
	/*
		Render model.
		ModelComponent& model, The model to render.
		shared_ptr<Shader>& shaderProgram, The shader to use when rendering.
		shared_ptr<Camera>& camera, The camera to use when rendering.
		unsigned int lightingBuffer, The handle of the uniform buffer object containing the lighting data.
		unsigned int lightingBlockId, The Id of the uniform block binding point.
	*/
	virtual void renderModel(ModelComponent& model, shared_ptr<Shader>& shaderProgram, shared_ptr<Camera>& camera, unsigned int lightingBuffer, unsigned int lightingBlockId) = 0;

	//TO DO
	virtual void renderModel(AnimatedModelComponent& model, shared_ptr<Shader>& shaderProgram, shared_ptr<Camera>& camera) = 0;
	//TO DO
	virtual void renderModel(AnimatedModelComponent& model, shared_ptr<Shader>& shaderProgram, shared_ptr<Camera>& camera, vector<Light>& lights) = 0;
	//TO DO
	virtual void renderModel(AnimatedModelComponent& model, shared_ptr<Shader>& shaderProgram, shared_ptr<Camera>& camera, unsigned int lightingBuffer, unsigned int lightingBlockId) = 0;

	/*
		Enable or disable v-sync
	*/
	virtual void setVSync(bool flag) = 0;

	/*
		Method called on engine shutdown. 
		Perform any further clean up of resources here.
	*/
	virtual void exit() = 0;

};

#endif // !GRAPHICS_H
