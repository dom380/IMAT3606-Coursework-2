#version 430

in vec4 colour;
layout (location=0) out vec4 FragColour;

void main() {
    FragColour = colour;
}

