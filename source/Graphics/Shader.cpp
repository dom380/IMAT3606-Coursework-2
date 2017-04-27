#include "Graphics\Shader.h"
#include "utils\GLSupport.h"
#include <gl\glm\glm\gtc\type_ptr.hpp>

Shader::Shader()
{
	linked = false;
	programHandle = 0;
}

Shader::~Shader()
{
	if (programHandle <= 0) {
		return;
	}
	//Get the number of and Ids of attached shaders
	GLint numShaders = 0;
	glGetProgramiv(programHandle, GL_ATTACHED_SHADERS, &numShaders);
	GLuint* shaderNames = new GLuint[numShaders];
	glGetAttachedShaders(programHandle, numShaders, NULL, shaderNames);
	for (int i = 0; i < numShaders; ++i) {
		glDeleteShader(shaderNames[i]);
	}
	glDeleteProgram(programHandle);
	delete[] shaderNames;
}

bool Shader::compileShader(const char * filePath, GLenum shaderType)
{
	if (programHandle <= 0) {
		programHandle = glCreateProgram();
	}

	ifstream inFile(filePath);
	if (!inFile) {
		fprintf(stderr, "Error opening file: %s\n", filePath);
		throw std::runtime_error("Failed to find shader");
	}
	std::stringstream code;
	code << inFile.rdbuf();
	inFile.close();
	string codeStr(code.str());
	GLuint shaderHandle = glCreateShader(shaderType);

	const char * c_code = codeStr.c_str();
	glShaderSource(shaderHandle, 1, &c_code, NULL);

	// Compile the shader
	glCompileShader(shaderHandle);
	//Check result
	int result;
	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &result);
	if (result == false) {
		fprintf(stderr, "Error compiling file: %s\n", filePath);
		GLint logLen;
		glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0) {
			char * log = (char *)malloc(logLen);
			GLsizei written;
			glGetShaderInfoLog(shaderHandle, logLen, &written, log);
			fprintf(stderr, "Shader log: \n%s", log);
			free(log);
		}
		return false;
	}
	else {
		glAttachShader(programHandle, shaderHandle);
	}
	return true;
}

