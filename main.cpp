// ItemBlock.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define STB_IMAGE_IMPLEMENTATION 
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

//#include <assimp/Importer.hpp>

#include "CommonValues.h"

#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
#include "Model.h"

//using namespace std;
// Window dimensions
const GLint WIDTH = 800, HEIGHT = 600;
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;

Material shinyMaterial;
Material dullMaterial;

Model xwing;
Model blackhawk;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Vertex shader
static const char* vShader = "Shaders/shader.vert";

// Fragment shader
static const char* fShader = "Shaders/shader.frag";

void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
    unsigned int vLength, unsigned int normalOffset)
{
    for (size_t i = 0; i < indiceCount; i += 3)
    {
        unsigned int in0 = indices[i] * vLength;
        unsigned int in1 = indices[i + 1] * vLength;
        unsigned int in2 = indices[i + 2] * vLength;
        glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
        glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
        glm::vec3 normal = glm::cross(v1, v2);
        normal = glm::normalize(normal);

        in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
        vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
        vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
        vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
    }

    for (size_t i = 0; i < verticeCount / vLength; i++)
    {
        unsigned int nOffset = i * vLength + normalOffset;
        glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
        vec = glm::normalize(vec);
        vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
    }
}

void CreateObjects() {

    /* 

    GLfloat vertices[] = {
        // x     y      z        u	  v			                 nx	  ny    nz
        -1.0f, -1.0f, -1.0f,   0.0f, 0.0f,		                0.0f, 0.0f, 0.0f, //0
        -1.0f, 1.0f, -1.0f,    0.0f, 1.0f,		                0.0f, 0.0f, 0.0f, //1
        1.0f, 1.0f, -1.0f,     1.0f, 1.0f,		                0.0f, 0.0f, 0.0f, //2
        1.0f, -1.0f, -1.0f,    0.0f, 0.0f,		                0.0f, 0.0f, 0.0f, //3
        -1.0f, -1.0f, 1.0f,    0.0f, 0.0f,		                0.0f, 0.0f, 0.0f, //4
        -1.0f, 1.0f, 1.0f,     0.0f, 0.0f,		                0.0f, 0.0f, 0.0f, //5
        1.0f, 1.0f, 1.0f,      0.0f, 0.0f,		                0.0f, 0.0f, 0.0f, //6
        1.0f, -1.0f, 1.0f,      0.0f, 0.0f,		                0.0f, 0.0f, 0.0f //7
    }; */

    unsigned int indices[] = {
        0, 1, 2,
        3, 4, 5,
        6, 7, 8,
        9, 10, 11,
        12, 13, 14,
        15, 16, 17,
        18, 19, 20,
        21, 22, 23,
        24, 25, 26,
        27, 28, 29,
        30, 31, 32,
        33, 34, 35
    };

    GLfloat vertices[] = {
        // x     y      z        u	  v			                 nx	  ny    nz
        
        // triangle (0-1-2)
        -1.0f, -1.0f, -1.0f,    0.0f, 0.0f,		                0.0f, 0.0f, 0.0f, //0
        -1.0f, 1.0f, -1.0f,     1.0f, 0.0f,		                0.0f, 0.0f, 0.0f, //1
        1.0f, 1.0f, -1.0f,      1.0f, 1.0f,		                0.0f, 0.0f, 0.0f, //2

        // triangle (0-3-2)
        -1.0f, -1.0f, -1.0f,    0.0f, 0.0f,		                0.0f, 0.0f, 0.0f, //0
        1.0f, -1.0f, -1.0f,     0.0f, 1.0f,		                0.0f, 0.0f, 0.0f, //3
        1.0f, 1.0f, -1.0f,      1.0f, 1.0f,		                0.0f, 0.0f, 0.0f, //2

        // triangle (1-5-4)
        -1.0f, 1.0f, -1.0f,     1.0f, 1.0f,		                0.0f, 0.0f, 0.0f, //1
        -1.0f, 1.0f, 1.0f,      1.0f, 0.0f,		                0.0f, 0.0f, 0.0f, //5
        -1.0f, -1.0f, 1.0f,     0.0f, 0.0f,		                0.0f, 0.0f, 0.0f, //4

        // triangle (1-0-4)
        -1.0f, 1.0f, -1.0f,     1.0f, 1.0f,		                0.0f, 0.0f, 0.0f, //1
        -1.0f, -1.0f, -1.0f,    0.0f, 1.0f,		                0.0f, 0.0f, 0.0f, //0
        -1.0f, -1.0f, 1.0f,     0.0f, 0.0f,		                0.0f, 0.0f, 0.0f, //4

        // triangle (2-6-5)
        1.0f, 1.0f, -1.0f,      1.0f, 1.0f,		                0.0f, 0.0f, 0.0f, //2
        1.0f, 1.0f, 1.0f,       1.0f, 0.0f,		                0.0f, 0.0f, 0.0f, //6
        -1.0f, 1.0f, 1.0f,      0.0f, 0.0f,		                0.0f, 0.0f, 0.0f, //5

        // triangle (2-1-5)
        1.0f, 1.0f, -1.0f,      1.0f, 1.0f,		                0.0f, 0.0f, 0.0f, //2
        -1.0f, 1.0f, -1.0f,     0.0f, 1.0f,		                0.0f, 0.0f, 0.0f, //1
        -1.0f, 1.0f, 1.0f,      0.0f, 0.0f,		                0.0f, 0.0f, 0.0f, //5

        // triangle (3-7-6)
        1.0f, -1.0f, -1.0f,     1.0f, 1.0f,		                0.0f, 0.0f, 0.0f, //3
        1.0f, -1.0f, 1.0f,      1.0f, 0.0f,		                0.0f, 0.0f, 0.0f, //7
        1.0f, 1.0f, 1.0f,       0.0f, 0.0f,		                0.0f, 0.0f, 0.0f, //6

        // triangle (3-2-6)
        1.0f, -1.0f, -1.0f,     1.0f, 1.0f,		                0.0f, 0.0f, 0.0f, //3
        1.0f, 1.0f, -1.0f,      0.0f, 1.0f,		                0.0f, 0.0f, 0.0f, //2
        1.0f, 1.0f, 1.0f,       0.0f, 0.0f,		                0.0f, 0.0f, 0.0f, //6

        // triangle (0-4-7)
        -1.0f, -1.0f, -1.0f,    1.0f, 1.0f,		                0.0f, 0.0f, 0.0f, //0
        -1.0f, -1.0f, 1.0f,     1.0f, 0.0f,		                0.0f, 0.0f, 0.0f, //4
        1.0f, -1.0f, 1.0f,      0.0f, 0.0f,		                0.0f, 0.0f, 0.0f, //7

        // triangle (0-3-7)
        -1.0f, -1.0f, -1.0f,    1.0f, 1.0f,		                0.0f, 0.0f, 0.0f, //0
        1.0f, -1.0f, -1.0f,     0.0f, 1.0f,		                0.0f, 0.0f, 0.0f, //3
        1.0f, -1.0f, 1.0f,      0.0f, 0.0f,		                0.0f, 0.0f, 0.0f, //7

        // triangle (4-5-6)
        -1.0f, -1.0f, 1.0f,     0.0f, 0.0f,		                0.0f, 0.0f, 0.0f, //4
        -1.0f, 1.0f, 1.0f,      0.0f, 1.0f,		                0.0f, 0.0f, 0.0f, //5
        1.0f, 1.0f, 1.0f,       1.0f, 1.0f,		                0.0f, 0.0f, 0.0f, //6

        // triangle (4-7-6)
        -1.0f, -1.0f, 1.0f,     0.0f, 0.0f,		                0.0f, 0.0f, 0.0f, //4
        1.0f, -1.0f, 1.0f,      1.0f, 0.0f,		                0.0f, 0.0f, 0.0f, //7
        1.0f, 1.0f, 1.0f,       1.0f, 1.0f,		                0.0f, 0.0f, 0.0f //6
    };
    
    calcAverageNormals(indices, 36, vertices, 288, 8, 5);

    unsigned int floorIndices[] = {
        0, 2, 1,
        1, 2, 3
    };

    GLfloat floorVertices[] = {
        -10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
        10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
        -10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
        10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
    };

    

    Mesh* obj1 = new Mesh();
    obj1->CreateMesh(vertices, indices, 288, 36);
    meshList.push_back(obj1);

    Mesh* obj2 = new Mesh();
    obj2->CreateMesh(vertices, indices, 288, 36);
    meshList.push_back(obj2);

    Mesh* obj3 = new Mesh();
    obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
    meshList.push_back(obj3);
}

