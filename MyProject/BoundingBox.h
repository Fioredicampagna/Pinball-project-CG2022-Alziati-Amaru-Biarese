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
    glm::vec3 Size, Center;
    
    BoundingBox(): Size(0.0f, 0.0f, 0.0f), Center(0.0f, 0.0f, 0.0f) { };
    
    void getSize(Model model)
    {
       
        float
            min_x, max_x,
            min_y, max_y,
            min_z, max_z;
          min_x = max_x = model.vertices[0].pos.x;
          min_y = max_y = model.vertices[0].pos.y;
          min_z = max_z = model.vertices[0].pos.z;
          for (int i = 0; i < model.vertices.size(); i++) {
              if (model.vertices[i].pos.x < min_x) min_x = model.vertices[i].pos.x;
              if (model.vertices[i].pos.x > max_x) max_x = model.vertices[i].pos.x;
              if (model.vertices[i].pos.y < min_y) min_y = model.vertices[i].pos.y;
              if (model.vertices[i].pos.y > max_y) max_y = model.vertices[i].pos.y;
              if (model.vertices[i].pos.z < min_z) min_z = model.vertices[i].pos.z;
              if (model.vertices[i].pos.z > max_z) max_z = model.vertices[i].pos.z;
          }
          Size = glm::vec3(max_x-min_x, max_y-min_y, max_z-min_z);
          Center = glm::vec3((min_x+max_x)/2, (min_y+max_y)/2, (min_z+max_z)/2);
    }
    
};

#endif /* BoundingBox_h */
