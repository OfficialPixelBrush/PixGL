#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include "structs.h"
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "shader.h"

int windowWidth = 800;
int windowHeight = 600;

float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

/*
glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f), 
    glm::vec3( 2.0f,  5.0f, -15.0f), 
    glm::vec3(-1.5f, -2.2f, -2.5f),  
    glm::vec3(-3.8f, -2.0f, -12.3f),  
    glm::vec3( 2.4f, -0.4f, -3.5f),  
    glm::vec3(-1.7f,  3.0f, -7.5f),  
    glm::vec3( 1.3f, -2.0f, -2.5f),  
    glm::vec3( 1.5f,  2.0f, -2.5f), 
    glm::vec3( 1.5f,  0.2f, -1.5f), 
    glm::vec3(-1.3f,  1.0f, -1.5f)  
};
*/

std::vector<Int3> lights;
std::vector<Cube> cubes;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
}

Int3 MinInt3(Int3 a, Int3 b) {
    return Int3{std::min(a.x,b.x),std::min(a.y,b.y),std::min(a.z,b.z)};
}

Int3 MaxInt3(Int3 a, Int3 b) {
    return Int3{std::max(a.x,b.x),std::max(a.y,b.y),std::max(a.z,b.z)};
}

bool CheckIfInsideCube(Int3 pos) {
    for (auto& c : cubes) {
        if (!c.occluder) { continue; }
        Int3 minCorner = MinInt3(c.cornerA, c.cornerB);
        Int3 maxCorner = MaxInt3(c.cornerA, c.cornerB);

        if (pos.x >= minCorner.x && pos.x <= maxCorner.x &&
            pos.y >= minCorner.y && pos.y <= maxCorner.y &&
            pos.z >= minCorner.z && pos.z <= maxCorner.z) {
            return true;
        }
    }
    return false;
}

void loadTexture(std::string path, unsigned int& texture) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Disable usage of Mipmaps. We don't need them.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    int width, height, nrChannels;
    // Texture 2
    unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        //std::cout << nrChannels << std::endl;
        int inputFormat = GL_RGB;
        if (nrChannels != 3) {
            switch(nrChannels) {
                case 1:
                    inputFormat = GL_RED;
                    break;
                case 4:
                    inputFormat = GL_RGBA;
                    break;
                default:
                    inputFormat = GL_RGB;
                    break;
            }
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, inputFormat, GL_UNSIGNED_BYTE, data);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}

float getDistance2D(int x0,int y0,int x1,int y1) {
    return sqrt(pow(x1-x0,2)+pow(y1-y0,2));
}

