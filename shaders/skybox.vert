#version 430 core 

layout (location = 0) in vec3 VertexPosition; 
out vec3 TexCoord; 

uniform mat4 projection; 
uniform mat4 view; 
uniform mat4 model;

void main()
{
    vec4 pos = projection * view * model * vec4(VertexPosition, 1.0);
    gl_Position = pos.xyww;
    TexCoord = VertexPosition.xyz;
}  