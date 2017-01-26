#version 430

layout (location=0) in vec4 vertPosition;

uniform mat4 mModel;

uniform mat4 mView;

uniform mat4 mProjection;

layout (location=1) in vec3  vertColour;
out vec4 colour;

void main()
{
	colour = vec4(vertColour, 1.0);
	gl_Position = mProjection * mView * mModel * vertPosition;
}
