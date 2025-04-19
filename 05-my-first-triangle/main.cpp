#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <GL/glew.h>

#include <string>
#include <stdio.h>

int windowWidth = 1024;
int windowHeight = 600;

SDL_Window* window = NULL;
SDL_GLContext context = NULL;

bool programRunning = true;
bool isFullscreen = false;

GLuint shaderProgram;

GLuint vao;
GLuint vbo;

bool init()
{
    if(!SDL_Init(SDL_INIT_VIDEO))
    {
        printf("Unable to initialise SDL: %s\n", SDL_GetError());
        return false;
    }

    SDL_PropertiesID properties = SDL_CreateProperties();

    SDL_SetStringProperty(properties, SDL_PROP_WINDOW_CREATE_TITLE_STRING, "Lesson 05 - My First Triangle!");
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

    std::string iconPath(SDL_GetBasePath());
    iconPath.append("resources/icon.png");
    SDL_Surface* icon = IMG_Load(iconPath.c_str());
    if(!icon)
    {
        printf("Unable to load icon: %s\n", SDL_GetError());
        return false;
    }
    if(!SDL_SetWindowIcon(window, icon))
    {
        SDL_DestroySurface(icon);
        printf("Unable to set window icon: %s\n", SDL_GetError());
        return false;
    }
    SDL_DestroySurface(icon);

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

    const char* vertexShaderSource =
        "#version 460\n"
        "layout(location = 0) in vec3 aPosition;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = vec4(aPosition, 1.0f);\n"
        "}";

    const char* fragmentShaderSource =
        "#version 460\n"
        "out vec4 fragment;\n"
        "void main()\n"
        "{\n"
        "   fragment = vec4(1.0, 0.48, 0.02, 1.0);\n"
        "}\n";

    shaderProgram = glCreateProgram();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    glAttachShader(shaderProgram, vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glAttachShader(shaderProgram, fragmentShader);
    
    glLinkProgram(shaderProgram);

    glDetachShader(shaderProgram, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(shaderProgram, fragmentShader);
    glDeleteShader(fragmentShader);

    GLfloat vertices[] = 
    {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
    };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindVertexArray(0);

    glClearColor(0.04f, 0.23f, 0.51f, 1.0f);

    glEnable(GL_DEPTH_TEST);

    return true;
}

void close()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shaderProgram);

    SDL_GL_DestroyContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void handleEvents()
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_EVENT_QUIT)
        {
            programRunning = false;
            return;
        }
        else if(event.type == SDL_EVENT_WINDOW_RESIZED)
        {
            windowWidth = event.window.data1;
            windowHeight = event.window.data2;
            glViewport(0, 0, windowWidth, windowHeight);
        }
        else if(event.type == SDL_EVENT_KEY_DOWN)
        {
            printf("Key pressed down: %s\n", SDL_GetKeyName(event.key.key));
            if(event.key.key == SDLK_ESCAPE)
            {
                programRunning = false;
                return;
            }
            else if(event.key.key == SDLK_F)
            {
                isFullscreen = !isFullscreen;
                SDL_SetWindowFullscreen(window, isFullscreen);
            }
        }
    }
}

void update()
{

}

void draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
    glUseProgram(0);

    SDL_GL_SwapWindow(window);
}

int main(int argc, char* argv[])
{
    if(!init())
    {
        close();
        return -1;
    }

    while(programRunning)
    {
        update();
        draw();
        handleEvents();
    }

    close();

    return 0;
}
