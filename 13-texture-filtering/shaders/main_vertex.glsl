#version 460

layout(location = 0) uniform mat4 uPMatrix;
layout(location = 1) uniform mat4 uVMatrix;
layout(location = 2) uniform mat4 uMMatrix;

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColour;
layout(location = 2) in vec2 aTextureCoordinate;

out vec3 colour;
out vec2 textureCoordinate;

void main()
{
    colour = aColour;
    textureCoordinate = aTextureCoordinate;
    gl_Position = uPMatrix * uVMatrix * uMMatrix * vec4(aPosition, 1.0);
}
