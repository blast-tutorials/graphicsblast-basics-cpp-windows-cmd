#include "shader.h"
#include <fstream>
#include <sstream>

Shader::Shader()
{
    shaderProgram = 0;
}

void Shader::setFilenames(string newVertexFilename, string newFragmentFilename)
{
    vertexFilename = SDL_GetBasePath() + newVertexFilename;
    fragmentFilename = SDL_GetBasePath() + newFragmentFilename;
}

string Shader::readFile(string filename)
{
    ifstream file(filename);
    if(!file.is_open())
    {
        errorMessage += "Cannot open file: ";
        errorMessage += filename;
        return "";
    }

    stringstream buffer;
    buffer << file.rdbuf();
    string contents = buffer.str();
    file.close();

    if(contents.empty())
    {
        errorMessage += "Shader source file is empty: ";
        errorMessage += filename;
    }

    return contents;
}

GLuint Shader::createShader(string filename, GLenum shaderType)
{
    string shaderSource = readFile(filename);
    if(shaderSource.empty())
    {
        return 0;
    }

    GLuint shader = glCreateShader(shaderType);
    if(shader == 0)
    {
        errorMessage = "Unable to create shader object";
        return 0;
    }

    const char* shaderText = shaderSource.c_str();
    glShaderSource(shader, 1, &shaderText, NULL);
    glCompileShader(shader);
    
    GLint compileStatus;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
    if(compileStatus == GL_FALSE)
    {
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH , &logLength);

        GLchar* compilerLog = new GLchar[logLength];
        glGetShaderInfoLog(shader, logLength, NULL, compilerLog);

        errorMessage = "Error compiling shader: ";
        errorMessage += compilerLog;
        delete[] compilerLog;

        glDeleteShader(shader);
        
        return 0;
    }

    return shader;
}

bool Shader::loadShader()
{
    errorMessage = "";
    deleteShader();

    if(vertexFilename.empty() || fragmentFilename.empty())
    {
        errorMessage = "Shader source filenames not set";
        return false;
    }

    GLuint vertexShader = createShader(vertexFilename, GL_VERTEX_SHADER);
    if(vertexShader == 0)
    {
        return false;
    }

    GLuint fragmentShader = createShader(fragmentFilename, GL_FRAGMENT_SHADER);
    if(fragmentShader == 0)
    {
        glDeleteShader(vertexShader);
        return false;
    }

    shaderProgram = glCreateProgram();
    if(shaderProgram == 0)
    {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        errorMessage = "Unable to create shader program";
        return false;
    }

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);
    
    glDetachShader(shaderProgram, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(shaderProgram, fragmentShader);
    glDeleteShader(fragmentShader);

    GLint linkStatus;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);
    if(linkStatus == GL_FALSE)
    {
        GLint logLength;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH , &logLength);

        GLchar* compilerLog = new GLchar[logLength];
        glGetProgramInfoLog(shaderProgram, logLength, NULL, compilerLog);

        errorMessage = "Error linking shader program: ";
        errorMessage += compilerLog;
        delete[] compilerLog;

        deleteShader();

        return false;
    }

    return true;
}

void Shader::deleteShader()
{
    unbind();
    glDeleteProgram(shaderProgram);
    shaderProgram = 0;
}

void Shader::bind()
{
    glUseProgram(shaderProgram);
}

void Shader::unbind()
{
    glUseProgram(0);
}

string Shader::getFilenames()
{
    return vertexFilename + " " + fragmentFilename;
}

string Shader::getError()
{
    return errorMessage;
}

GLuint Shader::getHandle()
{
    return shaderProgram;
}