bool Shader::link()
{
	if (linked) {
		return true;
	}
	if (programHandle <= 0) {
		fprintf(stderr, "Error linking program. Shader program has not been compiled.\n");
		return false;
	}
	glLinkProgram(programHandle);
	int result;
	glGetProgramiv(programHandle, GL_LINK_STATUS, &result);
	if (result == false) {
		fprintf(stderr, "Error linking program file.\n");
		GLint logLen;
		glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0) {
			char * log = (char *)malloc(logLen);
			GLsizei written;
			glGetProgramInfoLog(programHandle, logLen, &written, log);
			fprintf(stderr, "Program log: \n%s", log);
			free(log);
		}
		return false;
	}
	linked = true;

	/*struct glsl_type_set {
		GLenum      type;
		const char* name;
	}
	type_set[] = {
		GL_INVALID_ENUM,                              "invalid",
		GL_FLOAT,                                     "float",
		GL_FLOAT_VEC2,                                "vec2",
		GL_FLOAT_VEC3,                                "vec3",
		GL_FLOAT_VEC4,                                "vec4",
		GL_DOUBLE,                                    "double",
		GL_DOUBLE_VEC2,                               "dvec2",
		GL_DOUBLE_VEC3,                               "dvec3",
		GL_DOUBLE_VEC4,                               "dvec4",
		GL_INT,                                       "int",
		GL_INT_VEC2,                                  "ivec2",
		GL_INT_VEC3,                                  "ivec3",
		GL_INT_VEC4,                                  "ivec4",
		GL_UNSIGNED_INT,                              "unsigned int",
		GL_UNSIGNED_INT_VEC2,                         "uvec2",
		GL_UNSIGNED_INT_VEC3,                         "uvec3",
		GL_UNSIGNED_INT_VEC4,                         "uvec4",
		GL_BOOL,                                      "bool",
		GL_BOOL_VEC2,                                 "bvec2",
		GL_BOOL_VEC3,                                 "bvec3",
		GL_BOOL_VEC4,                                 "bvec4",
		GL_FLOAT_MAT2,                                "mat2",
		GL_FLOAT_MAT3,                                "mat3",
		GL_FLOAT_MAT4,                                "mat4",
		GL_FLOAT_MAT2x3,                              "mat2x3",
		GL_FLOAT_MAT2x4,                              "mat2x4",
		GL_FLOAT_MAT3x2,                              "mat3x2",
		GL_FLOAT_MAT3x4,                              "mat3x4",
		GL_FLOAT_MAT4x2,                              "mat4x2",
		GL_FLOAT_MAT4x3,                              "mat4x3",
		GL_DOUBLE_MAT2,                               "dmat2",
		GL_DOUBLE_MAT3,                               "dmat3",
		GL_DOUBLE_MAT4,                               "dmat4",
		GL_DOUBLE_MAT2x3,                             "dmat2x3",
		GL_DOUBLE_MAT2x4,                             "dmat2x4",
		GL_DOUBLE_MAT3x2,                             "dmat3x2",
		GL_DOUBLE_MAT3x4,                             "dmat3x4",
		GL_DOUBLE_MAT4x2,                             "dmat4x2",
		GL_DOUBLE_MAT4x3,                             "dmat4x3",
		GL_SAMPLER_1D,                                "sampler1D",
		GL_SAMPLER_2D,                                "sampler2D",
		GL_SAMPLER_3D,                                "sampler3D",
		GL_SAMPLER_CUBE,                              "samplerCube",
		GL_SAMPLER_1D_SHADOW,                         "sampler1DShadow",
		GL_SAMPLER_2D_SHADOW,                         "sampler2DShadow",
		GL_SAMPLER_1D_ARRAY,                          "sampler1DArray",
		GL_SAMPLER_2D_ARRAY,                          "sampler2DArray",
		GL_SAMPLER_1D_ARRAY_SHADOW,                   "sampler1DArrayShadow",
		GL_SAMPLER_2D_ARRAY_SHADOW,                   "sampler2DArrayShadow",
		GL_SAMPLER_2D_MULTISAMPLE,                    "sampler2DMS",
		GL_SAMPLER_2D_MULTISAMPLE_ARRAY,              "sampler2DMSArray",
		GL_SAMPLER_CUBE_SHADOW,                       "samplerCubeShadow",
		GL_SAMPLER_BUFFER,                            "samplerBuffer",
		GL_SAMPLER_2D_RECT,                           "sampler2DRect",
		GL_SAMPLER_2D_RECT_SHADOW,                    "sampler2DRectShadow",
		GL_INT_SAMPLER_1D,                            "isampler1D",
		GL_INT_SAMPLER_2D,                            "isampler2D",
		GL_INT_SAMPLER_3D,                            "isampler3D",
		GL_INT_SAMPLER_CUBE,                          "isamplerCube",
		GL_INT_SAMPLER_1D_ARRAY,                      "isampler1DArray",
		GL_INT_SAMPLER_2D_ARRAY,                      "isampler2DArray",
		GL_INT_SAMPLER_2D_MULTISAMPLE,                "isampler2DMS",
		GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY,          "isampler2DMSArray",
		GL_INT_SAMPLER_BUFFER,                        "isamplerBuffer",
		GL_INT_SAMPLER_2D_RECT,                       "isampler2DRect",
		GL_UNSIGNED_INT_SAMPLER_1D,                   "usampler1D",
		GL_UNSIGNED_INT_SAMPLER_2D,                   "usampler2D",
		GL_UNSIGNED_INT_SAMPLER_3D,                   "usampler3D",
		GL_UNSIGNED_INT_SAMPLER_CUBE,                 "usamplerCube",
		GL_UNSIGNED_INT_SAMPLER_1D_ARRAY,             "usampler2DArray",
		GL_UNSIGNED_INT_SAMPLER_2D_ARRAY,             "usampler2DArray",
		GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE,       "usampler2DMS",
		GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY, "usampler2DMSArray",
		GL_UNSIGNED_INT_SAMPLER_BUFFER,               "usamplerBuffer",
		GL_UNSIGNED_INT_SAMPLER_2D_RECT,              "usampler2DRect",
		GL_IMAGE_1D,                                  "image1D",
		GL_IMAGE_2D,                                  "image2D",
		GL_IMAGE_3D,                                  "image3D",
		GL_IMAGE_2D_RECT,                             "image2DRect",
		GL_IMAGE_CUBE,                                "imageCube",
		GL_IMAGE_BUFFER,                              "imageBuffer",
		GL_IMAGE_1D_ARRAY,                            "image1DArray",
		GL_IMAGE_2D_ARRAY,                            "image2DArray",
		GL_IMAGE_2D_MULTISAMPLE,                      "image2DMS",
		GL_IMAGE_2D_MULTISAMPLE_ARRAY,                "image2DMSArray",
		GL_INT_IMAGE_1D,                              "iimage1D",
		GL_INT_IMAGE_2D,                              "iimage2D",
		GL_INT_IMAGE_3D,                              "iimage3D",
		GL_INT_IMAGE_2D_RECT,                         "iimage2DRect",
		GL_INT_IMAGE_CUBE,                            "iimageCube",
		GL_INT_IMAGE_BUFFER,                          "iimageBuffer",
		GL_INT_IMAGE_1D_ARRAY,                        "iimage1DArray",
		GL_INT_IMAGE_2D_ARRAY,                        "iimage2DArray",
		GL_INT_IMAGE_2D_MULTISAMPLE,                  "iimage2DMS",
		GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY,            "iimage2DMSArray",
		GL_UNSIGNED_INT_IMAGE_1D,                     "uimage1D",
		GL_UNSIGNED_INT_IMAGE_2D,                     "uimage2D",
		GL_UNSIGNED_INT_IMAGE_3D,                     "uimage3D",
		GL_UNSIGNED_INT_IMAGE_2D_RECT,                "uimage2DRect",
		GL_UNSIGNED_INT_IMAGE_CUBE,                   "uimageCube",
		GL_UNSIGNED_INT_IMAGE_BUFFER,                 "uimageBuffer",
		GL_UNSIGNED_INT_IMAGE_1D_ARRAY,               "uimage1DArray",
		GL_UNSIGNED_INT_IMAGE_2D_ARRAY,               "uimage2DArray",
		GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE,         "uimage2DMS",
		GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY,   "uimage2DMSArray",
		GL_UNSIGNED_INT_ATOMIC_COUNTER,               "atomic_uint"
	};
	GLint uniform_count;
	glGetProgramiv(programHandle, GL_ACTIVE_UNIFORMS, &uniform_count);

	GLchar name[256];

	for (GLint i = 0; i < uniform_count; i++) {
		memset(name, '\0', 256);
		GLint  size;
		GLenum type;

		glGetActiveUniform(programHandle, i, 255, NULL, &size, &type, name);

		GLint location = glGetUniformLocation(programHandle, name);

		for (int j = 0; j < sizeof(type_set) / sizeof(glsl_type_set); j++) {
			if (type_set[j].type != type)
				continue;

			const char* type_name = type_set[j].name;

			if (size > 1)
				printf("Uniform %d (loc=%d):\t%20s %-20s <Size: %d>\n",
					i, location, type_name, name, size);
			else
				printf("Uniform %d (loc=%d):\t%20s %-20s\n",
					i, location, type_name, name);

			break;
		}

		if (i == (uniform_count - 1))
			printf("\n");
	}*/

	return true;
}

