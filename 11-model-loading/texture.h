#pragma once

#include <GL/glew.h>
#include <string>

using namespace std;

class Texture
{
    public:
        Texture();

        void setFilename(string newTextureFilename);
        bool loadTexture();
        void deleteTexture();

        void setParameter(GLenum parameter, int value);

        void bind();
        void unbind();
        
        GLuint getHandle();
        string getFilename();
        string getError();

    private:
        string filename;
        GLuint textureHandle;
        string errorMessage;
};
