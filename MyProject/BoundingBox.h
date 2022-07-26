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
    glm::vec3 CenterModel, MinXModel, MaxXModel, MinYModel, MaxYModel, MinZModel, MaxZModel;
    
    BoundingBox(): Size(0.0f, 0.0f, 0.0f), Center(0.0f, 0.0f, 0.0f), MinX(0.0f, 0.0f, 0.0f), MaxX(0.0f, 0.0f, 0.0f),
    MinY(0.0f, 0.0f, 0.0f), MaxY(0.0f, 0.0f, 0.0f), MinZ(0.0f, 0.0f, 0.0f), MaxZ(0.0f, 0.0f, 0.0f) { };
    
    void getSize(Model model)
    {
       
        MinXModel = MaxXModel = MinYModel = MaxYModel = MinZModel = MaxZModel = model.vertices[0].pos;
          for (int i = 0; i < model.vertices.size(); i++) {
              if (model.vertices[i].pos.x < MinXModel.x) MinXModel = model.vertices[i].pos;
              if (model.vertices[i].pos.x > MaxXModel.x) MaxXModel = model.vertices[i].pos;
              if (model.vertices[i].pos.y < MinYModel.y) MinYModel = model.vertices[i].pos;
              if (model.vertices[i].pos.y > MaxYModel.y) MaxYModel = model.vertices[i].pos;
              if (model.vertices[i].pos.z < MinZModel.z) MinZModel = model.vertices[i].pos;
              if (model.vertices[i].pos.z > MaxZModel.z) MaxZModel = model.vertices[i].pos;
          }
          Size = glm::vec3(MaxXModel.x - MinXModel.x, MaxYModel.y - MinYModel.y, MaxZModel.z - MinZModel.z);
          CenterModel = glm::vec3((MinXModel.x + MaxXModel.x)/2, (MinYModel.y + MaxYModel.y)/2, (MinZModel.z + MaxZModel.z)/2);
    }
    
    void transformBox(glm::mat4 transformMatrix) {
        MinX = glm::vec3(transformMatrix * glm::vec4(MinXModel, 1.0));
        MaxX = glm::vec3(transformMatrix * glm::vec4(MaxXModel, 1.0));
        MinY = glm::vec3(transformMatrix * glm::vec4(MinYModel, 1.0));
        MaxY = glm::vec3(transformMatrix * glm::vec4(MaxYModel, 1.0));
        MinZ = glm::vec3(transformMatrix * glm::vec4(MinZModel, 1.0));
        MaxZ = glm::vec3(transformMatrix * glm::vec4(MaxZModel, 1.0));
        Center = glm::vec3(transformMatrix * glm::vec4(CenterModel, 1.0));
    }
};

#endif /* BoundingBox_h */
