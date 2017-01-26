#ifndef GLSUPPORT_H
#define GLSUPPORT_H
#include "GL/glew.h"
#include <string>
using std::string;
/////////////////////////////////////////////////
/// TAKEN FROM SECOND YEAR - DO NOT CREDIT.
/////////////////////////////////////////////////
class OpenGLSupport
{
public:
	OpenGLSupport(){};
	string GetError()
	{
		// opengl doesn't report error conditions, you have
		// to ask for any errors - this will return
		// a string representing any poen gl errorsv found
		// if all is ok you get the empty string

		string s = "";
		GLenum err; 

		err = glGetError();


		if (err == GL_NO_ERROR)
		{
			return s;
		}


		// loop scanning for errors and appending them to a string
		while (err != GL_NO_ERROR)
		{
			switch (err)
			{
				case GL_NO_ERROR:
					//"No error has been recorded. The value of this symbolic constant is guaranteed to be 0.";
					break;

				case GL_INVALID_ENUM:
					s = s + "GL_INVALID_ENUM "; //"An unacceptable value is specified for an enumerated argument. The offending command is ignored and has no other side effect than to set the error flag.";
					break;

				case GL_INVALID_VALUE:
					s = s + "GL_INVALID_VALUE "; //"A numeric argument is out of range. The offending command is ignored and has no other side effect than to set the error flag.";
					break;

				case GL_INVALID_OPERATION:
					s = s + "GL_INVALID_OPERATION "; //"The specified operation is not allowed in the current state. The offending command is ignored and has no other side effect than to set the error flag.";
					break;

				//case GL_INVALID_FRAMEBUFFER_OPERATION:
				//    s = "The framebuffer object is not complete. The offending command is ignored and has no other side effect than to set the error flag.";
				//    break;

				case GL_OUT_OF_MEMORY:
					s = s + "GL_OUT_OF_MEMORY "; //"There is not enough memory left to execute the command. The state of the GL is undefined, except for the state of the error flags, after this error is recorded.";
					break;

				case GL_STACK_UNDERFLOW:
					s = s + "GL_STACK_UNDERFLOW";	//"An attempt has been made to perform an operation that would cause an internal stack to underflow.";
					break;

				case GL_STACK_OVERFLOW:
					s = s + "GL_STACK_OVERFLOW"; //"An attempt has been made to perform an operation that would cause an internal stack to overflow.";
					break;
			}
			// get next error, if any
			err = glGetError();
		}
		// return the error messages as a single string
		return s;

	}

};

#endif