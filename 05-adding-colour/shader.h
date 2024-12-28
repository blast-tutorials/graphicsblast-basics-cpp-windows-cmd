#pragma once

#include <GL/glew.h>
#include <string>

using namespace std;

class Shader
{
    public:
        Shader();

        void setFilenames(string vertexFilename, string fragmentFilename);
        bool loadShader();
        void deleteShader();

        void bind();
        void unbind();

        string getFilenames();
        string getError();
        GLuint getHandle();

    private:
        string vFilename, fFilename;
        string errorMessage;
        GLuint shaderProgram;

        string readFile(string filename);
};
