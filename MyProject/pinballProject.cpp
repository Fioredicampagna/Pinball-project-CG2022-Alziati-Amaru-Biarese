// This has been adapted from the Vulkan tutorial

#include "MyProject.hpp"

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

    float pullerState = 0.0f; // 0 stato a  riposo
                              // 1 tensione massima

    
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

        M_Puller.init(this, MODEL_PATH + "Puller.obj");
        DS_Puller.init(this, &DSLobj, {{0, UNIFORM, sizeof(UniformBufferObject), nullptr}, {1, TEXTURE, 0, &T_Pinball}});

        M_Flipper.init(this, MODEL_PATH + "RightFlipper.obj");
        DS_LeftFlipper.init(this, &DSLobj, {{0, UNIFORM, sizeof(UniformBufferObject), nullptr}, {1, TEXTURE, 0, &T_Pinball}});
        DS_RightFlipper.init(this, &DSLobj, {{0, UNIFORM, sizeof(UniformBufferObject), nullptr}, {1, TEXTURE, 0, &T_Pinball}});

        M_Button.init(this, MODEL_PATH + "RightButton.obj");
        DS_LeftButton.init(this, &DSLobj, {{0, UNIFORM, sizeof(UniformBufferObject), nullptr}, {1, TEXTURE, 0, &T_Pinball}});
        DS_RightButton.init(this, &DSLobj, {{0, UNIFORM, sizeof(UniformBufferObject), nullptr}, {1, TEXTURE, 0, &T_Pinball}});

        M_Ball.init(this, MODEL_PATH + "Ball.obj");
        DS_Ball.init(this, &DSLobj, {{0, UNIFORM, sizeof(UniformBufferObject), nullptr}, {1, TEXTURE, 0, &T_Pinball}});

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

       

        // glm::vec3 oldCameraPos = cameraPos;
        if (glfwGetKey(window, GLFW_KEY_LEFT))
        {
            //lookYaw += deltaT * ROT_SPEED;
            lookYaw += ROT_SPEED;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT))
        {
            //lookYaw -= deltaT * ROT_SPEED;
            lookYaw -= ROT_SPEED;
        }/*
        if (glfwGetKey(window, GLFW_KEY_UP))
        {
            //lookPitch += deltaT * ROT_SPEED;
            lookPitch += ROT_SPEED;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN))
        {
            //lookPitch -= deltaT * ROT_SPEED;
            lookPitch -= ROT_SPEED;
        }
        
        
        
        glm::vec3 RFDT = glm::vec3(glm::rotate(glm::mat4(1), lookYaw, glm::vec3(0, 1, 0)) *
                                   glm::vec4(FollowerDeltaTarget, 1.0f));*/
        
    
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

        // Pinball Body
        ubo.model = glm::mat4(1.0f);

        mapAndUnmap(currentImage, DS_PinballBody, data, ubo);

        // bumpers
        updateModel(currentImage, DS_Bumper1, data, ubo, 1.1819f, 9.1362f, 0.020626f, -6.51f, 0.0f, 0.0f);

        // bumper 2
        updateModel(currentImage, DS_Bumper2, data, ubo, -1.5055f, 9.1362f, 0.020626f, -6.51f, 0.0f, 0.0f);

        // bumper 3
        updateModel(currentImage, DS_Bumper3, data, ubo, -0.11626f, 9.1362f, 0.020626f, -6.51f, 0.0f, 0.0f);

        // puller
        updateModel(currentImage, DS_Puller, data, ubo, -2.5264f, 8.3925f, -7.5892f, 0.0f, -90.0f, 0.0f);

        // left flipper
        updateModel(currentImage, DS_LeftFlipper, data, ubo, 0.6906f, 8.4032f, -5.6357f, 29.8f, -3.24f, -5.64f);

        // right flipper
        updateModel(currentImage, DS_RightFlipper, data, ubo, -1.307f, 8.4032f, -5.6357f, 150.0f, -3.24f, -5.64f);

        // left button
        updateModel(currentImage, DS_LeftButton, data, ubo, 2.6175f, 8.7853f, -6.6902f, 0.0f, 0.0f, -90.0f);

        // right button
        updateModel(currentImage, DS_RightButton, data, ubo, -2.97f, 8.7853f, -6.6902f, 0.0f, 0.0f, 90.0f);

        // Score: 12 digits
        updateModel(currentImage, DS_DL1, data, ubo, 0.4366f, 12.789f, 4.1852f, 0.0f, -101.0f, 0.0f);
        updateModel(currentImage, DS_DL2, data, ubo, 0.713f, 12.789f, 4.1852f, 0.0f, -101.0f, 0.0f);
        updateModel(currentImage, DS_DL3, data, ubo, 0.9923f, 12.789f, 4.1852f, 0.0f, -101.0f, 0.0f);
        updateModel(currentImage, DS_DL4, data, ubo, 1.3917f, 12.789f, 4.1852f, 0.0f, -101.0f, 0.0f);
        updateModel(currentImage, DS_DL5, data, ubo, 1.6681f, 12.789f, 4.1852f, 0.0f, -101.0f, 0.0f);
        updateModel(currentImage, DS_DL6, data, ubo, 1.9474f, 12.789f, 4.1852f, 0.0f, -101.0f, 0.0f);
        updateModel(currentImage, DS_DR1, data, ubo, -2.8273f, 12.789f, 4.1852f, 0.0f, -101.0f, 0.0f);
        updateModel(currentImage, DS_DR2, data, ubo, -2.5509f, 12.789f, 4.1852f, 0.0f, -101.0f, 0.0f);
        updateModel(currentImage, DS_DR3, data, ubo, -2.2716f, 12.789f, 4.1852f, 0.0f, -101.0f, 0.0f);
        updateModel(currentImage, DS_DR4, data, ubo, -1.8722f, 12.789f, 4.1852f, 0.0f, -101.0f, 0.0f);
        updateModel(currentImage, DS_DR5, data, ubo, -1.5958f, 12.789f, 4.1852f, 0.0f, -101.0f, 0.0f);
        updateModel(currentImage, DS_DR6, data, ubo, -1.316f, 12.789f, 4.1852f, 0.0f, -101.0f, 0.0f);

        // ball
        updateModel(currentImage, DS_Ball, data, ubo, -0.30053f, 8.5335f, -5.9728f, 0.0f, 0.0f, 0.0f);

        // Logica del tiraggio del puller

        if (glfwGetKey(window, GLFW_KEY_SPACE))
        {
            pullerState += speed;
            pullerState = std::min(pullerState, 1.0f);
        }
        else
        {
            pullerState = 0;
        }
        updateModel(currentImage, DS_Puller, data, ubo, -2.5264f, 8.3925f, -7.5892f - maxPullerLenght * pullerState, 0.0f, -90.0f, 0.0f);
        
        
        
        updateCamera(deltaT);
        
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

    void updateModel(int currentImage, DescriptorSet DS, void *data, UniformBufferObject ubo, float x, float y, float z, float Rx, float Ry, float Rz)
    {
        ubo.model = MakeWorldMatrixEuler(glm::vec3(x, y, z), glm::vec3(Rx, Ry, Rz), glm::vec3(1.0f));

        mapAndUnmap(currentImage, DS, data, ubo);
    }
    
    void updateCamera(float deltaT){
        glm::vec3 FollowerTargetPos;
        
        glm::mat4 pinballWM = glm::rotate(glm::translate(glm::mat4(1), pinballPos),
                               lookYaw, glm::vec3(0,1,0));
        FollowerTargetPos = pinballWM * glm::translate(glm::mat4(1), FollowerDeltaTarget) *
                            glm::rotate(glm::mat4(1), lookPitch, glm::vec3(1,0,0)) *
                            glm::vec4(0.0f,0.0f,followerDist,1.0f);
        const float followerFilterCoeff = 7;
        float alpha = fmin(followerFilterCoeff * deltaT, 1.0f);
        cameraPos = cameraPos * (1.0f - alpha) + alpha * FollowerTargetPos;
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
