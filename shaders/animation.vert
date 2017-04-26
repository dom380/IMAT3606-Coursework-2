#version 430

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexUVs;
layout ( location = 3) in ivec4 BoneIDs; // Stream of vertex bone IDs
layout ( location = 4) in vec4 Weights; // Stream of vertex weights

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;
uniform mat3 normal;

const int MAX_BONES = 70; // Max number of bones
uniform mat4 gBones[MAX_BONES]; // Bone transformations  ???

uniform bool animatedCharacter;

out Data
{
	vec3 Normal;
	vec3 FragPos;
	vec2 TexCoord;
} outData;



void main()
{
	if (!animatedCharacter)
	{
		outData.FragPos = vec3(model * vec4(VertexPosition,1.0));
		outData.Normal = normalize(normal * VertexNormal);
	    gl_Position = projection * view * model * vec4(VertexPosition,1.0);
	}
	else
	{
		// Multiply each bone transformation by the particular weight and combine
   		mat4 BoneTransform = gBones[BoneIDs[0]] * Weights[0];
		BoneTransform += gBones[BoneIDs[1]] * Weights[1];
		BoneTransform += gBones[BoneIDs[2]] * Weights[2];
		BoneTransform += gBones[BoneIDs[3]] * Weights[3];

		// Transformed vertex position 
		vec4 tPos = BoneTransform * vec4(VertexPosition, 1.0);
		gl_Position = projection * view * model * tPos;

		// Transformed normal 
		vec4 tNormal = BoneTransform * vec4(VertexNormal, 0.0);
		outData.Normal = normalize(mat4(normal) * tNormal).xyz;

		vec4 worldPos = model * tPos;
		outData.FragPos = worldPos.xyz;   
		outData.TexCoord = VertexUVs;  
	}	
}