void CreateShader() {
    Shader* shader1 = new Shader();
    shader1->CreateFromFiles(vShader, fShader);
    shaderList.push_back(*shader1);
}

int main() {
    mainWindow = Window(1000, 1000); // 1280, 1024 or 1024, 768
    mainWindow.Initialise();

    CreateObjects();
    CreateShader();

    camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 5.0f, 0.5f);

    brickTexture = Texture("Textures/marioblock.png");
    brickTexture.LoadTextureA();
    dirtTexture = Texture("Textures/dirt.png");
    dirtTexture.LoadTextureA();
    plainTexture = Texture("Textures/plain.png");
    plainTexture.LoadTextureA();

    shinyMaterial = Material(4.0f, 256);
    dullMaterial = Material(0.3f, 4);


    mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f, -1.0f);

    unsigned int pointLightCount = 0;
    pointLights[0] = PointLight(0.0f, 0.0f, 1.0f,
        0.0f, 0.1f,
        0.0f, 0.0f, 0.0f,
        0.3f, 0.2f, 0.1f);
    pointLightCount++;
    pointLights[1] = PointLight(0.0f, 1.0f, 0.0f,
        0.0f, 0.1f,
        -4.0f, 2.0f, 0.0f,
        0.3f, 0.1f, 0.1f);
    pointLightCount++;

    unsigned int spotLightCount = 0;
    spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
        0.0f, 2.0f,
        0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        20.0f);
    spotLightCount++;
    spotLights[1] = SpotLight(1.0f, 1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, -1.5f, 0.0f,
        -100.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        20.0f);
    spotLightCount++;

    GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
        uniformSpecularIntensity = 0, uniformShininess = 0;
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);

    while (!mainWindow.getShouldClose())
    {
        GLfloat now = glfwGetTime(); // SDL_GetPerformanceCounter();
        deltaTime = now - lastTime; // (now - lastTime)*1000/SDL_GetPerformanceFrequency();
        lastTime = now;

        // Get + Handle User Input
        glfwPollEvents();

        camera.keyControl(mainWindow.getsKeys(), deltaTime);
        camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

        // Clear the window
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaderList[0].UseShader();
        uniformModel = shaderList[0].GetModelLocation();
        uniformProjection = shaderList[0].GetProjectionLocation();
        uniformView = shaderList[0].GetViewLocation();
        uniformEyePosition = shaderList[0].GetEyePositionLocation();
        uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
        uniformShininess = shaderList[0].GetShininessLocation();

        glm::vec3 lowerLight = camera.getCameraPosition();
        lowerLight.y -= 0.3f;
        spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

        shaderList[0].SetDirectionalLight(&mainLight);
        shaderList[0].SetPointLights(pointLights, pointLightCount);
        shaderList[0].SetSpotLights(spotLights, spotLightCount);

        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
        glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

        glm::mat4 model(1.0f);

        stbi_set_flip_vertically_on_load(true);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
        model = glm::rotate(model, 90.0f * toRadians, glm::vec3(-1.0f, 0.0f, 0.0f)); // Rotation
        //model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        brickTexture.UseTexture();
        shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
        meshList[0]->RenderMesh();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 4.0f, -2.5f));
        //model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        dirtTexture.UseTexture();
        dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
        meshList[1]->RenderMesh();

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        dirtTexture.UseTexture();
        shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
        meshList[2]->RenderMesh();

        glUseProgram(0);

        mainWindow.swapBuffers();
    }
    return 0;
}
