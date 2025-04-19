#version 460

in vec3 colour;

out vec4 fragment;

void main()
{
    fragment = vec4(colour, 1.0);
}
