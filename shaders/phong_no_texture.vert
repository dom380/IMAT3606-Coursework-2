#version 430

layout (location=0) in vec4 vertPosition;
layout (location=2) in vec3 normal;

//FIXES INTEL/NVIDIA SHADER ERROR
out vec2 texCoord;
out vec3 Normal;
out vec3 fragmentPos;

uniform mat4 mModel;

uniform mat4 mView;

uniform mat4 mProjection;


void main()
{
	fragmentPos = vec3(mModel * vertPosition);
    Normal = mat3(transpose(inverse(mModel))) * normal;  
	gl_Position = mProjection * mView * mModel * vertPosition;
}
