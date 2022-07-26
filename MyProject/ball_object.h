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
    BallObject(glm::vec3 pos, float radius, glm::vec3 rotation, struct Model model): GameObject(pos, rotation, model), Radius(radius) { };    // moves the ball, keeping it constrained within the window bounds (except bottom edge); returns new position
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
        
   
    float sphereXDistance = abs(this->Position.x - other.CollisionBox.Center.x);
    float sphereYDistance = abs(this->Position.y - other.CollisionBox.Center.y);
    float sphereZDistance = abs(this->Position.z - other.CollisionBox.Center.z);

    if (sphereXDistance >= (other.CollisionBox.Size.x + this->Radius)) { return false; }
    if (sphereYDistance >= (other.CollisionBox.Size.y + this->Radius)) { return false; }
    if (sphereZDistance >= (other.CollisionBox.Size.z + this->Radius)) { return false; }

    if (sphereXDistance < (other.CollisionBox.Size.x)) { return true; }
    if (sphereYDistance < (other.CollisionBox.Size.y)) { return true; }
    if (sphereZDistance < (other.CollisionBox.Size.z)) { return true; }

    float cornerDistance_sq = ((sphereXDistance - other.CollisionBox.Size.x) * (sphereXDistance - other.CollisionBox.Size.x)) +
                         ((sphereYDistance - other.CollisionBox.Size.y) * (sphereYDistance - other.CollisionBox.Size.y) +
                         ((sphereYDistance - other.CollisionBox.Size.z) * (sphereYDistance - other.CollisionBox.Size.z)));

    return (cornerDistance_sq < (this->Radius * this->Radius));
        
    }
};

#endif
