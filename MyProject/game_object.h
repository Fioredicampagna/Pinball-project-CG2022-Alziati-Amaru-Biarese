/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glm/glm.hpp>
#include "MyProject.hpp"


// Container object for holding all state relevant for a single
// game object entity. Each object in the game likely needs the
// minimal of state as described within GameObject.
class GameObject
{
public:
    // object state
    glm::vec3   Position, Size, Rotation;
    Model Model;
    // render state
    //Texture   Sprite;
    // constructor(s)
    GameObject(): Position(0.0f, 0.0f, 0.0f), Size(1.0f, 1.0f, 1.0f), Rotation(0.0f) { };
    GameObject(glm::vec3 pos, glm::vec3 size, glm::vec3 rotation, struct Model model): Position(pos), Size(size), Rotation(rotation), Model(model) { };
    // draw sprite
    //virtual void Draw();
    void getSize()
    {
        float
            min_x,
            max_x,
            min_y, max_y,
            min_z, max_z;
        min_x = max_x = Model.vertices[0].pos.x;
        min_y = max_y = Model.vertices[0].pos.y;
        min_z = max_z = Model.vertices[0].pos.z;
        for (int i = 0; i < Model.vertices.size(); i++)
        {
            if (Model.vertices[i].pos.x < min_x)
                min_x = Model.vertices[i].pos.x;
            if (Model.vertices[i].pos.x > max_x)
                max_x = Model.vertices[i].pos.x;
            if (Model.vertices[i].pos.y < min_y)
                min_y = Model.vertices[i].pos.y;
            if (Model.vertices[i].pos.y > max_y)
                max_y = Model.vertices[i].pos.y;
            if (Model.vertices[i].pos.z < min_z)
                min_z = Model.vertices[i].pos.z;
            if (Model.vertices[i].pos.z > max_z)
                max_z = Model.vertices[i].pos.z;
        }
        Size = glm::vec3(max_x - min_x, max_y - min_y, max_z - min_z);
    }
    
    void getSize(glm::mat4 transformMatrix)
    {
        float
            min_x,
            max_x,
            min_y, max_y,
            min_z, max_z;
        min_x = max_x =  Model.vertices[0].pos.x;
        min_y = max_y =  Model.vertices[0].pos.y;
        min_z = max_z =  Model.vertices[0].pos.z;
        
        
        for (int i = 0; i < Model.vertices.size(); i++)
        {
            Model.vertices[i].pos = glm::vec3(transformMatrix * glm::vec4(Model.vertices[i].pos, 0.0));
            if (Model.vertices[i].pos.x < min_x)
                min_x = Model.vertices[i].pos.x;
            if (Model.vertices[i].pos.x > max_x)
                max_x = Model.vertices[i].pos.x;
            if (Model.vertices[i].pos.y < min_y)
                min_y = Model.vertices[i].pos.y;
            if (Model.vertices[i].pos.y > max_y)
                max_y = Model.vertices[i].pos.y;
            if (Model.vertices[i].pos.z < min_z)
                min_z = Model.vertices[i].pos.z;
            if (Model.vertices[i].pos.z > max_z)
                max_z = Model.vertices[i].pos.z;
        }
        Size = glm::vec3(max_x - min_x, max_y - min_y, max_z - min_z);
    }
};

#endif
