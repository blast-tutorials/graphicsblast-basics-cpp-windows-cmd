#pragma once
#include <string>
#include <GL/glew.h>

using namespace std;

class Shader
{
    public:
        Shader();

        void setFilenames(string vertexFilename, string fragmentFilename);
        int loadShader();
        void deleteShader();

        void bind();
        void unbind();

        GLuint getHandle();
        string getFilenames();
        string getError();

    private:
        string vFilename, fFilename;
        GLuint shaderProgram;
        string errorMessage;

        string readFile(string filename);
};
