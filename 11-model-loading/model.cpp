#include "model.h"

#include <SDL3/SDL.h>

#include <fstream>
#include <sstream>
#include <vector>

Model::Model()
{
    indexCount = 0;
    vao = 0;

    for(int i = 0; i < 4; i++)
    {
        vbo[i] = 0;
    }
}

void Model::setFilename(string newModelFilename)
{
    filename = SDL_GetBasePath() + newModelFilename;
}

bool Model::loadOBJModel()
{
    deleteModel();

    if(filename.empty())
    {
        errorMessage = "Model filename not set";
        return false;
    }

    ifstream fileStream(filename);
    if(!fileStream)
    {
        errorMessage += "Cannot open file: ";
        errorMessage += filename;
        return false;
    }

    vector<float> fileVertexData;
    vector<float> fileTextureData;
    vector<float> fileNormalData;

    string line, lineIdentifier;
    float value1, value2, value3;

    while(getline(fileStream, line))
    {
        istringstream iss(line);
        iss >> lineIdentifier >> value1 >> value2 >> value3;

        if(lineIdentifier == "v")
        {
            fileVertexData.push_back(value1);
            fileVertexData.push_back(value2);
            fileVertexData.push_back(value3);
        }
        else if(lineIdentifier == "vt")
        {
            fileTextureData.push_back(value1);
            fileTextureData.push_back(value2);
        }
        else if(lineIdentifier == "vn")
        {
            fileNormalData.push_back(value1);
            fileNormalData.push_back(value2);
            fileNormalData.push_back(value3);
        }
    }

    fileStream.clear();
    fileStream.seekg(0);

    vector<GLfloat> vertices;
    vector<GLfloat> textureCoordinates;
    vector<GLfloat> normals;
    vector<GLuint> indices;

    int uniqueElements = 0;
    vector<string> elementBuffer;

    while(getline(fileStream, line))
    {
        istringstream iss(line);
        iss >> lineIdentifier;

        if(lineIdentifier != "f")
            continue;

        string element;
        for(int elementIndex = 0; elementIndex < 3; elementIndex++)
        {
            iss >> element;

            bool alreadyExists = false;
            for(int i = 0; i < (int) elementBuffer.size(); i++)
            {
                if(elementBuffer[i] == element)
                {
                    indices.push_back(i);
                    alreadyExists = true;
                    break;
                }
            }

            if(alreadyExists)
                continue;
            
            int vertexIndex, textureIndex, normalIndex;
            sscanf(element.c_str(), "%i/%i/%i", &vertexIndex, &textureIndex, &normalIndex);

            int vertexLocation = (vertexIndex - 1) * 3;
            vertices.push_back(fileVertexData[vertexLocation + 0]);
            vertices.push_back(fileVertexData[vertexLocation + 1]);
            vertices.push_back(fileVertexData[vertexLocation + 2]);

            int textureLocation = (textureIndex - 1) * 2;
            textureCoordinates.push_back(fileTextureData[textureLocation + 0]);
            textureCoordinates.push_back(fileTextureData[textureLocation + 1]);
 
            int normalLocation = (normalIndex - 1) * 3;
            normals.push_back(fileNormalData[normalLocation + 0]);
            normals.push_back(fileNormalData[normalLocation + 1]);
            normals.push_back(fileNormalData[normalLocation + 2]);

            indices.push_back(uniqueElements);

            elementBuffer.push_back(element);
            uniqueElements++;
        }

        element = "";
        iss >> element;
        if(element.length() != 0)
        {
            errorMessage = "File contains non-triangulated faces: ";
            errorMessage += filename;
            return false;
        }
    }

    fileStream.close();

    if(vertices.empty() || normals.empty() || textureCoordinates.empty() || indices.empty())
    {
        errorMessage = "Essential data missing from file: ";
        errorMessage += filename;
        return false;
    }

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(4, vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals[0]) * normals.size(), normals.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordinates[0]) * textureCoordinates.size(), textureCoordinates.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    indexCount = indices.size();

    return true;
}

void Model::deleteModel()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(4, vbo);

    indexCount = 0;
    vao = 0;

    for(int i = 0; i < 4; i++)
    {
        vbo[i] = 0;
    }

    errorMessage = "";
}

void Model::bind()
{
    glBindVertexArray(vao);
}

void Model::unbind()
{
    glBindVertexArray(0);
}

int Model::getIndexCount()
{
    return indexCount;
}

string Model::getFilename()
{
    return filename;
}

string Model::getError()
{
    return errorMessage;
}
