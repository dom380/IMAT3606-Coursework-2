#pragma once

class VertexArrayObject
{
public:
	

private:
	VertexArrayObject()
	{

	}


};

#include <vector>
#include <gl\glew.h>

namespace GraphicsUtils
{
	GLuint createVertexArrayObject(std::vector<GLuint>& vboHandles)
	{
		GLuint vaoHandle;
		//Create Vertex Array Object
		glGenVertexArrays(1, &vaoHandle);
		glBindVertexArray(vaoHandle);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		//Bind VBOs to VAO
		glBindBuffer(GL_ARRAY_BUFFER, vboHandles[0]); //position
		glVertexAttribPointer(0, 4, GL_FLOAT, false, 0, (GLubyte *)NULL);
		glBindBuffer(GL_ARRAY_BUFFER, vboHandles[1]); //uvs
		glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, (GLubyte *)NULL);
		glBindBuffer(GL_ARRAY_BUFFER, vboHandles[2]); //normals
		glVertexAttribPointer(2, 3, GL_FLOAT, false, 0, (GLubyte *)NULL);
		
		//Bind IBO to VAO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboHandles[3]);

		return vaoHandle;
	}
}