#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <glm/glm.hpp>
#include "BoundingBox.h"
#include "MyProject.hpp"


// Container object for holding all state relevant for a single
// game object entity. Each object in the game likely needs the
// minimal of state as described within GameObject.
class GameObject
{
public:
    // object state
    glm::vec3   Position, Rotation;
    glm::mat4 transform;
    
    Model Model;
    BoundingBox CollisionBox;
    // render state
    //Texture   Sprite;
    // constructor(s)
    GameObject(): Position(0.0f, 0.0f, 0.0f), Rotation(0.0f), CollisionBox(BoundingBox()) { };
    GameObject(glm::vec3 pos, glm::vec3 rotation, struct Model model): Position(pos), Rotation(rotation), Model(model), CollisionBox(BoundingBox()) { };
    // draw sprite
    //virtual void Draw();
    
    
};

#endif
