#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

int windowWidth = 1024;
int windowHeight = 600;

float moveSpeed = 10.0;
float mouseSensitivity = 0.3;

SDL_Window* window = NULL;
SDL_GLContext context = NULL;

bool programRunning = true;
bool isFullscreen = false;
bool useWireframe = false;
Uint64 previousTimestamp = 0;

float x = 0;
float y = 0;
float z = 0;
float pitch = 0;
float yaw = 0;

Shader mainShader;

GLuint vao;
GLuint vbo[3];

int init()
{
    if(SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("Unable to initialise SDL: %s\n", SDL_GetError());
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_SetRelativeMouseMode(SDL_TRUE);

    SDL_PropertiesID properties = SDL_CreateProperties();
    SDL_SetStringProperty(properties, SDL_PROP_WINDOW_CREATE_TITLE_STRING, "Lesson 08 - Movement");
    SDL_SetNumberProperty(properties, SDL_PROP_WINDOW_CREATE_X_NUMBER, SDL_WINDOWPOS_UNDEFINED);
    SDL_SetNumberProperty(properties, SDL_PROP_WINDOW_CREATE_Y_NUMBER, SDL_WINDOWPOS_UNDEFINED);
    SDL_SetNumberProperty(properties, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, windowWidth);
    SDL_SetNumberProperty(properties, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, windowHeight);
    SDL_SetNumberProperty(properties, "flags", SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    window = SDL_CreateWindowWithProperties(properties);
    SDL_DestroyProperties(properties);
    if(window == NULL)
    {
        printf("Unable to create a window: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Surface* icon = IMG_Load("resources/icon.png");
    if(icon == NULL)
    {
        printf("Unable to load icon: %s\n", SDL_GetError());
        return -1;
    }
    if(SDL_SetWindowIcon(window, icon) != 0)
    {
        SDL_DestroySurface(icon);
        printf("Unable to set window icon: %s\n", SDL_GetError());
        return -1;
    }
    SDL_DestroySurface(icon);

    context = SDL_GL_CreateContext(window);
    if(context == NULL)
    {
        printf("Unable to create an OpenGL context: %s\n", SDL_GetError());
        return -1;
    }

    GLenum glewError = glewInit();
    if(glewError != GLEW_OK)
    {
        printf("Unable to initialise GLEW: %s\n", glewGetErrorString(glewError));
        return -1;
    }

    int majorVersion, minorVersion;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &majorVersion);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minorVersion);
    if(majorVersion < 4 || (majorVersion == 4 && minorVersion < 6))
    {
        printf("Unable to get a recent OpenGL version!\n");
        return -1;
    }
    printf("%s\n", glGetString(GL_VERSION));

    mainShader.setSourceFiles("shaders/main_vertex.glsl", "shaders/main_fragment.glsl");
    if(mainShader.loadShader() != 0)
    {
        printf("Unable to create shader from files: %s\n", mainShader.getFilenames().c_str());
        printf("Error message: %s\n", mainShader.getError().c_str());
        return -1;
    }

    GLfloat vertices[] = 
    {
        0.0f, 0.5f, -0.5f,
        0.0f, -0.5f, -0.5f,
        0.0f, -0.5f, 0.5f,
        0.0f, 0.5f, 0.5f
    };

    GLfloat colours[] = 
    {
        1.0, 0.48, 0.02,
        1.0, 0.48, 0.02,
        1.0, 0.97, 0.02,
        1.0, 0.02, 0.05,
    };

    GLuint indices[] = {0, 1, 2, 0, 2, 3};

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(3, vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); 

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colours), colours, GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);

    glClearColor(0.04f, 0.23f, 0.51f, 1.0f);

    glEnable(GL_DEPTH_TEST);

    previousTimestamp = SDL_GetTicks();

    return 0;
}

