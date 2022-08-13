// This has been adapted from the Vulkan tutorial

#include "MyProject.hpp"
#include "game_object.h"
#include "ball_object.h"

const std::string MODEL_PATH = "models/PinballDark/";
const std::string TEXTURE_PATH = "textures/StarWarsPinball.png";

// The uniform buffer object used in this example
struct globalUniformBufferObject
{
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

struct UniformBufferObject
{
    alignas(16) glm::mat4 model;
};


// MAIN !
class MyProject : public BaseProject
{
protected:
    // variabili varie

    float speed = 0.05f;

    // Massima lunghezza della molla del puller
    float maxPullerLenght = 0.5f;
    float pullerStartingPosition = -7.5892f;
    float pullerActualPosition = pullerStartingPosition;
    float pullerState = 0.0f; // 0 stato a  riposo
                              // 1 tensione massima

    float leftFlipperRotation = -29.8f;
    float maxLeftFlipperRotation = 29.8f;

    bool startRotationLeft = false;
    bool startRotationRight = false;

    float deltaRotation = 9.0f;

    float rightFlipperRotation = -150.0f;
    float maxRightFlipperRotation = -210.0f;

    float buttonDepth = 0.03f;
    bool buttonLeftPressed = false;
    bool buttonRightPressed = false;

    float alfa = 0.1288712254f;
    float ballStartz = -5.4828f + ball.Radius + 0.03;
    float ballStartx = -2.5264f;
    float ballStarty = 8.4032f + std::abs(ballStartz - -5.9728f) * std::tan(alfa);
    /*
    float ballStartz = 3.9f;
    float ballStartx = 0.0f;
    float ballStarty = 8.4032f + std::abs(ballStartz - -5.9728f) * std::tan(alfa);
    */
    
    float dz = 0.0f;
    float dy = 0.0f;
    float dx = 0.0f;

    float vz = 0.0f;
    float vy = 0.0f;
    float vx = 0.0f;


    // variabili per la matrice camera
    glm::vec3 cameraPos = glm::vec3(0.0f, 17.0f, -15.0f);
    glm::vec3 pinballPos = glm::vec3(0.0f, 7.0f, 0.0f);
    float lookYaw = 0.0f;
    float lookPitch = 0.0f;
    glm::vec3 FollowerDeltaTarget = cameraPos - pinballPos;
    const float ROT_SPEED = glm::radians(0.5f);
    float followerDist = 0.6;

    // Here you list all the Vulkan objects you need:

    // Descriptor Layouts [what will be passed to the shaders]
    DescriptorSetLayout DSLglobal;
    DescriptorSetLayout DSLobj;

    // Pipelines [Shader couples]
    Pipeline P1;

    // Models, textures and Descriptors (values assigned to the uniforms)

    // 00: Pinball Texture: needs to be loaded just once, shared between every object
    Texture T_Pinball;

    // 01: Pinball Model
    Model M_PinballBody;
    DescriptorSet DS_PinballBody;

    // 02: Ball Model
    Model M_Ball;
    DescriptorSet DS_Ball;

    // 03: Bumper Model and instances
    Model M_Bumper;
    DescriptorSet DS_Bumper1;
    DescriptorSet DS_Bumper2;
    DescriptorSet DS_Bumper3;
    DescriptorSet *DS_Bumper_Array[3] = {&DS_Bumper1, &DS_Bumper2, &DS_Bumper3};

    // 04: Puller Model
    Model M_Puller;
    DescriptorSet DS_Puller;

    // 05: Flipper Model and instances
    Model M_Flipper;
    DescriptorSet DS_LeftFlipper;
    DescriptorSet DS_RightFlipper;
    DescriptorSet *DS_Flipper_Array[2] = {&DS_LeftFlipper, &DS_RightFlipper};

    // 06: Flipper Button and instances
    Model M_Button;
    DescriptorSet DS_LeftButton;
    DescriptorSet DS_RightButton;
    DescriptorSet *DS_Button_Array[2] = {&DS_LeftButton, &DS_RightButton};

    // 07: Score indicator and instances
    Model M_Score;
    DescriptorSet DS_DL1;
    DescriptorSet DS_DL2;
    DescriptorSet DS_DL3;
    DescriptorSet DS_DL4;
    DescriptorSet DS_DL5;
    DescriptorSet DS_DL6;
    DescriptorSet DS_DR1;
    DescriptorSet DS_DR2;
    DescriptorSet DS_DR3;
    DescriptorSet DS_DR4;
    DescriptorSet DS_DR5;
    DescriptorSet DS_DR6;
    DescriptorSet *DS_Score_Array[12] = {&DS_DL1, &DS_DL2, &DS_DL3, &DS_DL4, &DS_DL5, &DS_DL6, &DS_DR1, &DS_DR2, &DS_DR3, &DS_DR4, &DS_DR5, &DS_DR6};

    DescriptorSet DS_global;

   // game objects
    
    GameObject bumper1, bumper2, bumper3, leftFlipper, rightFlipper, puller;
    BallObject ball;
    
    // Here you set the main application parameters
    void setWindowParameters()
    {
        // window size, titile and initial background
        windowWidth = 800;
        windowHeight = 600;
        windowTitle = "Pinball";
        initialBackgroundColor = {1.0f, 1.0f, 1.0f, 1.0f};

        // Descriptor pool sizes
        uniformBlocksInPool = 24;
        texturesInPool = 24;
        setsInPool = 24;
    }

