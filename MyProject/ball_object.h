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
    glm::vec3 Acceleration;
    
    // constructor(s)
    BallObject(): GameObject(), Radius(12.5f), Acceleration(glm::vec3(0.0f))  { };
    BallObject(glm::vec3 pos, float radius, glm::vec3 rotation, struct Model model): GameObject(pos, rotation, model), Radius(radius), Acceleration(glm::vec3(0.0f)) { };    // moves the ball, keeping it constrained within the window bounds (except bottom edge); returns new position
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
        
    glm::vec3 relPosition = glm::vec3(glm::inverse(other.transform) * glm::vec4(this->Position, 1.0));
   
    float sphereXDistance = abs(relPosition.x - other.CollisionBox.Center.x);
    float sphereYDistance = abs(relPosition.y - other.CollisionBox.Center.y);
    float sphereZDistance = abs(relPosition.z - other.CollisionBox.Center.z);

    if (sphereXDistance >= (other.CollisionBox.Size.x / 2.0 + this->Radius)) { return false; }
    if (sphereYDistance >= (other.CollisionBox.Size.y / 2.0 + this->Radius)) { return false; }
    if (sphereZDistance >= (other.CollisionBox.Size.z / 2.0 + this->Radius)) { return false; }

    if (sphereXDistance < (other.CollisionBox.Size.x / 2.0)) { return true; }
    if (sphereYDistance < (other.CollisionBox.Size.y / 2.0)) { return true; }
    if (sphereZDistance < (other.CollisionBox.Size.z / 2.0)) { return true; }

    float cornerDistance_sq = ((sphereXDistance - other.CollisionBox.Size.x / 2.0) * (sphereXDistance - other.CollisionBox.Size.x/ 2.0)) +
                         ((sphereYDistance - other.CollisionBox.Size.y/ 2.0) * (sphereYDistance - other.CollisionBox.Size.y/ 2.0) +
                         ((sphereYDistance - other.CollisionBox.Size.z/ 2.0) * (sphereYDistance - other.CollisionBox.Size.z/ 2.0)));

    return (cornerDistance_sq < (this->Radius * this->Radius));
        
    }
    
    void bounce(GameObject &other) {
        if(SphereRectCollision(other)){
            glm::vec3 relPosition = glm::vec3(glm::inverse(other.transform) * glm::vec4(this->Position, 1.0));
            
            glm::vec3 distance = relPosition - other.CollisionBox.Center;
            glm::vec3 norm;
            
            float relativeAngle = asin(distance.z / distance.x);
            
            float threshold1 = asin(( other.CollisionBox.Size.z / 2.0) / (- other.CollisionBox.Size.x / 2.0)); // upper left
            float threshold2 = asin(( -other.CollisionBox.Size.z / 2.0) / (- other.CollisionBox.Size.x / 2.0)); // lower left
        
            
            if((relativeAngle >= threshold1 && relativeAngle < threshold2) && relPosition.x < other.CollisionBox.Center.x){
                // caso lato corto sinistra
                printf("lato corto sinistra");
            } else if((relativeAngle >= threshold1 && relativeAngle < threshold2) && relPosition.x > other.CollisionBox.Center.x){
                // caso lato corto destra
                printf("lato corto destra");
            } else if(((relativeAngle >= -glm::pi<float>() / 2.0f && relativeAngle < threshold1) || (relativeAngle >= threshold2  && relativeAngle < glm::pi<float>() / 2.0f)) && relPosition.z < other.CollisionBox.Center.z ) {
                // caso lato lungo sotto
                printf("lato lungo sotto");
            } else if(((relativeAngle >= -glm::pi<float>() / 2.0f && relativeAngle < threshold1) || (relativeAngle >= threshold2 && relativeAngle < glm::pi<float>() / 2.0f)) && relPosition.z > other.CollisionBox.Center.z ) {
                // caso lato lungo sopra
                printf("lato lungo sopra");
            }
            
        }
    }
    
};

#endif
