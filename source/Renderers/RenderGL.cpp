#include "Renderers\RenderGL.h"
#ifndef NDEBUG
#include <utils\GLSupport.h>
#endif
#if defined(_WIN32) || defined(_WIN64)
#include <gl\wglew.h>
#else
#include <gl\glxew.h>
#endif
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

RenderGL::RenderGL(int width, int height)
{
	this->width = width;
	this->height = height;
}

RenderGL::~RenderGL()
{
	
}

bool RenderGL::init()
{
	glewExperimental = true;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return false;
	}

	AssetManager::getInstance()->getShader(std::make_pair("lighting.vert", "lighting.frag"))->initialiseBoneUniforms();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	modelMat = glm::translate(modelMat, glm::vec3(0.0f, 0.0f, 0.0f));
	return true;
}

void RenderGL::prepare()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderGL::exit() {

}

int RenderGL::getWidth()
{
	return width;
}

int RenderGL::getHeight()
{
	return height;
}

void RenderGL::buildTextShader(unsigned int &vertArrayObj, unsigned int &vertBuffObj, shared_ptr<Shader> & textShader)
{
	glGenVertexArrays(1, &vertArrayObj);
	glGenBuffers(1, &vertBuffObj);
	glBindVertexArray(vertArrayObj);
	//create buffer for vertex and texture data
	glBindBuffer(GL_ARRAY_BUFFER, vertBuffObj);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	textShader = AssetManager::getInstance()->getShader(std::pair<string, string>("text.vert","text.frag"));
#ifndef NDEBUG
	string check = OpenGLSupport().GetError();
#endif
	glm::mat4 projection = glm::ortho(0.0f, (float)width, 0.0f, (float)height);
	textShader->setUniform("projection", projection);

#ifndef NDEBUG
	check = OpenGLSupport().GetError();
#endif

	textShader->setUniform("textColour", glm::vec4(1.0, 1.0, 1.0, 1.0));
	textShader->setUniform("tex", 0);
	glFlush();
}

void RenderGL::renderText(string& text, Font& font, shared_ptr<Transform>& transform, unsigned int VAO, unsigned int VBO, shared_ptr<Shader>& textShader, glm::vec3 colour)
{
	float charX = transform->position.x;
	float charY = transform->position.y;
	glEnable(GL_BLEND);
#ifndef NDEBUG
	string check = OpenGLSupport().GetError();
#endif
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	textShader->bindShader();
	textShader->setUniform("textColour", glm::vec4(colour,1.0));
#ifndef NDEBUG
	check = OpenGLSupport().GetError();
#endif
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);
	for (char c : text) {
		Font::Character character = font.getChar(c);
		GLfloat xpos = charX + character.bearing.x * transform->scale.x;
		GLfloat ypos = charY - (character.size.y - character.bearing.y) * transform->scale.y;

		GLfloat w = character.size.x * transform->scale.x;
		GLfloat h = character.size.y * transform->scale.y;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, character.texture);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		charX += (character.offset >> 6) *  transform->scale.x; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
#ifndef NDEBUG
	check = OpenGLSupport().GetError();
#endif
	glDisable(GL_BLEND);
}

void RenderGL::buildFontTexture(FT_Face& fontFace, unsigned int& textureID)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //Disable OpenGLs byte alignment restriction as Freetype uses 1 byte colour (grey scale)
	// Generate texture
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RED,
		fontFace->glyph->bitmap.width,
		fontFace->glyph->bitmap.rows,
		0,
		GL_RED,
		GL_UNSIGNED_BYTE,
		fontFace->glyph->bitmap.buffer
		);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFlush();
}

