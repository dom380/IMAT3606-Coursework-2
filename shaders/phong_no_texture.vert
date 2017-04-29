#version 430

layout (location=0) in vec4 vertPosition;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 fragTexCoord;
layout (location=3) in vec3 Ka;
layout (location=4) in vec3 Kd;
layout (location=5) in float Shininess;


out Data
{
//FIXES INTEL/NVIDIA SHADER ERROR
	vec2 texCoord;
	vec3 Normal;
	vec3 fragmentPos;
	vec3 Ka;
	vec3 Kd;
	float Shininess;
} outData;

uniform mat4 mModel;

uniform mat4 mView;

uniform mat4 mProjection;


void main()
{
	outData.fragmentPos = vec3(mModel * vertPosition);
    outData.Normal = mat3(transpose(inverse(mModel))) * normal;  
	outData.Ka = Ka;
	outData.Kd = Kd;
	outData.Shininess = Shininess;
	outData.texCoord = fragTexCoord;
	gl_Position = mProjection * mView * mModel * vertPosition;
}
