//
//  BoundingBox.h
//  PinballProject
//
//  
//

#ifndef BoundingBox_h
#define BoundingBox_h
#include "MyProject.hpp"

class BoundingBox{
public:
    glm::vec3 Size, Center, MinX, MaxX, MinY, MaxY, MinZ, MaxZ;
    
    BoundingBox(): Size(0.0f, 0.0f, 0.0f), Center(0.0f, 0.0f, 0.0f), MinX(0.0f, 0.0f, 0.0f), MaxX(0.0f, 0.0f, 0.0f),
    MinY(0.0f, 0.0f, 0.0f), MaxY(0.0f, 0.0f, 0.0f), MinZ(0.0f, 0.0f, 0.0f), MaxZ(0.0f, 0.0f, 0.0f) { };
    
    void getSize(Model model)
    {
       
        MinX = MaxX = MinY = MaxY = MinZ = MaxZ = model.vertices[0].pos;
          for (int i = 0; i < model.vertices.size(); i++) {
              if (model.vertices[i].pos.x < MinX.x) MinX = model.vertices[i].pos;
              if (model.vertices[i].pos.x > MaxX.x) MaxX = model.vertices[i].pos;
              if (model.vertices[i].pos.y < MinY.y) MinY = model.vertices[i].pos;
              if (model.vertices[i].pos.y > MaxY.y) MaxY = model.vertices[i].pos;
              if (model.vertices[i].pos.z < MinZ.z) MinZ = model.vertices[i].pos;
              if (model.vertices[i].pos.z > MaxZ.z) MaxZ = model.vertices[i].pos;
          }
          Size = glm::vec3(MaxX.x - MinX.x, MaxY.y - MinY.y, MaxZ.z - MinZ.z);
          Center = glm::vec3((MinX.x + MaxX.x)/2, (MinY.y + MaxY.y)/2, (MinZ.z + MaxZ.z)/2);
          //Transform = glm::translate(glm::mat4(1), center) * glm::scale(glm::mat4(1), size);
    }
    
    void transformBox(glm::mat4 transformMatrix) {
        MinX = glm::vec3(transformMatrix * glm::vec4(MinX, 0.0));
        MaxX = glm::vec3(transformMatrix * glm::vec4(MaxX, 0.0));
        MinY = glm::vec3(transformMatrix * glm::vec4(MinY, 0.0));
        MaxY = glm::vec3(transformMatrix * glm::vec4(MaxY, 0.0));
        MinZ = glm::vec3(transformMatrix * glm::vec4(MinZ, 0.0));
        MaxZ = glm::vec3(transformMatrix * glm::vec4(MaxZ, 0.0));
        Center = glm::vec3(transformMatrix * glm::vec4(Center, 0.0));
    }
};

#endif /* BoundingBox_h */
