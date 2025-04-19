#include "texture.h"
#include <SDL3_image/SDL_image.h>

Texture::Texture()
{
    textureHandle = 0;
}

void Texture::setFilename(string newTextureFilename)
{
    filename = SDL_GetBasePath() + newTextureFilename;
}

bool Texture::loadTexture()
{
    deleteTexture();

    if(filename.empty())
    {
        errorMessage = "Texture filename not set";
        return false;
    }

    SDL_Surface* surface = IMG_Load(filename.c_str());
    if(!surface)
    {
        errorMessage = "Unable to load image: ";
        errorMessage += filename;
        return false;
    }

    Uint32 rmask, gmask, bmask, amask;
    if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
    {
        rmask = 0xff000000;
        gmask = 0x00ff0000;
        bmask = 0x0000ff00;
        amask = 0x000000ff;
    }
    else if(SDL_BYTEORDER == SDL_LIL_ENDIAN)
    {
        rmask = 0x000000ff;
        gmask = 0x0000ff00;
        bmask = 0x00ff0000;
        amask = 0xff000000;
    }

    SDL_Surface* surfaceRGBA = SDL_CreateSurface(surface->w, surface->h, SDL_GetPixelFormatForMasks(32, rmask, gmask, bmask, amask));
    if(!surfaceRGBA)
    {
        errorMessage = "Unable to create RGBA surface";
        SDL_DestroySurface(surface);
        return false;
    }

    if(!SDL_BlitSurface(surface, NULL, surfaceRGBA, NULL))
    {
        errorMessage = "Failed to blit surface while loading";
        SDL_DestroySurface(surface);
        SDL_DestroySurface(surfaceRGBA);
        return false;
    }

    if(!SDL_FlipSurface(surfaceRGBA, SDL_FLIP_VERTICAL))
    {
        errorMessage = "Failed to flip surface";
        SDL_DestroySurface(surface);
        SDL_DestroySurface(surfaceRGBA);
        return false;
    }

    glGenTextures(1, &textureHandle);
    glBindTexture(GL_TEXTURE_2D, textureHandle);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, surfaceRGBA->w, surfaceRGBA->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surfaceRGBA->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    SDL_DestroySurface(surfaceRGBA);
    SDL_DestroySurface(surface);

    unbind();

    return true;
}

void Texture::deleteTexture()
{
    glDeleteTextures(1, &textureHandle);
    textureHandle = 0;
    errorMessage = "";
}

void Texture::setParameter(GLenum parameter, GLint value)
{
    bind();
    glTexParameteri(GL_TEXTURE_2D, parameter, value);
    unbind();
}

void Texture::bind()
{
    glBindTexture(GL_TEXTURE_2D, textureHandle);
}

void Texture::unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint Texture::getHandle()
{
    return textureHandle;
}

string Texture::getFilename()
{
    return filename;
}

string Texture::getError()
{
    return errorMessage;
}