    // Here you load and setup all your Vulkan objects
    void localInit()
    {
        // Descriptor Layouts [what will be passed to the shaders]
        DSLobj.init(this, {// this array contains the binding:
                           // first  element : the binding number
                           // second element : the time of element (buffer or texture)
                           // third  element : the pipeline stage where it will be used
                           {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
                           {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}});

        DSLglobal.init(this, {
                                 {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS},
                             });

        // Pipelines [Shader couples]
        // The last array, is a vector of pointer to the layouts of the sets that will
        // be used in this pipeline. The first element will be set 0, and so on..
        P1.init(this, "shaders/slotVert.spv", "shaders/slotFrag.spv", {&DSLglobal, &DSLobj});

        // Models, textures and Descriptors (values assigned to the uniforms)
        M_PinballBody.init(this, MODEL_PATH + "Body.obj");
        T_Pinball.init(this, TEXTURE_PATH);
        DS_PinballBody.init(this, &DSLobj, {// the second parameter, is a pointer to the Uniform Set Layout of this set
                                            // the last parameter is an array, with one element per binding of the set.
                                            // first  elmenet : the binding number
                                            // second element : UNIFORM or TEXTURE (an enum) depending on the type
                                            // third  element : only for UNIFORMs, the size of the corresponding C++ object
                                            // fourth element : only for TEXTUREs, the pointer to the corresponding texture object
                                            {0, UNIFORM, sizeof(UniformBufferObject), nullptr},
                                            {1, TEXTURE, 0, &T_Pinball}});

        M_Bumper.init(this, MODEL_PATH + "bumper1.obj");
        DS_Bumper1.init(this, &DSLobj, {{0, UNIFORM, sizeof(UniformBufferObject), nullptr}, {1, TEXTURE, 0, &T_Pinball}});
        DS_Bumper2.init(this, &DSLobj, {{0, UNIFORM, sizeof(UniformBufferObject), nullptr}, {1, TEXTURE, 0, &T_Pinball}});
        DS_Bumper3.init(this, &DSLobj, {{0, UNIFORM, sizeof(UniformBufferObject), nullptr}, {1, TEXTURE, 0, &T_Pinball}});

        
        
        bumper1 = GameObject(glm::vec3(1.1819f, 9.1362f, 0.020626f), glm::vec3(-6.51f, 0.0f, 0.0f), M_Bumper);
        bumper1.CollisionBox.getSize(bumper1.Model);
        bumper1.CollisionBox.Size = glm::vec3(glm::scale(glm::mat4(1.0f), glm::vec3(1.25f,1.25f,1.25f)) * glm::vec4(bumper1.CollisionBox.Size, 1.0f));

        bumper2 =  GameObject(glm::vec3(-1.5055f, 9.1362f, 0.020626f), glm::vec3(-6.51f, 0.0f, 0.0f), M_Bumper);
        bumper2.CollisionBox.getSize(bumper2.Model);
        bumper2.CollisionBox.Size = glm::vec3(glm::scale(glm::mat4(1.0f), glm::vec3(1.25f,1.25f,1.25f)) * glm::vec4(bumper2.CollisionBox.Size, 1.0f));

        bumper3 = GameObject(glm::vec3(-0.11626f, 9.1362f, 0.020626f), glm::vec3(-6.51f, 0.0f, 0.0f), M_Bumper);
        bumper3.CollisionBox.getSize(bumper3.Model);
        bumper3.CollisionBox.Size = glm::vec3(glm::scale(glm::mat4(1.0f), glm::vec3(1.25f,1.25f,1.25f)) * glm::vec4(bumper3.CollisionBox.Size, 1.0f));
        
        M_Puller.init(this, MODEL_PATH + "Puller.obj");
        DS_Puller.init(this, &DSLobj, {{0, UNIFORM, sizeof(UniformBufferObject), nullptr}, {1, TEXTURE, 0, &T_Pinball}});
        
        puller = GameObject(glm::vec3(-2.5264f, 8.3925f, pullerActualPosition), glm::vec3(0.0f, -90.0f, 0.0f), M_Puller);
        puller.CollisionBox.getSize(puller.Model);

        

        M_Flipper.init(this, MODEL_PATH + "RightFlipper.obj");
        DS_LeftFlipper.init(this, &DSLobj, {{0, UNIFORM, sizeof(UniformBufferObject), nullptr}, {1, TEXTURE, 0, &T_Pinball}});
        DS_RightFlipper.init(this, &DSLobj, {{0, UNIFORM, sizeof(UniformBufferObject), nullptr}, {1, TEXTURE, 0, &T_Pinball}});
        
        
         
        leftFlipper = GameObject(glm::vec3(0.6906f, 8.4032f, -5.6357f), glm::vec3(leftFlipperRotation, -3.24f, -5.64f), M_Flipper);
        leftFlipper.CollisionBox.getSize(leftFlipper.Model);
        leftFlipper.CollisionBox.Size = glm::vec3(glm::scale(glm::mat4(1.0f), glm::vec3(1.25f,1.25f,1.25f)) * glm::vec4(leftFlipper.CollisionBox.Size, 1.0f));

        rightFlipper = GameObject(glm::vec3(-1.307f, 8.4032f, -5.6357f), glm::vec3(rightFlipperRotation, -3.24f, -5.64f), M_Flipper);
        rightFlipper.CollisionBox.getSize(rightFlipper.Model);
        rightFlipper.CollisionBox.Size = glm::vec3(glm::scale(glm::mat4(1.0f), glm::vec3(1.25f,1.25f,1.25f)) * glm::vec4(rightFlipper.CollisionBox.Size, 1.0f));
 
        
        M_Button.init(this, MODEL_PATH + "RightButton.obj");
        DS_LeftButton.init(this, &DSLobj, {{0, UNIFORM, sizeof(UniformBufferObject), nullptr}, {1, TEXTURE, 0, &T_Pinball}});
        DS_RightButton.init(this, &DSLobj, {{0, UNIFORM, sizeof(UniformBufferObject), nullptr}, {1, TEXTURE, 0, &T_Pinball}});

        M_Ball.init(this, MODEL_PATH + "Ball.obj");
        DS_Ball.init(this, &DSLobj, {{0, UNIFORM, sizeof(UniformBufferObject), nullptr}, {1, TEXTURE, 0, &T_Pinball}});
        
        ball = BallObject(glm::vec3(ballStartx + dx, std::max(ballStarty - dy, 8.4032f), std::max(ballStartz - dz, -5.6352f)), 0.0f, glm::vec3(0.0f, 0.0f, 0.0f), M_Ball );
        ball.getRadius();
        float apiano = 0.49f * sin(alfa);

        ball.AccelerationGravity.y = apiano * sin(alfa);
        ball.AccelerationGravity.z = apiano * cos(alfa);

       
        M_Score.init(this, MODEL_PATH + "DL6.obj");
        DS_DL1.init(this, &DSLobj, {{0, UNIFORM, sizeof(UniformBufferObject), nullptr}, {1, TEXTURE, 0, &T_Pinball}});
        DS_DL2.init(this, &DSLobj, {{0, UNIFORM, sizeof(UniformBufferObject), nullptr}, {1, TEXTURE, 0, &T_Pinball}});
        DS_DL3.init(this, &DSLobj, {{0, UNIFORM, sizeof(UniformBufferObject), nullptr}, {1, TEXTURE, 0, &T_Pinball}});
        DS_DL4.init(this, &DSLobj, {{0, UNIFORM, sizeof(UniformBufferObject), nullptr}, {1, TEXTURE, 0, &T_Pinball}});
        DS_DL5.init(this, &DSLobj, {{0, UNIFORM, sizeof(UniformBufferObject), nullptr}, {1, TEXTURE, 0, &T_Pinball}});
        DS_DL6.init(this, &DSLobj, {{0, UNIFORM, sizeof(UniformBufferObject), nullptr}, {1, TEXTURE, 0, &T_Pinball}});
        DS_DR1.init(this, &DSLobj, {{0, UNIFORM, sizeof(UniformBufferObject), nullptr}, {1, TEXTURE, 0, &T_Pinball}});
        DS_DR2.init(this, &DSLobj, {{0, UNIFORM, sizeof(UniformBufferObject), nullptr}, {1, TEXTURE, 0, &T_Pinball}});
        DS_DR3.init(this, &DSLobj, {{0, UNIFORM, sizeof(UniformBufferObject), nullptr}, {1, TEXTURE, 0, &T_Pinball}});
        DS_DR4.init(this, &DSLobj, {{0, UNIFORM, sizeof(UniformBufferObject), nullptr}, {1, TEXTURE, 0, &T_Pinball}});
        DS_DR5.init(this, &DSLobj, {{0, UNIFORM, sizeof(UniformBufferObject), nullptr}, {1, TEXTURE, 0, &T_Pinball}});
        DS_DR6.init(this, &DSLobj, {{0, UNIFORM, sizeof(UniformBufferObject), nullptr}, {1, TEXTURE, 0, &T_Pinball}});

        DS_global.init(this, &DSLglobal, {{0, UNIFORM, sizeof(globalUniformBufferObject), nullptr}});
    }

    // Here you destroy all the objects you created!
    void localCleanup()
    {
        DS_PinballBody.cleanup();
        M_PinballBody.cleanup();

        DS_Bumper1.cleanup();
        DS_Bumper2.cleanup();
        DS_Bumper3.cleanup();
        M_Bumper.cleanup();

        DS_Puller.cleanup();
        M_Puller.cleanup();

        DS_LeftFlipper.cleanup();
        DS_RightFlipper.cleanup();
        M_Flipper.cleanup();

        DS_LeftButton.cleanup();
        DS_RightButton.cleanup();
        M_Button.cleanup();

        DS_DL1.cleanup();
        DS_DL2.cleanup();
        DS_DL3.cleanup();
        DS_DL4.cleanup();
        DS_DL5.cleanup();
        DS_DL6.cleanup();
        DS_DR1.cleanup();
        DS_DR2.cleanup();
        DS_DR3.cleanup();
        DS_DR4.cleanup();
        DS_DR5.cleanup();
        DS_DR6.cleanup();
        M_Score.cleanup();

        DS_Ball.cleanup();
        M_Ball.cleanup();

        T_Pinball.cleanup();
        DS_global.cleanup();

        P1.cleanup();
        DSLglobal.cleanup();
        DSLobj.cleanup();
    }

    // Here it is the creation of the command buffer:
    // You send to the GPU all the objects you want to draw,
    // with their buffers and textures
    void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage)
    {

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          P1.graphicsPipeline);
        vkCmdBindDescriptorSets(commandBuffer,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                P1.pipelineLayout, 0, 1, &DS_global.descriptorSets[currentImage],
                                0, nullptr);

