#include "entity.h"

Entity::Entity()
{
    model = NULL;
    texture = NULL;

    x = 0;
    y = 0;
    z = 0;
    rx = 0;
    ry = 0;
    rz = 0;

    updateModelMatrix();
}

void Entity::setModel(Model* newModel)
{
    model = newModel;
}

void Entity::setTexture(Texture* newTexture)
{
    texture = newTexture;
}

void Entity::setPosition(float newX, float newY, float newZ)
{
    x = newX;
    y = newY;
    z = newZ;
    updateModelMatrix();
}

void Entity::setOrientation(float newRX, float newRY, float newRZ)
{
    rx = newRX;
    ry = newRY;
    rz = newRZ;
    updateModelMatrix();
}

void Entity::updateModelMatrix()
{
    glm::mat4 t = glm::mat4(1.0f);
    t = glm::translate(t, glm::vec3(x, y, z));

    glm::mat4 r = glm::mat4(1.0f);

    float rxRadians = (rx * 3.1415) / 180;
    float ryRadians = (ry * 3.1415) / 180;
    float rzRadians = (rz * 3.1415) / 180;

    r = glm::rotate(r, rzRadians, glm::vec3(0.0, 0.0, 1.0));
    r = glm::rotate(r, ryRadians, glm::vec3(0.0, 1.0, 0.0));
    r = glm::rotate(r, rxRadians, glm::vec3(1.0, 0.0, 0.0));

    modelMatrix = t * r;
}

void Entity::draw()
{
    if(model == NULL || texture == NULL)
        return;

    glActiveTexture(GL_TEXTURE0);
    texture->bind();

    model->bind();

    glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glDrawElements(GL_TRIANGLES, model->getIndexCount(), GL_UNSIGNED_INT, 0);

    model->unbind();
    texture->unbind();
}
