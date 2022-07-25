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
    
    bool SphereRectCollision( GameObject &other) {
        
   
    float sphereXDistance = abs(this->Position.x - other.BBcenter.x);
    float sphereYDistance = abs(this->Position.y - other.BBcenter.y);
    float sphereZDistance = abs(this->Position.z - other.BBcenter.z);

    if (sphereXDistance >= (other.Size.x + this->Radius)) { return false; }
    if (sphereYDistance >= (other.Size.y + this->Radius)) { return false; }
    if (sphereZDistance >= (other.Size.z + this->Radius)) { return false; }

    if (sphereXDistance < (other.Size.x)) { return true; }
    if (sphereYDistance < (other.Size.y)) { return true; }
    if (sphereZDistance < (other.Size.z)) { return true; }

    float cornerDistance_sq = ((sphereXDistance - other.Size.x) * (sphereXDistance - other.Size.x)) +
                         ((sphereYDistance - other.Size.y) * (sphereYDistance - other.Size.y) +
                         ((sphereYDistance - other.Size.z) * (sphereYDistance - other.Size.z)));

    return (cornerDistance_sq < (this->Radius * this->Radius));
        
    }
};

#endif
