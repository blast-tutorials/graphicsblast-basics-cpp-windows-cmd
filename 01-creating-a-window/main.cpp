#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <GL/glew.h>

#include <stdio.h>

int windowWidth = 1024;
int windowHeight = 600;

SDL_Window* window = NULL;
SDL_GLContext context = NULL;

bool init()
{
    if(!SDL_Init(SDL_INIT_VIDEO))
    {
        printf("Unable to initialise SDL: %s\n", SDL_GetError());
        return false;
    }

    SDL_PropertiesID properties = SDL_CreateProperties();

    SDL_SetStringProperty(properties, SDL_PROP_WINDOW_CREATE_TITLE_STRING, "Lesson 01 - Creating a Window");
    SDL_SetNumberProperty(properties, SDL_PROP_WINDOW_CREATE_X_NUMBER, SDL_WINDOWPOS_UNDEFINED);
    SDL_SetNumberProperty(properties, SDL_PROP_WINDOW_CREATE_Y_NUMBER, SDL_WINDOWPOS_UNDEFINED);
    SDL_SetNumberProperty(properties, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, windowWidth);
    SDL_SetNumberProperty(properties, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, windowHeight);
    SDL_SetBooleanProperty(properties, SDL_PROP_WINDOW_CREATE_OPENGL_BOOLEAN, true);
    SDL_SetBooleanProperty(properties, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, true);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    window = SDL_CreateWindowWithProperties(properties);
    SDL_DestroyProperties(properties);
    if(!window)
    {
        printf("Unable to create a window: %s\n", SDL_GetError());
        return false;
    }

    context = SDL_GL_CreateContext(window);
    if(!context)
    {
        printf("Unable to create an OpenGL context: %s\n", SDL_GetError());
        return false;
    }

    GLenum glewError = glewInit();
    if(glewError != GLEW_OK)
    {
        printf("Unable to initialise GLEW: %s\n", glewGetErrorString(glewError));
        return false;
    }

    int majorVersion, minorVersion;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &majorVersion);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minorVersion);
    if(majorVersion < 4 || (majorVersion == 4 && minorVersion < 6))
    {
        printf("Unable to get a recent OpenGL version!\n");
        return false;
    }
    printf("%s\n", glGetString(GL_VERSION));

    glClearColor(0.04f, 0.23f, 0.51f, 1.0f);

    return true;
}

void close()
{
    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    SDL_GL_SwapWindow(window);
}

int main(int argc, char* argv[])
{
    if(!init())
    {
        close();
        return -1;
    }

    draw();
    SDL_Delay(2000);

    close();

    return 0;
}
