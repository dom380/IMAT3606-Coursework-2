#version 430

layout (location=0) in vec4 vertPosition;
layout (location=1) in vec2 fragTexCoord;
layout (location=2) in vec3 normal;

out vec2 texCoord;

uniform mat4 mModel;

uniform mat4 mView;

uniform mat4 mProjection;


void main()
{
	texCoord = fragTexCoord;
	gl_Position = mProjection * mView * mModel * vertPosition;
}