vector<unsigned int> RenderGL::bufferModelData(vector<glm::vec4>& vertices, vector<glm::vec3>& normals, vector<glm::vec2>& textures,
	vector<unsigned short>& indices, unsigned int& vaoHandle)
{
	unsigned int vboHandles[4];
	glGenBuffers(4, vboHandles);
	//Create IBO
	GLuint indiceBuffer = vboHandles[0];
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * indices.size(), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//Create VBO
	GLuint vertexBuffer = vboHandles[1];
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * vertices.size(), glm::value_ptr(vertices[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Create VBO for textureCoords
	GLuint texCoordBuffer = vboHandles[2];
	if (textures.size() > 0) 
	{
		glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * textures.size(), glm::value_ptr(textures[0]), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	//Creat VBO for normals
	GLuint normalBuffer = vboHandles[3];
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), glm::value_ptr(normals[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	vector<unsigned int> handles;
	handles.push_back(vertexBuffer);
	handles.push_back(texCoordBuffer);
	handles.push_back(normalBuffer);
	handles.push_back(indiceBuffer);
	glFlush();
	return handles;
}

void RenderGL::bufferModelData(shared_ptr<ModelData> data)
{
#ifndef NDEBUG
	string check = OpenGLSupport().GetError();
#endif
	unsigned int vboHandles[2];
	glGenBuffers(2, vboHandles);
	glBindBuffer(GL_ARRAY_BUFFER, vboHandles[0]);
	glBufferData(GL_ARRAY_BUFFER, data->vertices.size() * sizeof(ModelData::Vertex),
		&data->vertices[0], GL_STATIC_DRAW);
#ifndef NDEBUG
	check = OpenGLSupport().GetError();
#endif
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboHandles[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, data->indices.size() * sizeof(GLuint),
		&data->indices[0], GL_STATIC_DRAW);

#ifndef NDEBUG
	check = OpenGLSupport().GetError();
#endif
	vector<unsigned int> handles;
	handles.push_back(vboHandles[0]);
	handles.push_back(vboHandles[1]);
	data->vboHandles = handles;
	glFlush();
#ifndef NDEBUG
	check = OpenGLSupport().GetError();
#endif
}

void RenderGL::bufferLightingData(vector<Light>& lights, shared_ptr<Shader> &shader, unsigned int& uniformBuffer, unsigned int& bindingPoint)
{
	shader->bindShader();
	vector<glm::vec4> data;
	numOfLights = 0;
	for (Light currLight : lights)
	{
		//GLSL pads vec3 as vec4 so when buffering store them as vec4.
		data.push_back(glm::vec4(currLight.pos, 1.0));
		data.push_back(glm::vec4(currLight.ambient, 1.0));
		data.push_back(glm::vec4(currLight.diffuse, 1.0));
		data.push_back(glm::vec4(currLight.specular, 1.0));
		numOfLights++;
		if (numOfLights >= MAX_NUM_LIGHTS) break; //Only support MAX_NUM_LIGHTS lights
	}
	shader->setUniform("NUM_LIGHTS", numOfLights);
	if (bindingPoint >= 0 && bindingPoint <= currBindingPoint) //If buffer has already been created, just update the data
	{
		glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::vec4) * data.size(), glm::value_ptr(data[0]), GL_DYNAMIC_DRAW);
		return;
	}
	shader->bindUniformBlock("LightingBlock", currBindingPoint);
	glGenBuffers(1, &uniformBuffer);
	glBindBuffer(GL_UNIFORM_BUFFER, uniformBuffer);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::vec4) * data.size(), glm::value_ptr(data[0]), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, currBindingPoint, uniformBuffer);
	glFlush();
	bindingPoint = currBindingPoint;
	currBindingPoint++;
}

