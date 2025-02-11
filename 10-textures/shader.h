#pragma once

#include <SDL3/SDL.h>
#include <GL/glew.h>
#include <string>

using namespace std;

class Shader
{
    public:
        Shader();

        void setFilenames(string newVertexFilename, string newFragmentFilename);
        bool loadShader();
        void deleteShader();

        void bind();
        void unbind();

        string getFilenames();
        string getError();
        GLuint getHandle();

    private:
        string vertexFilename, fragmentFilename;
        string errorMessage;
        GLuint shaderProgram;

        GLuint createShader(string filename, GLenum shaderType);
        string readFile(string filename);
};
