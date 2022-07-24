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
};

#endif
