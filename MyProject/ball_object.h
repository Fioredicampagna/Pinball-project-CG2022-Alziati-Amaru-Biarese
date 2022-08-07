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
    float alfa = 0.1288712254f;	
    float   Radius;
    glm::vec3 AccelerationTot;
    glm::vec3 AccelerationGravity;
    glm::vec3 Speed;
           
    // constructor(s)
    BallObject(): GameObject(), Radius(12.5f), AccelerationTot(glm::vec3(0.0f)), Speed(glm::vec3(0.0f)) { };
    BallObject(glm::vec3 pos, float radius, glm::vec3 rotation, struct Model model): GameObject(pos, rotation, model), Radius(radius), AccelerationTot(glm::vec3(0.0f)),  Speed(glm::vec3(0.0f)) {};    // moves the ball, keeping it constrained within the window bounds (except bottom edge); returns new position
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
    
    bool bounce(GameObject &other) {
        if(SphereRectCollision(other)){
            glm::vec3 relPosition = glm::vec3(glm::inverse(other.transform) * glm::vec4(this->Position, 1.0));
            
            glm::vec3 distance = relPosition - other.CollisionBox.Center;
            glm::vec3 norm;
            glm::vec3 accVersor = glm::vec3(glm::inverse(other.transform) * glm::vec4(AccelerationTot, 1.0));
            
            accVersor = glm::normalize(accVersor);

            
           

            



            
            float relativeAngle = atan(distance.z / distance.x);
            
            float threshold1 = atan(( other.CollisionBox.Size.z / 2.0) / (- other.CollisionBox.Size.x / 2.0)); // upper left
            float threshold2 = atan(( -other.CollisionBox.Size.z / 2.0) / (- other.CollisionBox.Size.x / 2.0)); // lower left
        
            
            if((relativeAngle >= threshold1 && relativeAngle < threshold2) && relPosition.x < other.CollisionBox.Center.x){
                // caso lato corto destra
                printf("lato corto destra\n");

                norm = glm::vec3(-1.0f, 0.0f, 0.0f);

            } else if((relativeAngle >= threshold1 && relativeAngle < threshold2) && relPosition.x > other.CollisionBox.Center.x){
                // caso lato corto sinistra
                printf("lato corto sinistra\n");

                norm = glm::vec3(1.0f, 0.0f, 0.0f);
                
            } else if(((relativeAngle >= -glm::pi<float>() / 2.0f && relativeAngle < threshold1) || (relativeAngle >= threshold2  && relativeAngle < glm::pi<float>() / 2.0f)) && relPosition.z < other.CollisionBox.Center.z ) {
                // caso lato lungo sotto
                printf("lato lungo sotto\n");

                norm = glm::vec3(0.0f, 0.0f, -1.0f);
                glm::vec3 normWorld = glm::vec3(other.transform * glm::vec4(norm, 1.0f));
                glm::vec3 reflected2 = glm::reflect(glm::normalize(AccelerationTot), normWorld);
                glm::vec3 bounceAcc = glm::length(AccelerationTot)*glm::normalize(reflected2);
                bounceAcc.z = -bounceAcc.z;
                AccelerationTot = bounceAcc;
                Speed.x = glm::sign(AccelerationTot.x)*std::abs(Speed.x);
                Speed.y = glm::sign(AccelerationTot.y)*std::abs(Speed.y);
                Speed.z = glm::sign(AccelerationTot.z)*std::abs(Speed.z);
            
            return true;
            } else if(((relativeAngle >= -glm::pi<float>() / 2.0f && relativeAngle < threshold1) || (relativeAngle >= threshold2 && relativeAngle < glm::pi<float>() / 2.0f)) && relPosition.z > other.CollisionBox.Center.z ) {
                // caso lato lungo sopra
                printf("lato lungo sopra\n");

                norm = glm::vec3(0.0f, 0.0f, 1.0f);
            }

            
            
            //float dot = glm::dot(norm, accVersor);
            //glm::vec3 reflected = accVersor - 2.0f * norm * dot;

            
            glm::vec3 reflected2 = glm::reflect(accVersor, norm);
            glm::vec3 normWorld = glm::vec3(other.transform * glm::vec4(norm, 1.0f));
            reflected2 = glm::reflect(glm::normalize(AccelerationTot), normWorld);
            
            
            //------------------------------------------------------------------------------------------------
            glm::vec3 bounceAcc = glm::length(AccelerationTot)*glm::normalize(reflected2);
            
            std::cout << bounceAcc.x << bounceAcc.y <<bounceAcc.z << "\n";
            std::cout<< "\n";
            
    

            AccelerationTot = bounceAcc;
            Speed.x = glm::sign(AccelerationTot.x)*std::abs(Speed.x);
            Speed.y = glm::sign(AccelerationTot.y)*std::abs(Speed.y);
            Speed.z = glm::sign(AccelerationTot.z)*std::abs(Speed.z);
            return true;
        }
        return false;
    }
    
};

#endif
