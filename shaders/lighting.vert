#version 430

layout (location=0) in vec4 VertexPosition;
layout (location=1) in vec3 VertexNormal;
layout (location=2) in vec2 VertexUV;
layout (location=3) in vec3 Ka;
layout (location=4) in vec3 Kd;
layout (location=5) in float Shininess;
layout (location=6) in ivec4 BoneIDs; // Stream of vertex bone IDs
layout (location=7) in vec4 Weights; // Stream of vertex weights

out Data
{
	vec3 Normal;
	vec3 FragPos;
	vec2 texCoord;
	vec3 Ka;
	vec3 Kd;
	float Shininess;
} outData;


uniform mat3 normal;
uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

const int MAX_BONES = 70; // Max number of bones
uniform mat4 gBones[MAX_BONES]; // Bone transformations  ???

uniform bool animatedCharacter;

void main()
{
	outData.texCoord = VertexUV; 
	outData.Ka = Ka;
	outData.Kd = Kd;
	outData.Shininess = Shininess;

	if (!animatedCharacter)
	{
		outData.FragPos = vec3(model * VertexPosition);
		outData.Normal = normalize(normal * VertexNormal);
	    gl_Position = projection * view * model * VertexPosition;
	}
	else
	{
		// Multiply each bone transformation by the particular weight
		// and combine them. 
   		mat4 BoneTransform = gBones[ BoneIDs[0] ] * Weights[0];
		BoneTransform += gBones[ BoneIDs[1] ] * Weights[1];
		BoneTransform += gBones[ BoneIDs[2] ] * Weights[2];
		BoneTransform += gBones[ BoneIDs[3] ] * Weights[3];

		// Transformed vertex position 
		vec4 tPos = BoneTransform * VertexPosition;
		gl_Position = projection * view * model * tPos;

		// Transformed normal 
		vec4 tNormal = BoneTransform * vec4(VertexNormal, 0.0);
		outData.Normal = normalize(mat4(normal) * tNormal).xyz;

		outData.FragPos = vec4(model * tPos).xyz;  
	}	 
}