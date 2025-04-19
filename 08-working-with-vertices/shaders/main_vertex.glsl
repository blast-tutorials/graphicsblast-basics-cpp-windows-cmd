#version 460

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColour;

out vec3 colour;

void main()
{
    colour = aColour;
    gl_Position = vec4(aPosition, 1.0);
}
