#version 430
in vec3 ourColor;
in vec2 TexCoord;

out vec4 color;

uniform sampler2D tex;

void main()
{
    color = texture(tex, TexCoord);
}