void Shader::bindShader()
{
	if(linked && programHandle > 0)
		glUseProgram(programHandle);
}

void Shader::setUniform(const char * name, const vec2 & v)
{
	GLint location = glGetUniformLocation(programHandle, name);
	glUniform2f(location, v.x, v.y);
}

void Shader::setUniform(const char * name, const vec3 & v)
{
	GLint location = glGetUniformLocation(programHandle, name);
	glUniform3f(location, v.x, v.y, v.z);
}

void Shader::setUniform(const char * name, const vec4 & v)
{
	GLint location = glGetUniformLocation(programHandle, name);
	glUniform4f(location, v.x, v.y, v.z, v.w);
}

void Shader::setUniform(const char * name, const mat4 & m)
{
	GLint location = glGetUniformLocation(programHandle, name);
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(m));
}

void Shader::setUniform(const char * name, const mat3 & m)
{
	GLint location = glGetUniformLocation(programHandle, name);
	glUniformMatrix3fv(location, 1, false, glm::value_ptr(m));
}

void Shader::setUniform(const char * name, float val)
{
	GLint location = glGetUniformLocation(programHandle, name);
	glUniform1f(location, val);
}

void Shader::setUniform(const char * name, int val)
{
	GLint location = glGetUniformLocation(programHandle, name);
	glUniform1i(location, val);
}