        drawModel(commandBuffer, currentImage, M_PinballBody, DS_PinballBody, P1);

        drawModelInstanced(commandBuffer, currentImage, M_Bumper, DS_Bumper_Array, P1, 3);

        drawModel(commandBuffer, currentImage, M_Puller, DS_Puller, P1);

        drawModelInstanced(commandBuffer, currentImage, M_Flipper, DS_Flipper_Array, P1, 2);

        drawModelInstanced(commandBuffer, currentImage, M_Button, DS_Button_Array, P1, 2);

        drawModelInstanced(commandBuffer, currentImage, M_Score, DS_Score_Array, P1, 12);

        drawModel(commandBuffer, currentImage, M_Ball, DS_Ball, P1);
    }

    
    
    
    
    // Here is where you update the uniforms.
    // Very likely this will be where you will be writing the logic of your application.
    void updateUniformBuffer(uint32_t currentImage)
    {

        static auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
        static float lastTime = 0.0f;
        float deltaT = time - lastTime;

        // static float debounce = time;

       


         /* glm::vec3 RFDT = glm::vec3(glm::rotate(glm::mat4(1), lookYaw, glm::vec3(0, 1, 0)) *
                                    glm::vec4(FollowerDeltaTarget, 1.0f));*/
        updateCamera(deltaT);

        updatePuller();
        
        updateFlippers();
        
        
        /*if(!ball.SphereRectCollision(leftFlipper) && !ball.SphereRectCollision(rightFlipper)
           &&!ball.SphereRectCollision(bumper1) && !ball.SphereRectCollision(bumper2) && !ball.SphereRectCollision(bumper3)) {
            updateBallPosition();
        } */
        if(ball.inGame)
            updateBallPosition();
        
        
        

        globalUniformBufferObject gubo{};
        UniformBufferObject ubo{};

        void *data;

        gubo.view = glm::lookAt(cameraPos,
                                pinballPos /*+ RFDT*/,
                                glm::vec3(0.0f, 1.0f, 0.0f));
        gubo.proj = glm::perspective(glm::radians(45.0f),
                                     swapChainExtent.width / (float)swapChainExtent.height,
                                     0.1f, 100.0f);
        gubo.proj[1][1] *= -1;

        vkMapMemory(device, DS_global.uniformBuffersMemory[0][currentImage], 0,
                    sizeof(gubo), 0, &data);
        memcpy(data, &gubo, sizeof(gubo));
        vkUnmapMemory(device, DS_global.uniformBuffersMemory[0][currentImage]);

        
        updateScene(currentImage, data, ubo);
    }

    
    
    
    
    
    glm::mat4 MakeWorldMatrixEuler(glm::vec3 pos, glm::vec3 YPR, glm::vec3 size)
    {
        glm::mat4 out = glm::translate(glm::mat4(1.0), pos) * glm::rotate(glm::mat4(1.0), glm::radians(YPR.x), glm::vec3(0, 1, 0)) * glm::rotate(glm::mat4(1.0), glm::radians(YPR.y), glm::vec3(1, 0, 0)) * glm::rotate(glm::mat4(1.0), glm::radians(YPR.z), glm::vec3(0, 0, 1)) * glm::scale(glm::mat4(1.0), size);
        return out;
    }

