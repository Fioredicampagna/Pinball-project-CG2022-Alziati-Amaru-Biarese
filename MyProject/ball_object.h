/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef BALLOBJECT_H
#define BALLOBJECT_H


#include <glm/glm.hpp>

#include "game_object.h"



// BallObject holds the state of the Ball object inheriting
// relevant state data from GameObject. Contains some extra
// functionality specific to Breakout's ball object that
// were too specific for within GameObject alone.
class BallObject : public GameObject
{
public:
    // ball state	
    float   Radius;
    
    // constructor(s)
    BallObject(): GameObject(), Radius(12.5f)   { };
    BallObject(glm::vec3 pos, float radius, glm::vec3 rotation, struct Model model): GameObject(pos, glm::vec3(radius * 2.0f, radius * 2.0f, radius * 2.0f), rotation, model), Radius(radius) { };
    // moves the ball, keeping it constrained within the window bounds (except bottom edge); returns new position
    //glm::vec2 Move(float dt, unsigned int window_width);
    // resets the ball to original state with given position and velocity
    //void      Reset(glm::vec3 position, glm::vec2 velocity);
};

#endif
