#version 430
in vec3 ourColor;
in vec2 TexCoord;

out vec4 color;

uniform sampler2D tex;

void main()
{
	vec4 texel = texture(tex, TexCoord);
	if(texel.a < 0.3) //Discard any low alpha value fragments as a quick fix for transparency with depth testing 
		discard;
    color = texel;
}