unsigned int RenderGL::createVertexArrayObject(vector<unsigned int>& vboHandles)
{
	if (vboHandles.size() == 4)
	{
		unsigned int vaoHandle;
		//Create Vertex Array Object
		glGenVertexArrays(1, &vaoHandle);
		glBindVertexArray(vaoHandle);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		//Bind VBOs to VAO
		glBindBuffer(GL_ARRAY_BUFFER, vboHandles[0]);
		glVertexAttribPointer(0, 4, GL_FLOAT, false, 0, (GLubyte *)NULL);
		glBindBuffer(GL_ARRAY_BUFFER, vboHandles[1]);
		glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, (GLubyte *)NULL);
		glBindBuffer(GL_ARRAY_BUFFER, vboHandles[2]);
		glVertexAttribPointer(2, 3, GL_FLOAT, false, 0, (GLubyte *)NULL);
		//Bind IBO to VAO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboHandles[3]);
		return vaoHandle;
	}
	else
	{
		unsigned int vaoHandle;
		//Create Vertex Array Object
		glGenVertexArrays(1, &vaoHandle);
		glBindVertexArray(vaoHandle);
		glBindBuffer(GL_ARRAY_BUFFER, vboHandles[0]);
		// Vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(ModelData::Vertex),
			(GLvoid*)0);
		// Vertex Normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ModelData::Vertex),
			(GLvoid*)offsetof(ModelData::Vertex, Normal));
		// Vertex Texture Coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ModelData::Vertex),
			(GLvoid*)offsetof(ModelData::Vertex, TexCoords));
		//Ambient 
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ModelData::Vertex),
			(GLvoid*)offsetof(ModelData::Vertex, Ka));
		//Diffuse
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(ModelData::Vertex),
			(GLvoid*)offsetof(ModelData::Vertex, Kd));
		//Specular
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(ModelData::Vertex),
			(GLvoid*)offsetof(ModelData::Vertex, Shininess));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboHandles[1]);
		return vaoHandle;
	}
}

unsigned int RenderGL::createTextVertexArrayObject(unsigned int & vboHandle)
{
	unsigned int vertArrayObj;
	glGenVertexArrays(1, &vertArrayObj);
	glBindVertexArray(vertArrayObj);
	//create buffer for vertex and texture data
	glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	return vertArrayObj;
}

unsigned int RenderGL::createUIVertextArrayObject(unsigned int & vboHandle, unsigned int& eboHandle, vector<GLfloat> vertices, vector<GLuint> indices)
{
	unsigned int vertArrayObj;
	glGenVertexArrays(1, &vertArrayObj);
	glGenBuffers(1, &vboHandle);
	glGenBuffers(1, &eboHandle);

	glBindVertexArray(vertArrayObj);

	glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboHandle);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// TexCoord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	return vertArrayObj;
}

unsigned int RenderGL::createSkyboxVertexArrayObject(GLuint& vboHandle, std::vector<GLfloat> vertices)
{
	unsigned int arrayObject;

	glGenVertexArrays(1, &arrayObject);
	glBindVertexArray(arrayObject);
	glGenBuffers(1, &vboHandle);

	//load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, vboHandle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	//setup vertex attribute pointers
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindVertexArray(0);

	return arrayObject;
}

void RenderGL::renderModel(ModelComponent& model, shared_ptr<Shader>& shaderProgram, shared_ptr<Camera>& camera)
{
	vector<Light> defaultLights;
	defaultLights.push_back(Light());
	renderModel(model, shaderProgram, camera, defaultLights);
}

void RenderGL::renderModel(ModelComponent& model, shared_ptr<Shader>& shaderProgram, shared_ptr<Camera>& camera, vector<Light>& lights)
{
//#ifndef NDEBUG
//	string check = OpenGLSupport().GetError();
//#endif
//	shaderProgram->bindShader();
//#ifndef NDEBUG
//	check = OpenGLSupport().GetError();
//#endif
//	glBindVertexArray(model.getVertArray());
//	if (model.getTexture() != nullptr) {
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, model.getTexture()->object());
//		shaderProgram->setUniform("tex", 0);
//	}
//	else {
//		shaderProgram = AssetManager::getInstance()->getShader(std::pair<std::string, std::string>("phong_no_texture.vert", "phong_no_texture.frag"));
//		shaderProgram->bindShader();
//	}
//	Transform* transform = model.getTransform();
//	glm::quat orientation = transform->orientation;
//	glm::mat4 mMat = modelMat * glm::translate(transform->position) * glm::rotate(orientation.w, glm::vec3(orientation.x, orientation.y, orientation.z)) * glm::scale(transform->scale);
//	shaderProgram->setUniform("tex", 0);
//	shaderProgram->setUniform("mView", camera->getView());
//	shaderProgram->setUniform("mProjection", camera->getProjection());
//	shaderProgram->setUniform("mModel", mMat);
//	shaderProgram->setUniform("viewPos", camera->getPosition());
//	if (model.getMaterial().used)
//	{
//		shaderProgram->setUniform("material", model.getMaterial());
//	}
//	if (lights.size() > 0 && lights.size() <= MAX_NUM_LIGHTS)
//	{
//		shaderProgram->setUniform("lights", lights);
//		shaderProgram->setUniform("NUM_LIGHTS", (int)lights.size());
//	}
//	else
//	{
//		shaderProgram->setUniform("NUM_LIGHTS", 0);
//	}
//	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(model.getIndexSize()), GL_UNSIGNED_SHORT, BUFFER_OFFSET(0));
//#ifndef NDEBUG
//	check = OpenGLSupport().GetError();
//#endif
//	glBindVertexArray(0);
}

