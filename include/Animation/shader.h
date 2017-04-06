#pragma once
#pragma warning( disable : 4290 )

#include <gl\glew.h>
#include <gl\glm\glm\glm.hpp>
#include <string>
#include <map>
#include <stdexcept>
#include "Math3D.h"

class ShaderException : public std::runtime_error 
{
  public:
	  ShaderException( const std::string & msg ) :std::runtime_error(msg)
	{ 

	}
};

namespace GLSLShader 
{
  enum GLSLShaderType 
  {
    VERTEX = GL_VERTEX_SHADER, 
    FRAGMENT = GL_FRAGMENT_SHADER, 
    GEOMETRY = GL_GEOMETRY_SHADER,
    TESS_CONTROL = GL_TESS_CONTROL_SHADER,
    TESS_EVALUATION = GL_TESS_EVALUATION_SHADER,
    COMPUTE = GL_COMPUTE_SHADER
  };
};

class Shader
{
  private:
    int  handle;
    bool linked;
    std::map<std::string, int> uniformLocations;

	static const unsigned int ui_BoneArraySize = 70;
	GLint m_boneLocation[ui_BoneArraySize]; //!< Bone uniform locations 

	GLint getUniformLocation(const char* name)
	{
		std::map<std::string, int>::iterator pos;
		pos = uniformLocations.find(name);

		if (pos == uniformLocations.end())
		{
			uniformLocations[name] = glGetUniformLocation(handle, name);
		}

		return uniformLocations[name];
	}

	bool fileExists(const std::string& fileName)
	{
		struct stat info;
		int ret = -1;

		ret = stat(fileName.c_str(), &info);
		return 0 == ret;
	}

	std::string getExtension(const char* name)
	{
		std::string nameStr(name);

		size_t loc = nameStr.find_last_of('.');

		if (loc != std::string::npos)
		{
			return nameStr.substr(loc, std::string::npos);
		}

		return "";
	}

    // Make these private in order to make the object non-copyable
	Shader(const Shader& other) 
	{ 

	}

	Shader& operator=(const Shader& other) 
	{ 
		return *this; 
	}

  public:
    Shader();
    ~Shader();

    void compileShader(const char* fileName) throw (ShaderException);
    void compileShader(const char* fileName, GLSLShader::GLSLShaderType type) throw (ShaderException);
    void compileShader(const std::string& source, GLSLShader::GLSLShaderType type, const char* fileName = NULL) throw (ShaderException);

    void link() throw (ShaderException);
    void validate() throw(ShaderException);
    void use() throw (ShaderException);

    int  getHandle();
    bool isLinked();

    void bindAttribLocation(GLuint location, const char* name);
    void bindFragDataLocation(GLuint location, const char* name);

    void setUniform(const char* name, float x, float y, float z);
    void setUniform(const char* name, const glm::vec2& v);
    void setUniform(const char* name, const glm::vec3& v);
    void setUniform(const char* name, const glm::vec4& v);
    void setUniform(const char* name, const glm::mat4& m);
    void setUniform(const char* name, const glm::mat3& m);
    void setUniform(const char* name, float val);
    void setUniform(const char* name, int val);
    void setUniform(const char* name, bool val);
    void setUniform(const char* name, GLuint val);
	void setUniformIndex(unsigned int Index, const Matrix4f& matIn);

	void initialiseBoneUniforms();

    void printActiveUniforms();
    void printActiveUniformBlocks();
    void printActiveAttribs();

    const char* getTypeString(GLenum type);
};