int main(int argc, char *argv[])
{
    // Lights
    lights.push_back(Int3{ 16,0,38});
    //lights.push_back(Int3{ 63,0,5});

    // Cubes
    cubes.push_back(Cube{Int3{0,0,64},Int3{64,0,0},false});
    cubes.push_back(Cube{Int3{0,0,0},Int3{10,10,10}});
    cubes.push_back(Cube{Int3{30,0,10},Int3{50,5,20}});
    cubes.push_back(Cube{Int3{50,0,20},Int3{60,20,30}});
    cubes.push_back(Cube{Int3{22,0,40},Int3{24,10,42}});

    stbi_set_flip_vertically_on_load(true); 

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "PixGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glEnable(GL_DEPTH_TEST);
    
    Shader ourShader("shaders/shader.vs", "shaders/shader.fs");

    // VBO
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    float newVerts[6*6*5*cubes.size()];
    for(unsigned int i = 0; i < cubes.size(); i++) {
        for (unsigned int v = 0; v < 6*6*5; v++) {
            switch (v % 5) {
                case 0: // x
                    if (vertices[v] < -0.1f)
                        newVerts[v + i*(6*6*5)] = (float)cubes[i].cornerA.x;
                    else if (vertices[v] > 0.1f)
                        newVerts[v + i*(6*6*5)] = (float)cubes[i].cornerB.x;
                    else
                        newVerts[v + i*(6*6*5)] = vertices[v];
                    break;

                case 1: // y
                    if (vertices[v] < -0.1f)
                        newVerts[v + i*(6*6*5)] = (float)cubes[i].cornerA.y;
                    else if (vertices[v] > 0.1f)
                        newVerts[v + i*(6*6*5)] = (float)cubes[i].cornerB.y;
                    else
                        newVerts[v + i*(6*6*5)] = vertices[v];
                    break;

                case 2: // z
                    if (vertices[v] < -0.1f)
                        newVerts[v + i*(6*6*5)] = (float)cubes[i].cornerA.z;
                    else if (vertices[v] > 0.1f)
                        newVerts[v + i*(6*6*5)] = (float)cubes[i].cornerB.z;
                    else
                        newVerts[v + i*(6*6*5)] = vertices[v];
                    break;

                default: // texcoords or other
                    newVerts[v + i*(6*6*5)] = vertices[v];
                    break;
            }
        }
    }
    glBufferData(GL_ARRAY_BUFFER, sizeof(newVerts), newVerts, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // load and create a texture 
    // -------------------------
    unsigned int baseTexture, lightMap;
    loadTexture("textures/tile.png",baseTexture);
    glGenTextures(1, &lightMap);
    glBindTexture(GL_TEXTURE_2D, lightMap); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Disable usage of Mipmaps. We don't need them.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    int lightMapScale = 64;
    std::vector<float> data(lightMapScale*lightMapScale);
    int maxSteps = 256;
    for (int y = 0; y < lightMapScale; y++) {
        for (int x = 0; x < lightMapScale; x++) {
            float currentLightValue = 0.0;
            for (auto l : lights) {
                for (int aa = 0; aa < 4; aa++) {
                    float nudgeX = 0.5;
                    float nudgeY = 0.5;
                    switch (aa) {
                        case 0:
                            break;
                        case 1:
                            nudgeX *= -1.0;
                            break;
                        case 2:
                            nudgeY *= -1.0;
                            break;
                        case 3:
                            nudgeX *= -1.0;
                            nudgeY *= -1.0;
                            break;
                    }
                    float dx = l.x - x + nudgeX;
                    float dy = l.z - y + nudgeY;
                    float distance = std::sqrt(dx * dx + dy * dy);
            
                    if (distance == 0) {
                        data[x + y * lightMapScale] = 1.0f;
                        continue;
                    }
            
                    float stepX = dx / distance;
                    float stepY = dy / distance;
            
                    float currentX = x + 0.5f;
                    float currentY = y + 0.5f;
                    bool blocked = false;
            
                    for (int step = 0; step < std::min((int)distance, maxSteps); step++) {
                        int mapX = (int)currentX;
                        int mapY = (int)currentY;
                        
                        if (CheckIfInsideCube(Int3{mapX,0,mapY})) {
                            blocked = true;
                            break;
                        }
            
                        currentX += stepX;
                        currentY += stepY;
                    }
            
                    if (!blocked) {
                        currentLightValue += 1.0f - getDistance2D(x, y, l.x, l.z) * 0.02f;
                    }
                }
                // Divided by 4 to account for 4 AA samples
                data[x + y * lightMapScale] = currentLightValue/4.0;
            }
        }
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, lightMapScale, lightMapScale, 0, GL_RED, GL_FLOAT, data.data());
        
    // bind Texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, baseTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, lightMap);

    ourShader.use();
    ourShader.setInt("BaseTexture", 0); // or with shader class
    ourShader.setFloat("TextureScale", 16.0); // or with shader class
    ourShader.setInt("LightMap", 1); // or with shader class

    glm::mat4 model = glm::mat4(1.0f);
    //model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
    //model = glm::scale(model, glm::vec3(0.2, 0.2, 0.2));
    glm::mat4 view = glm::mat4(1.0f);
    //view = glm::translate(view, glm::vec3(0.0f, 0.0f, 0.0f));
    
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)windowWidth/(float)windowHeight, 0.1f, 200.0f);
    
    ourShader.setMat4("model",model);
    ourShader.setMat4("view",view);
    ourShader.setMat4("projection",proj);

    while(!glfwWindowShouldClose(window))
    {
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render container
        ourShader.use();

        const float radius = 60.0f;
        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;
        glm::mat4 view;
        view = glm::lookAt(glm::vec3(32.0 + camX, 16.0, 32.0 + camZ), glm::vec3(32.0, 0.0, 32.0), glm::vec3(0.0, 1.0, 0.0));  

        ourShader.setMat4("view",view);
        
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36*cubes.size());

        // swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}