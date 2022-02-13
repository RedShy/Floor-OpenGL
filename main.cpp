#define __USE_MINGW_ANSI_STDIO 0
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include "camera.h"
#include "Object.h"
#include "Model.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
bool collision(const glm::vec3& direction, const float deltaTime);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f, 0.5f, -1.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastFrame = 0.0f;

std::vector<Object*> objects;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Floor Homework", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    float vertices[] =
        {
            //floor
            //position          normal        texture coord  
            -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            -0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 20.0f,
            0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 20.0f, 0.0f,

            0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 20.0f,
            0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 20.0f, 20.0f,
            -0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 20.0f, 0.0f,
     
            //Pyramid
            -0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, -0.75f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.0f, 0.5f,  0.0f, 1.0f, 0.0f, 0.5f, 1.0f,

            -0.5f, 0.0f, 0.5f,  0.0f, 0.5f, 0.5f, 0.0f, 0.0f,
            0.0f, 0.5f, 0.0f,  0.0f, 0.5f, 0.5f, 1.0f, 0.0f,
            0.5f, 0.0f, 0.5f,  0.0f, 0.5f, 0.5f, 0.5f, 1.0f,

            -0.5f, 0.0f, 0.5f,  -0.5f, 0.25f, -0.25f, 0.0f, 0.0f,
            0.0f, 0.0f, -0.75f,  -0.5f, 0.25f, -0.25f, 1.0f, 0.0f,
            0.0f, 0.5f, 0.0f,  -0.5f, 0.25f, -0.25f, 0.5f, 1.0f,

            0.5f, 0.0f, 0.5f,  0.5f, 0.25f, -0.25f, 0.0f, 0.0f,
            0.0f, 0.0f, -0.75f,  0.5f, 0.25f, -0.25f, 1.0f, 0.0f,
            0.0f, 0.5f, 0.0f,  0.5f, 0.25f, -0.25f, 0.5f, 1.0f,

            //Cube
            -0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f,  0.0f,
             0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 1.0f,  0.0f,
             0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 1.0f,  1.0f,
             0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 1.0f,  1.0f,
            -0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f,  1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f,  0.0f,

            -0.5f, -0.5f,  0.5f, 0.0f,  0.0f,  1.0f, 0.0f,  0.0f,
             0.5f, -0.5f,  0.5f, 0.0f,  0.0f,  1.0f, 1.0f,  0.0f,
             0.5f,  0.5f,  0.5f, 0.0f,  0.0f,  1.0f, 1.0f,  1.0f,
             0.5f,  0.5f,  0.5f, 0.0f,  0.0f,  1.0f, 1.0f,  1.0f,
            -0.5f,  0.5f,  0.5f, 0.0f,  0.0f,  1.0f, 0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f, 0.0f,  0.0f,  1.0f, 0.0f,  0.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f,  1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f,  1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f,  0.0f,
 
             0.5f,  0.5f,  0.5f, 1.0f,  0.0f,  0.0f, 1.0f,  0.0f,
             0.5f,  0.5f, -0.5f, 1.0f,  0.0f,  0.0f, 1.0f,  1.0f,
             0.5f, -0.5f, -0.5f, 1.0f,  0.0f,  0.0f, 0.0f,  1.0f,
             0.5f, -0.5f, -0.5f, 1.0f,  0.0f,  0.0f, 0.0f,  1.0f,
             0.5f, -0.5f,  0.5f, 1.0f,  0.0f,  0.0f, 0.0f,  0.0f,
             0.5f,  0.5f,  0.5f, 1.0f,  0.0f,  0.0f, 1.0f,  0.0f,
 
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f, 0.0f,  1.0f,
             0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f, 1.0f,  1.0f,
             0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f, 1.0f,  0.0f,
             0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f, 1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f, 0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f, 0.0f,  1.0f,

            -0.5f,  0.5f, -0.5f, 0.0f,  1.0f,  0.0f, 0.0f,  1.0f,
             0.5f,  0.5f, -0.5f, 0.0f,  1.0f,  0.0f, 1.0f,  1.0f,
             0.5f,  0.5f,  0.5f, 0.0f,  1.0f,  0.0f, 1.0f,  0.0f,
             0.5f,  0.5f,  0.5f, 0.0f,  1.0f,  0.0f, 1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f, 0.0f,  1.0f,  0.0f, 0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, 0.0f,  1.0f,  0.0f, 0.0f,  1.0f
        };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*) (3 * sizeof(GLfloat))) ;
    glEnableVertexAttribArray(1);

    // Texture Coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*) (6 * sizeof(GLfloat))) ;
    glEnableVertexAttribArray(2);

    Shader objectShader("./shader/shader.vs", "./shader/shader.fs");

    // light properties
    objectShader.use();
    objectShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
    objectShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
    objectShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
    objectShader.setFloat("light.constant", 1.0f);
    objectShader.setFloat("light.linear", 0.01125f);
    objectShader.setFloat("light.quadratic", 0.04f);

    Shader lampShader("./shader/lamp.vs", "./shader/lamp.fs");

    Object floor (&objectShader, VAO, 0, 6);
    floor.loadTexture("./textures/floor.jpg");
    floor.setScale(glm::vec3(100.0f, 20.0f, 100.0f));

    Object pyramid(&objectShader, VAO, 6, 12);
    pyramid.loadTexture("./textures/floor6.jpg");
    pyramid.setPosition(glm::vec3(-0.0f, 0.0f, -7.0f));
    pyramid.setScale(glm::vec3(1.5f,3.0f,1.5f));
    pyramid.setCollisionVertex(glm::vec3(0.0f, 0.0f, -0.75f));
    objects.push_back(&pyramid);

    Object pyramid2(&objectShader, VAO, 6, 12);
    pyramid2.setPosition(glm::vec3(2.57f, 0.0f, -4.45f));
    pyramid2.setScale(glm::vec3(0.75f,2.0f,0.75f));
    pyramid2.loadTexture("./textures/floor11.jpg");
    pyramid2.setCollisionVertex(glm::vec3(0.0f, 0.0f, -0.75f));
    objects.push_back(&pyramid2);

    Object cube(&objectShader, VAO, 18, 36);
    cube.loadTexture("./textures/floor5.jpg");
    cube.setPosition(glm::vec3(-2.6f, 0.3f, -8.7f));
    cube.setScale(glm::vec3(0.6f));
    cube.setCollisionVertex(glm::vec3(-0.5f, -0.5f, -0.5f));
    objects.push_back(&cube);

    Object cube2(&objectShader, VAO, 18, 36);
    cube2.loadTexture("./textures/floor7.jpg");
    cube2.setPosition(glm::vec3(2.6f, 0.3f, -8.7f));
    cube2.setScale(glm::vec3(0.6f));
    cube2.setCollisionVertex(glm::vec3(-0.5f, -0.5f, -0.5f));
    objects.push_back(&cube2);

    Object sphere(&objectShader, "./model/sphere.obj");
    sphere.setPosition(glm::vec3(-2.0f, 1.0f, -5.0f));
    sphere.loadTexture("./textures/floor2.jpg");
    sphere.setScale(glm::vec3(0.5f));
    objects.push_back(&sphere);

    Object sphere2(&objectShader, "./model/sphere.obj");
    sphere2.setPosition(glm::vec3(2.6f, 1.0f, -8.6f));
    sphere2.loadTexture("./textures/ball4.jpg");
    sphere2.setScale(glm::vec3(0.2f));
    sphere2.setRotation(-20.0f,glm::vec3(0.0f,1.0f,0.0f));
    objects.push_back(&sphere2);

    Object light(&lampShader, VAO, 18, 36);
    light.setPosition(glm::vec3(0.0f, 2.0f, -7.0f));
    light.setScale(glm::vec3(0.2f));

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        //lighting
        objectShader.use();
        objectShader.setVec3("light.position", light.getPosition());
        objectShader.setVec3("viewPos", camera.Position);

        objectShader.setMat4("view",view); 
        objectShader.setMat4("projection",projection);

        lampShader.use();
        lampShader.setMat4("projection", projection);
        lampShader.setMat4("view", view);

        floor.draw();
        for(int i=0; i<objects.size(); i++)
        {
            objects[i]->draw();
        }
        
        light.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}


void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && !collision(camera.Front,deltaTime))
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && !collision(camera.Front,-deltaTime))
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && !collision(camera.Right,-deltaTime))
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && !collision(camera.Right,deltaTime))
        camera.ProcessKeyboard(RIGHT, deltaTime);
}


void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

bool collision(const glm::vec3& directionMovement, const float deltaTime)
{
    //Try the new position, if is in collision with any object don't move
    glm::vec3 newPosition=camera.Position;

    const float velocity=camera.MovementSpeed * deltaTime;

    newPosition += directionMovement * velocity;

    for(int i=0; i<objects.size(); ++i)
    {
        if(objects[i]->isInCollision(newPosition))
        {
            return true;
        }
    }

    return false;
}
