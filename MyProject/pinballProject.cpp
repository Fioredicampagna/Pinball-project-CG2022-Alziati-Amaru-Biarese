// This has been adapted from the Vulkan tutorial

#include "MyProject.hpp"

const std::string MODEL_PATH = "models/PinballDark/";
const std::string TEXTURE_PATH = "textures/StarWarsPinball.png";

// The uniform buffer object used in this example
struct globalUniformBufferObject {
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

struct UniformBufferObject {
    alignas(16) glm::mat4 model;
};


// MAIN !
class MyProject : public BaseProject {
    protected:
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
        
        // 04: Puller Model
        Model M_Puller;
        DescriptorSet DS_Puller;
        
        // 05: Flipper Model and instances
        Model M_Flipper;
        DescriptorSet DS_LeftFlipper;
        DescriptorSet DS_RightFlipper;
        
    
    DescriptorSet DS_global;

    
    // Here you set the main application parameters
    void setWindowParameters() {
        // window size, titile and initial background
        windowWidth = 800;
        windowHeight = 600;
        windowTitle = "My Project";
        initialBackgroundColor = {1.0f, 1.0f, 1.0f, 1.0f};
        
        // Descriptor pool sizes
        uniformBlocksInPool = 6;
        texturesInPool = 5;
        setsInPool = 6;
    }
    