    void drawModel(VkCommandBuffer commandBuffer, int currentImage, Model model, DescriptorSet DS, Pipeline P)
    {
        VkBuffer vertexBuffers[] = {model.vertexBuffer};
        // property .vertexBuffer of models, contains the VkBuffer handle to its vertex buffer
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        // property .indexBuffer of models, contains the VkBuffer handle to its index buffer
        vkCmdBindIndexBuffer(commandBuffer, model.indexBuffer, 0,
                             VK_INDEX_TYPE_UINT32);

        // property .pipelineLayout of a pipeline contains its layout.
        // property .descriptorSets of a descriptor set contains its elements.
        vkCmdBindDescriptorSets(commandBuffer,
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                P.pipelineLayout, 1, 1, &DS.descriptorSets[currentImage],
                                0, nullptr);

        // property .indices.size() of models, contains the number of triangles * 3 of the mesh.
        vkCmdDrawIndexed(commandBuffer,
                         static_cast<uint32_t>(model.indices.size()), 1, 0, 0, 0);
    }

    void drawModelInstanced(VkCommandBuffer commandBuffer, int currentImage, Model model, DescriptorSet **DS_Array, Pipeline P, int nInstance)
    {
        VkBuffer vertexBuffers[] = {model.vertexBuffer};
        // property .vertexBuffer of models, contains the VkBuffer handle to its vertex buffer
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        // property .indexBuffer of models, contains the VkBuffer handle to its index buffer
        vkCmdBindIndexBuffer(commandBuffer, model.indexBuffer, 0,
                             VK_INDEX_TYPE_UINT32);

        // property .pipelineLayout of a pipeline contains its layout.
        // property .descriptorSets of a descriptor set contains its elements.
        for (int i = 0; i < nInstance; i++)
        {
            vkCmdBindDescriptorSets(commandBuffer,
                                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    P.pipelineLayout, 1, 1, &DS_Array[i]->descriptorSets[currentImage],
                                    0, nullptr);

            // property .indices.size() of models, contains the number of triangles * 3 of the mesh.
            vkCmdDrawIndexed(commandBuffer,
                             static_cast<uint32_t>(model.indices.size()), 1, 0, 0, 0);
        }
    }

    void mapAndUnmap(int currentImage, DescriptorSet DS, void *data, UniformBufferObject ubo)
    {
        vkMapMemory(device, DS.uniformBuffersMemory[0][currentImage], 0,
                    sizeof(ubo), 0, &data);
        memcpy(data, &ubo, sizeof(ubo));
        vkUnmapMemory(device, DS.uniformBuffersMemory[0][currentImage]);
    }

    void updateModel(int currentImage, DescriptorSet DS, void *data, UniformBufferObject ubo, glm::vec3 position, glm::vec3 rotation)
    {
        ubo.model = MakeWorldMatrixEuler(position, rotation, glm::vec3(1.0f));

        mapAndUnmap(currentImage, DS, data, ubo);
    }
    
    void updateModel(int currentImage, DescriptorSet DS, void *data, UniformBufferObject ubo, glm::vec3 position, glm::vec3 rotation, GameObject &object)
    {
        ubo.model = MakeWorldMatrixEuler(position, rotation, glm::vec3(1.0f));

        object.transform = ubo.model;
        
        
        mapAndUnmap(currentImage, DS, data, ubo);
    }
    
    void updateScene(int currentImage, void *data, UniformBufferObject ubo){
        // Pinball Body
        
        updateModel(currentImage, DS_PinballBody, data, ubo, glm::vec3(0.0f), glm::vec3(0.0f));
        
        // bumper 1
        
        updateModel(currentImage, DS_Bumper1, data, ubo, bumper1.Position, bumper1.Rotation, bumper1);

        // bumper 2
        
        updateModel(currentImage, DS_Bumper2, data, ubo, bumper2.Position, bumper2.Rotation, bumper2);

        // bumper 3
        
        updateModel(currentImage, DS_Bumper3, data, ubo, bumper3.Position, bumper3.Rotation, bumper3);

        // puller
        
        updateModel(currentImage, DS_Puller, data, ubo, puller.Position, puller.Rotation, puller);

        // left flipper
        
        
        updateModel(currentImage, DS_LeftFlipper, data, ubo, leftFlipper.Position, leftFlipper.Rotation, leftFlipper); // 29.8 max rotation
        

        // right flipper
        
        
        updateModel(currentImage, DS_RightFlipper, data, ubo, rightFlipper.Position, rightFlipper.Rotation, rightFlipper); // 150f max rotation

        // left button
        updateModel(currentImage, DS_LeftButton, data, ubo, glm::vec3(2.6175f - buttonDepth * buttonLeftPressed, 8.7853f, -6.6902f), glm::vec3(0.0f, 0.0f, -90.0f));

        // right button
        updateModel(currentImage, DS_RightButton, data, ubo, glm::vec3(-2.97f + buttonDepth * buttonRightPressed, 8.7853f, -6.6902f), glm::vec3(0.0f, 0.0f, 90.0f));

        // Score: 12 digits
        updateModel(currentImage, DS_DL1, data, ubo, glm::vec3(0.4366f, 12.789f, 4.1852f), glm::vec3(0.0f, -101.0f, 0.0f));
        updateModel(currentImage, DS_DL2, data, ubo, glm::vec3(0.713f, 12.789f, 4.1852f), glm::vec3(0.0f, -101.0f, 0.0f));
        updateModel(currentImage, DS_DL3, data, ubo, glm::vec3(0.9923f, 12.789f, 4.1852f), glm::vec3(0.0f, -101.0f, 0.0f));
        updateModel(currentImage, DS_DL4, data, ubo, glm::vec3(1.3917f, 12.789f, 4.1852f), glm::vec3(0.0f, -101.0f, 0.0f));
        updateModel(currentImage, DS_DL5, data, ubo, glm::vec3(1.6681f, 12.789f, 4.1852f), glm::vec3(0.0f, -101.0f, 0.0f));
        updateModel(currentImage, DS_DL6, data, ubo, glm::vec3(1.9474f, 12.789f, 4.1852f), glm::vec3(0.0f, -101.0f, 0.0f));
        updateModel(currentImage, DS_DR1, data, ubo, glm::vec3(-2.8273f, 12.789f, 4.1852f), glm::vec3(0.0f, -101.0f, 0.0f));
        updateModel(currentImage, DS_DR2, data, ubo, glm::vec3(-2.5509f, 12.789f, 4.1852f), glm::vec3(0.0f, -101.0f, 0.0f));
        updateModel(currentImage, DS_DR3, data, ubo, glm::vec3(-2.2716f, 12.789f, 4.1852f), glm::vec3(0.0f, -101.0f, 0.0f));
        updateModel(currentImage, DS_DR4, data, ubo, glm::vec3(-1.8722f, 12.789f, 4.1852f), glm::vec3(0.0f, -101.0f, 0.0f));
        updateModel(currentImage, DS_DR5, data, ubo, glm::vec3(-1.5958f, 12.789f, 4.1852f), glm::vec3(0.0f, -101.0f, 0.0f));
        updateModel(currentImage, DS_DR6, data, ubo, glm::vec3(-1.316f, 12.789f, 4.1852f), glm::vec3(0.0f, -101.0f, 0.0f));
        
        
        
        // ball
        
        updateModel(currentImage, DS_Ball, data, ubo, ball.Position, glm::vec3(0.0f, 0.0f, 0.0f));

        std::cout << ball.Position.z << "\n";
    }