void Shader::setUniform(const char * name, bool val)
{
	GLint location = glGetUniformLocation(programHandle, name);
	glUniform1i(location, val);
}

void Shader::setUniform(const char * name, GLuint val)
{
	GLint location = glGetUniformLocation(programHandle, name);
	glUniform1ui(location, val);
}

void Shader::setUniform(const char * name, Light val)
{
	string locationName = name;
	locationName += ".pos";
	setUniform(locationName.c_str(), val.pos);
	locationName = name;
	locationName += ".ambient";
	setUniform(locationName.c_str(), val.ambient);
	locationName = name;
	locationName += ".diffuse";
	setUniform(locationName.c_str(), val.diffuse);
	locationName = name;
	locationName += ".specular";
	setUniform(locationName.c_str(), val.specular);
}

void Shader::setUniform(const char *name, vector<Light> vals)
{
	//Maybe be really slow to do for all objects on each frame, but cleaner than uniform block
	for (int i = 0; i < vals.size(); ++i)
	{
		Light light = vals.at(i);
		std::ostringstream locationName;
		locationName << name << "[" << i << "]";
		setUniform(locationName.str().c_str(), light);
	}
}

void Shader::setUniform(const char * name, Material val)
{
	string locationName = name;
	locationName += ".d";
	setUniform(locationName.c_str(), val.d);
	locationName = name;
	locationName += ".Ka";
	setUniform(locationName.c_str(), val.Ka);
	locationName = name;
	locationName += ".Kd";
	setUniform(locationName.c_str(), val.Kd);
	locationName = name;
	locationName += ".Ks";
	setUniform(locationName.c_str(), val.Ks);
	locationName = name;
	locationName += ".Ns";
	setUniform(locationName.c_str(), val.Ns);
}

void Shader::bindUniformBlock(const char * blockName, unsigned int & bindingPoint)
{
	unsigned int blockIndex = glGetUniformBlockIndex(programHandle, blockName);
	glUniformBlockBinding(programHandle, blockIndex, bindingPoint);
}


void Shader::bindAttribLocation(GLuint location, const char* name)
{
	glBindAttribLocation(programHandle, location, name);
}

void Shader::bindFragDataLocation(GLuint location, const char* name)
{
	glBindFragDataLocation(programHandle, location, name);
}

void Shader::setUniformIndex(unsigned int Index, const Matrix4f& matIn)
{
	// Pass transformation matrix to uniform array. 
	glUniformMatrix4fv(m_boneLocation[Index], 1, GL_TRUE, (const GLfloat*)matIn.m);
}

void Shader::initialiseBoneUniforms()
{
	for (unsigned int i = 0; i < ui_BoneArraySize; i++) {

		char Name[128];
		memset(Name, 0, sizeof(Name));
		_snprintf_s(Name, sizeof(Name), "gBones[%d]", i);
		m_boneLocation[i] = glGetUniformLocation(programHandle, Name);
	}
}