    // Here you load and setup all your Vulkan objects
    void localInit() {
        // Descriptor Layouts [what will be passed to the shaders]
        DSLobj.init(this, {
                    // this array contains the binding:
                    // first  element : the binding number
                    // second element : the time of element (buffer or texture)
                    // third  element : the pipeline stage where it will be used
                    {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
                    {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}
                  });

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
        DS_PinballBody.init(this, &DSLobj, {
        // the second parameter, is a pointer to the Uniform Set Layout of this set
        // the last parameter is an array, with one element per binding of the set.
        // first  elmenet : the binding number
        // second element : UNIFORM or TEXTURE (an enum) depending on the type
        // third  element : only for UNIFORMs, the size of the corresponding C++ object
        // fourth element : only for TEXTUREs, the pointer to the corresponding texture object
                    {0, UNIFORM, sizeof(UniformBufferObject), nullptr},
                    {1, TEXTURE, 0, &T_Pinball}
                });
        
        /*M_SlHandle.init(this, "models/SlotHandle.obj");
        T_SlHandle.init(this, "textures/SlotHandle.png");
        DS_SlHandle.init(this, &DSLobj, {
                    {0, UNIFORM, sizeof(UniformBufferObject), nullptr},
                    {1, TEXTURE, 0, &T_SlHandle}
                }); */


        M_Bumper.init(this, MODEL_PATH + "bumper1.obj");
        DS_Bumper1.init(this, &DSLobj, {
                    {0, UNIFORM, sizeof(UniformBufferObject), nullptr},
                    {1, TEXTURE, 0, &T_Pinball}
                });
        DS_Bumper2.init(this, &DSLobj, {
                    {0, UNIFORM, sizeof(UniformBufferObject), nullptr},
                    {1, TEXTURE, 0, &T_Pinball}
                });
        DS_Bumper3.init(this, &DSLobj, {
                    {0, UNIFORM, sizeof(UniformBufferObject), nullptr},
                    {1, TEXTURE, 0, &T_Pinball}
                });




        DS_global.init(this, &DSLglobal, {
                    {0, UNIFORM, sizeof(globalUniformBufferObject), nullptr}
                });
    }

    // Here you destroy all the objects you created!
    void localCleanup() {
        DS_PinballBody.cleanup();
        M_PinballBody.cleanup();
        
        DS_Bumper1.cleanup();
        DS_Bumper2.cleanup();
        DS_Bumper3.cleanup();
        M_Bumper.cleanup();

        /*
        DS_SlHandle.cleanup();
        T_SlHandle.cleanup();
        M_SlHandle.cleanup();
        
        
        T_SlWheel.cleanup();
         */

        T_Pinball.cleanup();
        DS_global.cleanup();

        P1.cleanup();
        DSLglobal.cleanup();
        DSLobj.cleanup();
    }
    
    // Here it is the creation of the command buffer:
    // You send to the GPU all the objects you want to draw,
    // with their buffers and textures
    void populateCommandBuffer(VkCommandBuffer commandBuffer, int currentImage) {
                
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                P1.graphicsPipeline);
        vkCmdBindDescriptorSets(commandBuffer,
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        P1.pipelineLayout, 0, 1, &DS_global.descriptorSets[currentImage],
                        0, nullptr);

                
        VkBuffer vertexBuffers[] = {M_PinballBody.vertexBuffer};
        // property .vertexBuffer of models, contains the VkBuffer handle to its vertex buffer
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        // property .indexBuffer of models, contains the VkBuffer handle to its index buffer
        vkCmdBindIndexBuffer(commandBuffer, M_PinballBody.indexBuffer, 0,
                                VK_INDEX_TYPE_UINT32);

        // property .pipelineLayout of a pipeline contains its layout.
        // property .descriptorSets of a descriptor set contains its elements.
        vkCmdBindDescriptorSets(commandBuffer,
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        P1.pipelineLayout, 1, 1, &DS_PinballBody.descriptorSets[currentImage],
                        0, nullptr);
                        
        // property .indices.size() of models, contains the number of triangles * 3 of the mesh.
        vkCmdDrawIndexed(commandBuffer,
                    static_cast<uint32_t>(M_PinballBody.indices.size()), 1, 0, 0, 0);

        /*
        VkBuffer vertexBuffers2[] = {M_SlHandle.vertexBuffer};
        VkDeviceSize offsets2[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers2, offsets2);
        vkCmdBindIndexBuffer(commandBuffer, M_SlHandle.indexBuffer, 0,
                                VK_INDEX_TYPE_UINT32);
        vkCmdBindDescriptorSets(commandBuffer,
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        P1.pipelineLayout, 1, 1, &DS_SlHandle.descriptorSets[currentImage],
                        0, nullptr);
        vkCmdDrawIndexed(commandBuffer,
                    static_cast<uint32_t>(M_SlHandle.indices.size()), 1, 0, 0, 0); */



        VkBuffer vertexBuffers3[] = {M_Bumper.vertexBuffer};
        VkDeviceSize offsets3[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers3, offsets3);
        vkCmdBindIndexBuffer(commandBuffer, M_Bumper.indexBuffer, 0,
                                VK_INDEX_TYPE_UINT32);
        vkCmdBindDescriptorSets(commandBuffer,
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        P1.pipelineLayout, 1, 1, &DS_Bumper1.descriptorSets[currentImage],
                        0, nullptr);
        vkCmdDrawIndexed(commandBuffer,
                    static_cast<uint32_t>(M_Bumper.indices.size()), 1, 0, 0, 0);

        vkCmdBindDescriptorSets(commandBuffer,
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        P1.pipelineLayout, 1, 1, &DS_Bumper2.descriptorSets[currentImage],
                        0, nullptr);
        vkCmdDrawIndexed(commandBuffer,
                    static_cast<uint32_t>(M_Bumper.indices.size()), 1, 0, 0, 0);

        vkCmdBindDescriptorSets(commandBuffer,
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        P1.pipelineLayout, 1, 1, &DS_Bumper3.descriptorSets[currentImage],
                        0, nullptr);
        vkCmdDrawIndexed(commandBuffer,
                    static_cast<uint32_t>(M_Bumper.indices.size()), 1, 0, 0, 0);
        
        

    }

