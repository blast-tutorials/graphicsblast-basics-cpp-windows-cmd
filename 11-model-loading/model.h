#pragma once

#include <GL/glew.h>
#include <string>

using namespace std;

class Model
{
    public:
        Model();

        void setFilename(string newModelFilename);
        bool loadOBJModel();
        void deleteModel();

        void bind();
        void unbind();
        
        string getFilename();
        string getError();
        int getIndexCount();

    private:
        string filename;
        string errorMessage;
        int indexCount;

        GLuint vao;
        GLuint vbo[4];
};
