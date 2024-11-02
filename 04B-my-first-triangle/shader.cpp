#include "shader.h"
#include <fstream>
#include <sstream>

Shader::Shader()
{
    shaderProgram = 0;
}

void Shader::setFilenames(string vertexFilename, string fragmentFilename)
{
    vFilename = vertexFilename;
    fFilename = fragmentFilename;
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

bool Shader::loadShader()
{
    errorMessage = "";
    deleteShader();

    if(vFilename.empty() || fFilename.empty())
    {
        errorMessage = "Shader source filenames not set";
        return false;
    }

    shaderProgram = glCreateProgram();
    if(shaderProgram == 0)
    {
        errorMessage = "Unable to create shader program";
        return false;
    }

    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    if(vShader == 0)
    {
        errorMessage = "Unable to create vertex shader";
        deleteShader();
        return false;
    }

    string vTextString = readFile(vFilename);
    if(vTextString.empty())
    {
        glDeleteShader(vShader);
        deleteShader();
        return false;
    }

    const char* vText = vTextString.c_str();
    glShaderSource(vShader, 1, &vText, NULL);
    glCompileShader(vShader);

    GLint vStatus;
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &vStatus);
    if(vStatus == GL_FALSE)
    {
        GLint logLength;
        glGetShaderiv(vShader, GL_INFO_LOG_LENGTH , &logLength);

        GLchar* compilerLog = new GLchar[logLength];
        glGetShaderInfoLog(vShader, logLength, NULL, compilerLog);

        errorMessage = "Error compiling vertex shader: ";
        errorMessage += compilerLog;
        delete[] compilerLog;

        glDeleteShader(vShader);
        deleteShader();
        
        return false;
    }
    glAttachShader(shaderProgram, vShader);

    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    if(fShader == 0)
    {
        errorMessage = "Unable to create fragment shader";
        glDetachShader(shaderProgram, vShader);
        glDeleteShader(vShader);
        deleteShader();
        return false;
    }

    string fTextString = readFile(fFilename);
    if(fTextString.empty())
    {
        glDetachShader(shaderProgram, vShader);
        glDeleteShader(vShader);
        glDeleteShader(fShader);
        deleteShader();
        return false;
    }

    const char* fText = fTextString.c_str();
    glShaderSource(fShader, 1, &fText, NULL);
    glCompileShader(fShader);

    GLint fStatus;
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &fStatus);
    if(fStatus == GL_FALSE)
    {
        GLint logLength;
        glGetShaderiv(fShader, GL_INFO_LOG_LENGTH , &logLength);

        GLchar* compilerLog = new GLchar[logLength];
        glGetShaderInfoLog(fShader, logLength, NULL, compilerLog);

        errorMessage = "Error compiling fragment shader: ";
        errorMessage += compilerLog;
        delete[] compilerLog;

        glDetachShader(shaderProgram, vShader);
        glDeleteShader(vShader);
        glDeleteShader(fShader);

        deleteShader();

        return false;
    }
    glAttachShader(shaderProgram, fShader);

    glLinkProgram(shaderProgram);

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

        glDetachShader(shaderProgram, vShader);
        glDeleteShader(vShader);
        glDetachShader(shaderProgram, fShader);
        glDeleteShader(fShader);

        deleteShader();

        return false;
    }

    glDetachShader(shaderProgram, vShader);
    glDeleteShader(vShader);
    glDetachShader(shaderProgram, fShader);
    glDeleteShader(fShader);

    unbind();

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
    return vFilename + " " + fFilename;
}

string Shader::getError()
{
    return errorMessage;
}

GLuint Shader::getHandle()
{
    return shaderProgram;
}