void RenderGL::renderModel(ModelComponent & model, shared_ptr<Shader>& shaderProgram, shared_ptr<Camera>& camera, unsigned int lightingBuffer, unsigned int lightingBlockId)
{
#ifndef NDEBUG
	string check = OpenGLSupport().GetError();
#endif
	shaderProgram->bindShader();
#ifndef NDEBUG
	check = OpenGLSupport().GetError();
#endif
	auto mesh = model.getData();
	auto material = model.getMaterial();
	if (model.getTexture() != nullptr) {
		shaderProgram->setUniform("isTextured", true);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, model.getTexture()->object());
		shaderProgram->setUniform("tex", 0);
	}
	else {
		shaderProgram->setUniform("isTextured", false);
		shaderProgram = AssetManager::getInstance()->getShader(std::pair<std::string, std::string>("lighting.vert", "lighting.frag"));
		shaderProgram->bindShader();
	}
	//Set number of lights
	numOfLights <= MAX_NUM_LIGHTS ? shaderProgram->setUniform("NUM_LIGHTS", numOfLights) : shaderProgram->setUniform("NUM_LIGHTS", 0);
	Transform* transform = model.getTransform();
	glm::quat orientation = transform->orientation;
	glm::mat4 mMat = glm::translate(transform->position) * glm::mat4_cast(orientation) * glm::scale(transform->scale);
	glm::mat4 mv = camera->getView() * mMat;

	shaderProgram->setUniform("view", camera->getView());
	shaderProgram->setUniform("projection", camera->getProjection());
	shaderProgram->setUniform("model", mMat);
	shaderProgram->setUniform("normal", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
	shaderProgram->setUniform("viewPos", camera->getPosition());
	shaderProgram->setUniform("material", mesh->material);

	glBindVertexArray(model.getData()->getVertArray(this));
	glBindBufferBase(GL_UNIFORM_BUFFER, lightingBlockId, lightingBuffer); //Bind lighting data
	for (unsigned int i = 0; i < mesh->meshes.size(); i++) 
	{
		glDrawElementsBaseVertex(GL_TRIANGLES,
			mesh->meshes[i].NumIndices,
			GL_UNSIGNED_INT,
			(void*)(sizeof(unsigned int) * mesh->meshes[i].BaseIndex),
			mesh->meshes[i].BaseVertex);
#ifndef NDEBUG
		check = OpenGLSupport().GetError();
#endif
	}
	
	glBindVertexArray(0);
}

void RenderGL::freeModel(ModelComponent & model)
{
	auto handles = model.getData()->vboHandles;
	glDeleteBuffers(handles.size(), &handles[0]);
}

void RenderGL::freeAnimatedModel(AnimatedModelComponent & model)
{
	std::vector<unsigned int> handles;
	auto meshes = model.getAllModels();
	for (auto mesh : meshes)
	{
		if (mesh)
		{
			handles = mesh->getVBOHandles();
			glDeleteBuffers(handles.size(), &handles[0]);
		}
	}
}

//TO DO
void RenderGL::renderModel(AnimatedModelComponent& model, shared_ptr<Shader>& shaderProgram, shared_ptr<Camera>& camera)
{
	vector<Light> defaultLights;
	defaultLights.push_back(Light());
	renderModel(model, shaderProgram, camera, defaultLights);
}
//TO DO
void RenderGL::renderModel(AnimatedModelComponent& model, shared_ptr<Shader>& shaderProgram, shared_ptr<Camera>& camera, vector<Light>& lights)
{

}

