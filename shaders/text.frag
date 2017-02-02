#version 430

in vec2 texCoord;

out vec4 FragColour;

uniform vec4 textColour;
uniform sampler2D tex;

void main() {
   vec4 sampled = vec4(1.0, 1.0, 1.0, texture(tex, texCoord).r);
   FragColour = textColour * sampled;
}