    void updateCamera(float deltaT)
    {
        
        // camera update
        if (glfwGetKey(window, GLFW_KEY_RIGHT))
        {
            // lookYaw += deltaT * ROT_SPEED;
            lookYaw += ROT_SPEED;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT))
        {
            // lookYaw -= deltaT * ROT_SPEED;
            lookYaw -= ROT_SPEED;
        }
        if (glfwGetKey(window, GLFW_KEY_ENTER))
        {
            lookYaw = 0.0f;
        } /*
         if (glfwGetKey(window, GLFW_KEY_UP))
         {
             //lookPitch += deltaT * ROT_SPEED;
             lookPitch += ROT_SPEED;
         }
         if (glfwGetKey(window, GLFW_KEY_DOWN))
         {
             //lookPitch -= deltaT * ROT_SPEED;
             lookPitch -= ROT_SPEED;
         } */
        
        glm::vec3 FollowerTargetPos;

        glm::mat4 pinballWM = glm::rotate(glm::translate(glm::mat4(1), pinballPos),
                                          lookYaw, glm::vec3(0, 1, 0));
        FollowerTargetPos = pinballWM * glm::translate(glm::mat4(1), FollowerDeltaTarget) *
                            glm::rotate(glm::mat4(1), lookPitch, glm::vec3(1, 0, 0)) *
                            glm::vec4(0.0f, 0.0f, followerDist, 1.0f);
        const float followerFilterCoeff = 7;
        float alpha = fmin(followerFilterCoeff * deltaT, 1.0f);
        cameraPos = cameraPos * (1.0f - alpha) + alpha * FollowerTargetPos;
    }
    
    void updatePuller(){
        glm::vec3 k = glm::vec3(-0.2f, -0.6f * tan(alfa), -0.6f);
        // Logica del tiraggio del puller
        if (glfwGetKey(window, GLFW_KEY_SPACE))
        {
            pullerState += speed;
            pullerState = std::min(pullerState, 1.0f);
            pullerActualPosition = pullerStartingPosition - maxPullerLenght * pullerState;
        }
        else
        {
            
            if(ball.inGame == false && pullerState != 0.0f)
            {
                ball.inGame = true;

                ball.AccelerationTot =  pullerState * k ;
            }
           
            pullerActualPosition = pullerActualPosition + 0.1 * pullerState;
            pullerActualPosition = std::min(pullerActualPosition, pullerStartingPosition);
            pullerState -= speed;
            pullerState = std::max(pullerState, 0.0f);
        }
        puller.Position = glm::vec3(-2.5264f, 8.3925f, pullerActualPosition);
    }
    
    void updateFlippers() {
        if (glfwGetKey(window, GLFW_KEY_A))
        {
            startRotationLeft = true;
            buttonLeftPressed = true;
        }
        else
        {
            buttonLeftPressed = false;
        }

        if (glfwGetKey(window, GLFW_KEY_L))
        {
            startRotationRight = true;
            buttonRightPressed = true;
        }
        else
        {
            buttonRightPressed = false;
        }

        if (startRotationLeft)
        {
            leftFlipperRotation += deltaRotation;
            leftFlipperRotation = std::min(leftFlipperRotation, maxLeftFlipperRotation);
            if (leftFlipperRotation >= maxLeftFlipperRotation || !glfwGetKey(window, GLFW_KEY_A))
                startRotationLeft = false;
        }
        else if (!glfwGetKey(window, GLFW_KEY_A))
        {
            leftFlipperRotation -= deltaRotation;
            leftFlipperRotation = std::max(leftFlipperRotation, -maxLeftFlipperRotation);
        }

        if (startRotationRight)
        {
            rightFlipperRotation -= deltaRotation;
            rightFlipperRotation = std::max(rightFlipperRotation, maxRightFlipperRotation);
            if (rightFlipperRotation <= maxRightFlipperRotation || !glfwGetKey(window, GLFW_KEY_L))
                startRotationRight = false;
        }
        else if (!glfwGetKey(window, GLFW_KEY_L))
        {
            rightFlipperRotation += deltaRotation;
            rightFlipperRotation = std::min(rightFlipperRotation, maxRightFlipperRotation + 60.0f);
        }
        
        leftFlipper.Rotation = glm::vec3(leftFlipperRotation, -3.24f, -5.64f);
        rightFlipper.Rotation = glm::vec3(rightFlipperRotation, -3.24f, -5.64f);
    }

    
    void updateBallPosition()
    {

        float dt = 0.3f;
        float tempDy;
        float accIntensity;
        glm::vec3 norm;
        glm::vec3 accVers;
        glm::vec3 reflected;
        glm::vec3 bounceAcc = glm::vec3(0.0f, 0.0f, 0.0f);

        //float z = -5.9728f;
        /*if(ball.AccelerationTot.x > 0)
            ball.AccelerationTot.x = std::min(ball.AccelerationTot.x, 1.0f);
        else
            ball.AccelerationTot.x = std::max(ball.AccelerationTot.x, -0.5f);
        if(ball.AccelerationTot.y > 0)
            ball.AccelerationTot.y = std::min(ball.AccelerationTot.y, 0.5f);
        else
            ball.AccelerationTot.y = std::max(ball.AccelerationTot.y, -0.5f);
        if(ball.AccelerationTot.z > 0)
            ball.AccelerationTot.z = std::min(ball.AccelerationTot.z, 0.5f);
        else
            ball.AccelerationTot.z = std::max(ball.AccelerationTot.z, -0.5f);*/

        dz = ball.Speed.z * dt + 0.5f *  (ball.AccelerationTot.z + ball.AccelerationGravity.z)* std::pow(dt, 2) 
                        -glm::sign(ball.Speed.z)*std::abs(0.7f)* std::pow(dt, 2);
        ball.Speed.z += 0.5f *  ball.AccelerationTot.z * dt;

        dy = ball.Speed.y * dt + 0.5 * (ball.AccelerationTot.z*sin(alfa)/cos(alfa) + ball.AccelerationGravity.y) * std::pow(dt, 2)
                     -glm::sign(ball.Speed.z)*std::abs(0.7f*tan(alfa))* std::pow(dt, 2);
        ball.Speed.y += 0.5 * ball.AccelerationTot.z*sin(alfa)/cos(alfa) * dt;

        dx = ball.Speed.x * dt + 0.5 * ball.AccelerationTot.x * std::pow(dt, 2) -glm::sign(ball.Speed.x)*std::abs(0.7f*ball.AccelerationTot.z/ball.AccelerationTot.x)* std::pow(dt, 2);
        ball.Speed.x += 0.5 * ball.AccelerationTot.x * dt;
        float r = std::abs(dx/dz);
        if(dx > 0)
            dx = std::min(dx, 0.007f);
        else
            dx = std::max(dx, -0.007f);
        if(dz > 0)
            dz = std::min(dz, 0.007f/r);
        else
            dz = std::max(dz, -0.007f/r);
        if(dy > 0)
            dy = std::min(dy, dz*sin(alfa)/cos(alfa));
        else
            dy = std::max(dy, dz*sin(alfa)/cos(alfa));
        
        
        ball.Position = glm::vec3(ball.Position.x + dx, std::max(ball.Position.y - dy, 8.4032f), std::max(ball.Position.z - dz, -8.0f));

        if(ball.Position.x >= 2.3465f){
            /*norm = glm::vec3(-1.0f, 0.0f, 0.0f);
            reflected = glm::reflect(accVers, norm);
            bounceAcc = reflected * accIntensity;
            ball.AccelerationTot.x = 0.0f; */
            ball.Position.x = 2.3465f;
            ball.AccelerationTot.x = - ball.AccelerationTot.x;
            ball.Speed.x = glm::sign(ball.AccelerationTot.x)*std::abs(ball.Speed.x);
            ball.Speed.y = glm::sign(ball.AccelerationTot.z)*std::abs(ball.Speed.y);
            ball.Speed.z = glm::sign(ball.AccelerationTot.z)*std::abs(ball.Speed.z);
            /*ball.Speed.x = 0.0f;
            ball.Speed.y = 0.0f;
            ball.Speed.z = 0.0f;*/
        }else if(ball.Position.x <= -2.7434f){
            /*norm = glm::vec3(1.0f, 0.0f, 0.0f);
            reflected = glm::reflect(accVers, norm);
            bounceAcc = reflected * accIntensity;
            ball.AccelerationTot.x = 0.0f;*/
            ball.Position.x = -2.7434f;
            ball.AccelerationTot.x = - ball.AccelerationTot.x;
            ball.Speed.x = glm::sign(ball.AccelerationTot.x)*std::abs(ball.Speed.x);
            ball.Speed.y = glm::sign(ball.AccelerationTot.z)*std::abs(ball.Speed.y);
            ball.Speed.z = glm::sign(ball.AccelerationTot.z)*std::abs(ball.Speed.z);
            /*ball.Speed.x = 0.0f;
            ball.Speed.y = 0.0f;
            ball.Speed.z = 0.0f;*/
        }else if(ball.Position.z >= 4.1008f){
            /*glm::mat4 rX = glm::rotate(glm::mat4(1.0f), alfa, glm::vec3(1.0f, 0.0f, 0.0f));
            norm = glm::vec3(rX*glm::vec4(glm::vec3(0.0f, 0.0f, -1.0f), 0.0f));
            norm = glm::normalize(norm);
            reflected = glm::reflect(accVers, norm);
            bounceAcc = reflected * accIntensity;*/
            tempDy = (4.1008f - ball.Position.z)*sin(alfa)/cos(alfa);
            ball.Position.z = 4.1008f;
            ball.Position.y = std::max(ball.Position.y - tempDy, 8.4032f);
            ball.AccelerationTot.z = - ball.AccelerationTot.z;
            ball.AccelerationTot.y = ball.AccelerationTot.z*sin(alfa)/cos(alfa);
            ball.Speed.x = glm::sign(ball.AccelerationTot.x)*std::abs(ball.Speed.x);
            ball.Speed.y = glm::sign(ball.AccelerationTot.y)*std::abs(ball.Speed.y);
            ball.Speed.z = glm::sign(ball.AccelerationTot.z)*std::abs(ball.Speed.z);
            /*ball.Speed.x = 0.0f;
            ball.Speed.y = 0.0f;
            ball.Speed.z = 0.0f;*/
        }else if(ball.Position.z <= -5.4828f){
            if((ball.Position.x >= -2.7434f && ball.Position.x <= -1.563f) ||
            (ball.Position.x <= 2.3465f && ball.Position.x >= 0.9115f)){
                /*glm::mat4 rX = glm::rotate(glm::mat4(1.0f), -alfa, glm::vec3(1.0f, 0.0f, 0.0f));
                norm = glm::vec3(rX*glm::vec4(glm::vec3(0.0f, 0.0f, 1.0f), 0.0f));
                norm = glm::normalize(norm);
                reflected = glm::reflect(accVers, norm);
                bounceAcc = reflected * accIntensity;*/
                tempDy = (-5.4828f - ball.Position.z)*sin(alfa)/cos(alfa);
                ball.Position.z = -5.4828f;
                ball.Position.y = std::max(ball.Position.y - tempDy, 8.4032f);
                ball.AccelerationTot.z = -ball.AccelerationTot.z;
                ball.AccelerationTot.y = ball.AccelerationTot.z*sin(alfa)/cos(alfa);
                ball.Speed.x = glm::sign(ball.AccelerationTot.x)*std::abs(ball.Speed.x);
                ball.Speed.y = glm::sign(ball.AccelerationTot.y)*std::abs(ball.Speed.y);
                ball.Speed.z = glm::sign(ball.AccelerationTot.z)*std::abs(ball.Speed.z);
                /*ball.Speed.x = 0.0f;
                ball.Speed.y = 0.0f;
                ball.Speed.z = 0.0f;*/
            }else if(ball.Position.z <= -7.0f){
                ball.inGame = false;
                ball.Position.x = ballStartx;
                ball.Position.y = ballStarty;
                ball.Position.z = ballStartz;
                ball.Speed = glm::vec3(0.0f);
                ball.AccelerationTot = glm::vec3(0.0f);
            }
        }


        //bumper controll
        //----------------------------------------------------------------------------

        bool checkLateral = true;
            //check if is hitting top (if is in the upper part of the bumper)
        if( ball.Position.z <= bumper1.Position.z + bumper1.CollisionBox.Size.z/2.0f && ball.Position.z >= bumper1.Position.z)
        {
            //check if is "inside" the area of one of the bumper 
            if(ball.Position.x <= bumper1.Position.x + bumper1.CollisionBox.Size.x/2.0f && ball.Position.x >= bumper1.Position.x - bumper1.CollisionBox.Size.x/2.0f
                || ball.Position.x <= bumper2.Position.x + bumper2.CollisionBox.Size.x/2.0f && ball.Position.x >= bumper2.Position.x - bumper2.CollisionBox.Size.x/2.0f 
                    || ball.Position.x <= bumper3.Position.x + bumper3.CollisionBox.Size.x/2.0f && ball.Position.x >= bumper3.Position.x - bumper3.CollisionBox.Size.x/2.0f){

                if(!((ball.Position.x <= bumper1.Position.x + bumper1.CollisionBox.Size.x/2.0f &&  ball.Position.x >= bumper1.Position.x + bumper1.CollisionBox.Size.x/2.0f - 0.1f) 
                        || (ball.Position.x <= bumper2.Position.x + bumper2.CollisionBox.Size.x/2.0f &&  ball.Position.x >= bumper2.Position.x + bumper2.CollisionBox.Size.x/2.0f -0.1f)
                        || (ball.Position.x <= bumper3.Position.x + bumper3.CollisionBox.Size.x/2.0f &&  ball.Position.x >= bumper3.Position.x + bumper3.CollisionBox.Size.x/2.0f -0.1f)
                        || (ball.Position.x >= bumper1.Position.x - bumper1.CollisionBox.Size.x/2.0f && ball.Position.x <= bumper1.Position.x - bumper1.CollisionBox.Size.x/2.0f + 0.1f)
                        || (ball.Position.x >= bumper2.Position.x - bumper2.CollisionBox.Size.x/2.0f && ball.Position.x <= bumper2.Position.x - bumper2.CollisionBox.Size.x/2.0f + 0.1f)
                        || (ball.Position.x >= bumper3.Position.x - bumper3.CollisionBox.Size.x/2.0f && ball.Position.x <= bumper3.Position.x - bumper3.CollisionBox.Size.x/2.0f + 0.1f))){
                                ball.Position.z = bumper1.Position.z + bumper1.CollisionBox.Size.z/2.0f;
                                ball.Position.y = bumper1.Position.y + bumper1.CollisionBox.Size.z/2.0f*sin(alfa)/cos(alfa);
                                ball.AccelerationTot.z = -ball.AccelerationTot.z;
                                ball.AccelerationTot.y = ball.AccelerationTot.z*sin(alfa)/cos(alfa); 
                                ball.Speed.x = glm::sign(ball.AccelerationTot.x)*std::abs(ball.Speed.x);
                                ball.Speed.y = glm::sign(ball.AccelerationTot.y)*std::abs(ball.Speed.y);
                                ball.Speed.z = glm::sign(ball.AccelerationTot.z)*std::abs(ball.Speed.z);
                                checkLateral = false;
                        }
                        
            }
            
                 //check if is hitting bottom 
        }
        
        else if(ball.Position.z >= bumper1.Position.z - bumper1.CollisionBox.Size.z/2.0f && ball.Position.z <= bumper1.Position.z)
        {
             //check if is "inside" the area of one of the bumper 

             if(ball.Position.x <= bumper1.Position.x + bumper1.CollisionBox.Size.x/2.0f && ball.Position.x >= bumper1.Position.x - bumper1.CollisionBox.Size.x/2.0f
                || ball.Position.x <= bumper2.Position.x + bumper2.CollisionBox.Size.x/2.0f && ball.Position.x >= bumper2.Position.x - bumper2.CollisionBox.Size.x/2.0f 
                    || ball.Position.x <= bumper3.Position.x + bumper3.CollisionBox.Size.x/2.0f && ball.Position.x >= bumper3.Position.x - bumper3.CollisionBox.Size.x/2.0f){

                if(!((ball.Position.x <= bumper1.Position.x + bumper1.CollisionBox.Size.x/2.0f &&  ball.Position.x >= bumper1.Position.x + bumper1.CollisionBox.Size.x/2.0f - 0.1f) 
                        || (ball.Position.x <= bumper2.Position.x + bumper2.CollisionBox.Size.x/2.0f &&  ball.Position.x >= bumper2.Position.x + bumper2.CollisionBox.Size.x/2.0f -0.1f)
                        || (ball.Position.x <= bumper3.Position.x + bumper3.CollisionBox.Size.x/2.0f &&  ball.Position.x >= bumper3.Position.x + bumper3.CollisionBox.Size.x/2.0f -0.1f)
                        || (ball.Position.x >= bumper1.Position.x - bumper1.CollisionBox.Size.x/2.0f && ball.Position.x <= bumper1.Position.x - bumper1.CollisionBox.Size.x/2.0f + 0.1f)
                        || (ball.Position.x >= bumper2.Position.x - bumper2.CollisionBox.Size.x/2.0f && ball.Position.x <= bumper2.Position.x - bumper2.CollisionBox.Size.x/2.0f + 0.1f)
                        || (ball.Position.x >= bumper3.Position.x - bumper3.CollisionBox.Size.x/2.0f && ball.Position.x <= bumper3.Position.x - bumper3.CollisionBox.Size.x/2.0f + 0.1f))){           
                                ball.Position.z = bumper1.Position.z - bumper1.CollisionBox.Size.z/2.0f;
                                ball.Position.y = bumper1.Position.y - bumper1.CollisionBox.Size.z/2.0f*sin(alfa)/cos(alfa);
                                ball.AccelerationTot.z = -ball.AccelerationTot.z;
                                ball.AccelerationTot.y = ball.AccelerationTot.z*sin(alfa)/cos(alfa); 
                                ball.Speed.x = glm::sign(ball.AccelerationTot.x)*std::abs(ball.Speed.x);
                                ball.Speed.y = glm::sign(ball.AccelerationTot.y)*std::abs(ball.Speed.y);
                                ball.Speed.z = glm::sign(ball.AccelerationTot.z)*std::abs(ball.Speed.z);
                                checkLateral = false;
                }
                        
            }

        }
        
        if(checkLateral && ball.Position.z >= bumper1.Position.z - bumper1.CollisionBox.Size.z/2.0f && ball.Position.z <= bumper1.Position.z + bumper1.CollisionBox.Size.z/2.0f)
        {
            // -----X----->
            if(ball.Position.x <= bumper1.Position.x + bumper1.CollisionBox.Size.x/2.0f &&  ball.Position.x >= bumper1.Position.x)
            {
                ball.Position.x = bumper1.Position.x + bumper1.CollisionBox.Size.x/2.0f;
                ball.AccelerationTot.x = - ball.AccelerationTot.x;
                ball.Speed.x = glm::sign(ball.AccelerationTot.x)*std::abs(ball.Speed.x);
                ball.Speed.y = glm::sign(ball.AccelerationTot.y)*std::abs(ball.Speed.y);
                ball.Speed.z = glm::sign(ball.AccelerationTot.z)*std::abs(ball.Speed.z);

            }else if(ball.Position.x <= bumper2.Position.x + bumper2.CollisionBox.Size.x/2.0f &&  ball.Position.x >= bumper2.Position.x)
            {
                ball.Position.x = bumper2.Position.x + bumper2.CollisionBox.Size.x/2.0f;
                ball.AccelerationTot.x = - ball.AccelerationTot.x;
                ball.Speed.x = glm::sign(ball.AccelerationTot.x)*std::abs(ball.Speed.x);
                ball.Speed.y = glm::sign(ball.AccelerationTot.y)*std::abs(ball.Speed.y);
                ball.Speed.z = glm::sign(ball.AccelerationTot.z)*std::abs(ball.Speed.z);

            }else if(ball.Position.x <= bumper3.Position.x + bumper3.CollisionBox.Size.x/2.0f &&  ball.Position.x >= bumper3.Position.x)
            {

                ball.Position.x = bumper3.Position.x + bumper3.CollisionBox.Size.x/2.0f;
                ball.AccelerationTot.x = - ball.AccelerationTot.x;
                ball.Speed.x = glm::sign(ball.AccelerationTot.x)*std::abs(ball.Speed.x);
                ball.Speed.y = glm::sign(ball.AccelerationTot.y)*std::abs(ball.Speed.y);
                ball.Speed.z = glm::sign(ball.AccelerationTot.z)*std::abs(ball.Speed.z);

            }else if(ball.Position.x >= bumper1.Position.x - bumper1.CollisionBox.Size.x/2.0f && ball.Position.x <= bumper1.Position.x )
            {

                ball.Position.x = bumper1.Position.x - bumper1.CollisionBox.Size.x/2.0f;
                ball.AccelerationTot.x = - ball.AccelerationTot.x;
                ball.Speed.x = glm::sign(ball.AccelerationTot.x)*std::abs(ball.Speed.x);
                ball.Speed.y = glm::sign(ball.AccelerationTot.y)*std::abs(ball.Speed.y);
                ball.Speed.z = glm::sign(ball.AccelerationTot.z)*std::abs(ball.Speed.z);

            }else if(ball.Position.x >= bumper2.Position.x - bumper2.CollisionBox.Size.x/2.0f && ball.Position.x <= bumper2.Position.x)
            {

                ball.Position.x = bumper2.Position.x - bumper2.CollisionBox.Size.x/2.0f;
                ball.AccelerationTot.x = - ball.AccelerationTot.x;
                ball.Speed.x = glm::sign(ball.AccelerationTot.x)*std::abs(ball.Speed.x);
                ball.Speed.y = glm::sign(ball.AccelerationTot.y)*std::abs(ball.Speed.y);
                ball.Speed.z = glm::sign(ball.AccelerationTot.z)*std::abs(ball.Speed.z);

            }else if(ball.Position.x >= bumper3.Position.x - bumper3.CollisionBox.Size.x/2.0f && ball.Position.x <= bumper3.Position.x)
            {   

                ball.Position.x = bumper3.Position.x - bumper3.CollisionBox.Size.x/2.0f;
                ball.AccelerationTot.x = - ball.AccelerationTot.x;
                ball.Speed.x = glm::sign(ball.AccelerationTot.x)*std::abs(ball.Speed.x);
                ball.Speed.y = glm::sign(ball.AccelerationTot.y)*std::abs(ball.Speed.y);
                ball.Speed.z = glm::sign(ball.AccelerationTot.z)*std::abs(ball.Speed.z);

            }
        }

        
        ball.bounce(leftFlipper);
        ball.bounce(rightFlipper);
        //if(ball.bounce(bumper1)){
         //   printf("Dolore");
       // }
        //ball.bounce(bumper2);
        //ball.bounce(bumper3);
        
    }

};

// This is the main: probably you do not need to touch this!
int main()
{
    MyProject app;

    try
    {
        app.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}