//TO DO - change animation shader to use light uniform blocks for multiple lights - address vertex array objects not existing across threads
//ability to change animation being played and user input to move the character - texture support?
void RenderGL::renderModel(AnimatedModelComponent& model, shared_ptr<Shader>& shaderProgram, shared_ptr<Camera>& camera, unsigned int lightingBuffer, unsigned int lightingBlockId)
{
	shaderProgram->bindShader();
	shaderProgram->setUniform("isTextured", false);
	//Set number of lights
	numOfLights <= MAX_NUM_LIGHTS ? shaderProgram->setUniform("NUM_LIGHTS", numOfLights) : shaderProgram->setUniform("NUM_LIGHTS", 0);
	Transform* transform = model.getTransform();
	glm::quat orientation = transform->orientation;
	glm::mat4 mMat = glm::translate(transform->position) * glm::mat4_cast(orientation) * glm::scale(transform->scale);
	glm::mat4 mv = camera->getView() * mMat;
	
	shaderProgram->setUniform("view", camera->getView());
	shaderProgram->setUniform("projection", camera->getProjection());
	shaderProgram->setUniform("model", mMat);
	shaderProgram->setUniform("normal", glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
	shaderProgram->setUniform("viewPos", camera->getPosition());
	//material for animated model is set within render function of animated model and not here

	glBindBufferBase(GL_UNIFORM_BUFFER, lightingBlockId, lightingBuffer); //Bind lighting data

	//Set the Teapot material properties in the shader and render
	shaderProgram->setUniform("animatedCharacter", true);
	model.getCurrentModel()->render();
	shaderProgram->setUniform("animatedCharacter", false);
#ifndef NDEBUG
	auto check = OpenGLSupport().GetError();
#endif		
}

void RenderGL::renderSkybox(shared_ptr<Skybox>& skybox, shared_ptr<Camera>& camera)
{
	glDepthFunc(GL_LEQUAL);

	if (skybox->initialized == false)
	{
		skybox->init();
	}

	shared_ptr<Shader> shader = AssetManager::getInstance()->getShader(std::make_pair("skybox.vert", "skybox.frag"));
	shader->bindShader();
	shader->setUniform("view", camera->getView());
	shader->setUniform("projection", camera->getProjection());

	glm::mat4 model = glm::mat4();
	model = glm::scale(model, glm::vec3(1500.f));
	shader->setUniform("model", model);

	glBindVertexArray(skybox->vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glDepthFunc(GL_LESS);
}

void RenderGL::setVSync(bool flag)
{
	int val = flag ? 1 : 0;
#if defined(_WIN32) || defined(_WIN64)
	if (WGL_EXT_swap_control)
	{
		wglSwapIntervalEXT(val);
	}
#else
	if (GLX_EXT_swap_control)
	{
		glXSwapIntervalEXT(val);
#endif
}

GLuint RenderGL::loadCubemapTexture(std::string& filePrefix)
{
	shared_ptr<Shader> shader = AssetManager::getInstance()->getShader(std::make_pair("skybox.vert", "skybox.frag"));
	shader->bindShader();

	glActiveTexture(GL_TEXTURE0); //active an opengl texture unit to use

	GLuint texID;
	glGenTextures(1, &texID); //generate a texture id and store in texID variable
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID); //binds a type cube map texture to the generated texID

	const std::string suffixes[] = { "right", "left", "top", "bottom", "back", "front" }; //suffixes for the image names

	//specifys the targets for each face of the cube, these are pre-defined types in opengl
	GLuint targets[] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};

	//Loops through each face, loading the bitmap texture and assigns that images to the corresponding target location
	for (int i = 0; i < 6; i++)
	{
		std::string textureFile = filePrefix + "_" + suffixes[i] + ".tga";
		Bitmap bmp = Bitmap::bitmapFromFile(textureFile);

		glTexImage2D(targets[i], 0, GL_RGB, bmp.width(), bmp.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, bmp.pixelBuffer());
	}

	//sets the properties to be used for the texture
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	//set the shader uniform	
	shader->setUniform("skybox", 0);

	return texID;
}