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
    
    void getRadius()
    {
        float max_x;
        int max_x_index = 0;
        max_x = Model.vertices[0].pos.x;
        for (int i = 0; i < Model.vertices.size(); i++)
        {
            if (Model.vertices[i].pos.x > max_x)
            {
                max_x = Model.vertices[i].pos.x;
                max_x_index = i;
            }
        }

        glm::vec3 distance = glm::vec3(0.0f) - Model.vertices[max_x_index].pos;

        Radius = glm::length(distance);
    }
    
    bool CheckCollision(GameObject &other) // AABB - Circle collision
    {
        // get center point circle first
        glm::vec3 center(this->Position + this->Radius);
        // calculate AABB info (center, half-extents)
        glm::vec3 aabb_half_extents(other.Size.x / 2.0f, other.Size.y / 2.0f, other.Size.z / 2.0f);
        
        glm::vec3 aabb_center(
            other.Position.x + aabb_half_extents.x,
            other.Position.y + aabb_half_extents.y,
            other.Position.z + aabb_half_extents.z);
        // get difference vector between both centers
        glm::vec3 difference = center - aabb_center;
        glm::vec3 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
        // add clamped value to AABB_center and we get the value of box closest to circle
        glm::vec3 closest = aabb_center + clamped;
        // retrieve vector between center circle and closest point AABB and check if length <= radius
        difference = closest - center;
        return glm::length(difference) < this->Radius;
    }
};

#endif
