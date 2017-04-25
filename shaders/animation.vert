#version 430

layout (location=0) in vec3 VertexPosition;
layout (location=1) in vec3 VertexNormal;

layout (location=2) in ivec4 BoneIDs; // Stream of vertex bone IDs
layout (location=3) in vec4 Weights; // Stream of vertex weights

layout(location = 4) in vec2 uvs;
layout(location = 5) in vec3 colour;

out vec3 Normal;
out vec3 Position;
out vec2 texCoords;
out vec3 outCol;
out vec4 ShadowCoord;
out vec3 LightPos;

uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;
//uniform mat4 ShadowMatrix;

const int MAX_BONES = 70; // Max number of bones
uniform mat4 gBones[MAX_BONES]; // Bone transformations  ???

uniform bool animatedCharacter;

void main()
{
	if (!animatedCharacter)
	{
		Position = (ModelViewMatrix * vec4(VertexPosition,1.0)).xyz;
		Normal = normalize( NormalMatrix * VertexNormal );
	    gl_Position = MVP * vec4(VertexPosition,1.0);
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
		vec4 tPos = BoneTransform * vec4(VertexPosition, 1.0);
		gl_Position = MVP * tPos;

		// Transformed normal 
		vec4 tNormal = BoneTransform * vec4(VertexNormal, 0.0);
		Normal = normalize(mat4(NormalMatrix) * tNormal).xyz;

		vec4 worldPos = ModelMatrix * tPos;
		Position = worldPos.xyz;   
		texCoords = uvs;  
		outCol = colour;
	}	

	LightPos = vec3(ViewMatrix * vec4(0,200,100,1));
	//ShadowCoord = ShadowMatrix * vec4(VertexPosition,1.0);
}