    // Here is where you update the uniforms.
    // Very likely this will be where you will be writing the logic of your application.
    void updateUniformBuffer(uint32_t currentImage) {
        /* static auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>
                    (currentTime - startTime).count();
        static float lastTime = 0.0f;
        float deltaT = time - lastTime;

        static int state = 0;        // 0 - everything is still.
                                    // 3 - three wheels are turning
                                    // 2 - two wheels are turning
                                    // 1 - one wheels is turning
                                    
        static float debounce = time;
        static float ang1 = 0.0f;
        static float ang2 = 0.0f;
        static float ang3 = 0.0f;

        if(glfwGetKey(window, GLFW_KEY_SPACE)) {
            if(time - debounce > 0.33) {
                debounce = time;

                if(state == 0) {
                    state = 3;
                } else {
                    state --;
                }
            }
        }
        
        if(state == 3) {
            ang3 += deltaT;
        }
        if(state >= 2) {
            ang2 += deltaT;
        }
        if(state >= 1) {
            ang1 += deltaT;
        } */

        
                    
        globalUniformBufferObject gubo{};
        UniformBufferObject ubo{};
        
        void* data;

        gubo.view = glm::lookAt(glm::vec3(1.0f, 1.0f, 1.5f) * 12.0f,
                                glm::vec3(0.0f, 0.0f, 10.0f),
                                glm::vec3(0.0f, 0.0f, 10.0f));
        gubo.proj = glm::perspective(glm::radians(45.0f),
                        swapChainExtent.width / (float) swapChainExtent.height,
                        0.1f, 100.0f);
        gubo.proj[1][1] *= -1;

        vkMapMemory(device, DS_global.uniformBuffersMemory[0][currentImage], 0,
                            sizeof(gubo), 0, &data);
        memcpy(data, &gubo, sizeof(gubo));
        vkUnmapMemory(device, DS_global.uniformBuffersMemory[0][currentImage]);
        

        // For the Slot Body
        ubo.model = glm::rotate(glm::mat4(1.0f),
                                glm::radians(-45.0f),
                               glm::vec3(0.0f, 0.0f, 1.0f)) *
                               glm::rotate(glm::mat4(1.0f),
                                 glm::radians(90.0f),
                                glm::vec3(1.0f, 0.0f, 0.0f));
        
        glm::mat4 body_position = ubo.model;
        
        vkMapMemory(device, DS_PinballBody.uniformBuffersMemory[0][currentImage], 0,
                            sizeof(ubo), 0, &data);
        memcpy(data, &ubo, sizeof(ubo));
        vkUnmapMemory(device, DS_PinballBody.uniformBuffersMemory[0][currentImage]);

        /*
        // For the Slot Handle
        ubo.model = glm::translate(glm::mat4(1.0f),glm::vec3(0.3f,0.5f,-0.15f));
        vkMapMemory(device, DS_SlHandle.uniformBuffersMemory[0][currentImage], 0,
                            sizeof(ubo), 0, &data);
        memcpy(data, &ubo, sizeof(ubo));
        vkUnmapMemory(device, DS_SlHandle.uniformBuffersMemory[0][currentImage]);
        */
         
        // Three bumpers instancing
                
        // bumper 1
        ubo.model = MakeWorldMatrixEuler(glm::vec3(1.1819f, 9.1362f, 0.020626f), glm::vec3(-6.51f, 0.0f, 0.0f), glm::vec3(1.0f));
        
        vkMapMemory(device, DS_Bumper1.uniformBuffersMemory[0][currentImage], 0,
                            sizeof(ubo), 0, &data);
        memcpy(data, &ubo, sizeof(ubo));
        vkUnmapMemory(device, DS_Bumper1.uniformBuffersMemory[0][currentImage]);

        // bumper 2
        ubo.model =  MakeWorldMatrixEuler(glm::vec3(-1.5055f, 9.1362f, 0.020626f), glm::vec3(-6.51f, 0.0f, 0.0f), glm::vec3(1.0f));
        
        vkMapMemory(device, DS_Bumper2.uniformBuffersMemory[0][currentImage], 0,
                            sizeof(ubo), 0, &data);
        memcpy(data, &ubo, sizeof(ubo));
        vkUnmapMemory(device, DS_Bumper2.uniformBuffersMemory[0][currentImage]);

        // bumper 3
        ubo.model = MakeWorldMatrixEuler(glm::vec3(-0.11626f, 9.1362f, 0.020626f), glm::vec3(-6.51f, 0.0f, 0.0f), glm::vec3(1.0f)); 
        
        vkMapMemory(device, DS_Bumper3.uniformBuffersMemory[0][currentImage], 0,
                            sizeof(ubo), 0, &data);
        memcpy(data, &ubo, sizeof(ubo));
        vkUnmapMemory(device, DS_Bumper3.uniformBuffersMemory[0][currentImage]);
         
    }
    
    glm::mat4 MakeWorldMatrixEuler(glm::vec3 pos, glm::vec3 YPR, glm::vec3 size) {
        glm::mat4 out = glm::translate(glm::mat4(1.0), pos) * glm::rotate(glm::mat4(1.0), glm::radians(YPR.x), glm::vec3(0,1,0)) * glm::rotate(glm::mat4(1.0), glm::radians(YPR.y), glm::vec3(1,0,0)) * glm::rotate(glm::mat4(1.0), glm::radians(YPR.z), glm::vec3(0,0,1)) *  glm::scale(glm::mat4(1.0), size);
        return out;
    }
};

// This is the main: probably you do not need to touch this!
int main() {
    MyProject app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