void close()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(3, vbo);
    mainShader.deleteShader();

    SDL_GL_DeleteContext(context);
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
            printf("Key pressed down: %s\n", SDL_GetKeyName(event.key.keysym.sym));
            if(event.key.keysym.sym == SDLK_ESCAPE)
            {
                programRunning = false;
                return;
            }
            else if(event.key.keysym.sym == SDLK_f)
            {
                isFullscreen = !isFullscreen;
                SDL_SetWindowFullscreen(window, isFullscreen);
            }
            else if(event.key.keysym.sym == SDLK_r)
            {
                if(mainShader.loadShader() != 0)
                {
                    printf("Unable to create shader from files: %s\n", mainShader.getFilenames().c_str());
                    printf("Error message: %s\n", mainShader.getError().c_str());
                    programRunning = false;
                }
            }
            else if(event.key.keysym.sym == SDLK_t)
            {
                useWireframe = !useWireframe;
                if(useWireframe)
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                }
                else
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }
            }
        }
        else if(event.type == SDL_EVENT_MOUSE_MOTION)
        {
            yaw -= event.motion.xrel * mouseSensitivity;
            pitch += event.motion.yrel * mouseSensitivity;

            if(yaw > 360)
                yaw -= 360;
            else if(yaw < 0)
                yaw += 360;

            if(pitch > 30)
                pitch = 30;
            else if(pitch < -30)
                pitch = -30;
        }
    }
}

void update()
{
    Uint64 currentTimestamp = SDL_GetTicks();
    Uint64 timeDelta = currentTimestamp - previousTimestamp;
    previousTimestamp = SDL_GetTicks();

    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);

    if(keyboardState[SDL_SCANCODE_W] || keyboardState[SDL_SCANCODE_UP])
    {
        x += cos(yaw*3.1415/180) * moveSpeed * timeDelta / 1000;
        y += sin(yaw*3.1415/180) * moveSpeed * timeDelta / 1000;
    }
    else if(keyboardState[SDL_SCANCODE_S] || keyboardState[SDL_SCANCODE_DOWN])
    {
        x -= cos(yaw*3.1415/180) * moveSpeed * timeDelta / 1000;
        y -= sin(yaw*3.1415/180) * moveSpeed * timeDelta / 1000;
    }
    else if(keyboardState[SDL_SCANCODE_A] || keyboardState[SDL_SCANCODE_LEFT])
    {
        x -= sin(yaw*3.1415/180) * moveSpeed * timeDelta / 1000;
        y += cos(yaw*3.1415/180) * moveSpeed * timeDelta / 1000;
    }
    else if(keyboardState[SDL_SCANCODE_D] || keyboardState[SDL_SCANCODE_RIGHT])
    {
        x += sin(yaw*3.1415/180) * moveSpeed * timeDelta / 1000;
        y -= cos(yaw*3.1415/180) * moveSpeed * timeDelta / 1000;
    }
    else if(keyboardState[SDL_SCANCODE_LSHIFT])
    {
        z += moveSpeed * timeDelta / 1000;
    }
    else if(keyboardState[SDL_SCANCODE_LCTRL])
    {
        z -= moveSpeed * timeDelta / 1000;
    }
}

void draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 pMatrix = glm::perspective(1.0f, (float) windowWidth / windowHeight, 0.1f, 100.f);

    float targetX = x + (cos(yaw*3.1415/180) * cos(pitch*3.1415/180));
    float targetY = y + (sin(yaw*3.1415/180) * cos(pitch*3.1415/180));
    float targetZ = z - sin(pitch*3.1415/180);
    glm::vec3 target = glm::vec3(targetX, targetY, targetZ);
    glm::mat4 vMatrix = glm::lookAt(glm::vec3(x, y, z), target, glm::vec3(0, 0, 1));

    mainShader.bind();

    glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(pMatrix));
    glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(vMatrix));

    glBindVertexArray(vao);

    glm::mat4 mMatrix = glm::mat4(1.0f);
    mMatrix = glm::translate(mMatrix, glm::vec3(5.0, 0.0, 0.0));
    glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(mMatrix));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    mainShader.unbind();

    SDL_GL_SwapWindow(window);
}

int main(int argc, char* argv[])
{
    if(init() != 0)
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
