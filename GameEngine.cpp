#include "GameEngine.hpp"

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
using namespace glm;

#include "graphics/Shader.hpp"
#include "graphics/Texture.hpp"
#include "graphics/Mesh.hpp"
#include "graphics/VoxelRenderer.hpp"
#include "graphics/LineBatch.hpp"
#include "Window.hpp"
#include "Events.hpp"
#include "Camera.hpp"
#include "loaders/png_loading.hpp"
#include "voxel.hpp"
#include "Chunk.hpp"
#include "Chunks.hpp"
#include "files/files.hpp"
#include "drawMenu.hpp"

int WIDTH = 1280;
int HEIGHT = 720;

float vertices[] = {
        // x    y
       -0.01f,-0.01f,
        0.01f, 0.01f,

       -0.01f, 0.01f,
        0.01f,-0.01f,
};

int attrs[] = {2,  0};

const int gridSize = 32;

const int SCREEN_WIDTH = gridSize*32;
const int SCREEN_HEIGHT = gridSize*16;

static void inputK(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(window, GL_TRUE);
}

static void error_callback(int error, const char* description){
    fputs(description, stderr);
}


int startGame(bool isSaved, Chunks* chunks) {
    Window::initialize(WIDTH, HEIGHT, "Mincraft");
    Events::initialize();

    Shader* shader = load_shader("main.glslv", "main.glslf");
    if (shader == nullptr){
        std::cerr << "failed to load shader" << std::endl;
        Window::terminate();
        return 1;
    }

    Shader* crosshairShader = load_shader("crosshair.glslv", "crosshair.glslf");
    if (crosshairShader == nullptr){
        std::cerr << "failed to load crosshair shader" << std::endl;
        delete shader;
        Window::terminate();
        return 1;
    }
    
    Texture* texture = load_texture("block.png");
    if (texture == nullptr){
        std::cerr << "failed to load texture" << std::endl;
        delete shader;
        Window::terminate();
        return 1;
    }
    
    Mesh** meshes = new Mesh*[chunks->volume];
    for (size_t i = 0; i < chunks->volume; i++)
        meshes[i] = nullptr;
    VoxelRenderer renderer(1024*1024*8);


    glClearColor(0.6f,0.62f,0.65f,1);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Mesh* crosshair = new Mesh(vertices, 4, attrs);
    Camera* camera = new Camera(vec3(0,0,20), radians(90.0f));

    float lastTime = glfwGetTime();
    float delta = 0.0f;

    float camX = 0.0f;
    float camY = 0.0f;

    float speed = 50;
    int blockType = 1;
    bool shouldMenuOpen = false;
    while (!Window::isShouldClose()){
        float currentTime = glfwGetTime();
        delta = currentTime - lastTime;
        lastTime = currentTime;
        
        
        Chunk* closes[27];
        for (size_t i = 0; i < chunks->volume; i++){
            Chunk* chunk = chunks->chunks[i];
            if (!chunk->modified)
                continue;
            chunk->modified = false;
            if (meshes[i] != nullptr)
                delete meshes[i];

            for (int i = 0; i < 27; i++)
                closes[i] = nullptr;
            for (size_t j = 0; j < chunks->volume; j++){
                Chunk* other = chunks->chunks[j];

                int ox = other->x - chunk->x;
                int oy = other->y - chunk->y;
                int oz = other->z - chunk->z;

                if (abs(ox) > 1 || abs(oy) > 1 || abs(oz) > 1)
                    continue;

                ox += 1;
                oy += 1;
                oz += 1;
                closes[(oy * 3 + oz) * 3 + ox] = other;
            }
            Mesh* mesh = renderer.render(chunk, (const Chunk**)closes, true);
            meshes[i] = mesh;
        }
        
        

        if (Events::jpressed(GLFW_KEY_ESCAPE)){
            unsigned char* buffer = new unsigned char[chunks->volume * CHUNK_VOL];
            chunks->write(buffer);
            write_binary_file("world.bt", (const char*)buffer, chunks->volume * CHUNK_VOL);
            delete[] buffer;
            Window::setShouldClose(true);
            if(Events::_cursor_locked){
                Events::toogleCursor();
            }
            shouldMenuOpen = true;
        }
        if (Events::jpressed(GLFW_KEY_TAB)){
            Events::toogleCursor();
        }
       
        if (Events::pressed(GLFW_KEY_W)){
            camera->position += camera->front * delta * speed;
        }
        if (Events::pressed(GLFW_KEY_S)){
            camera->position -= camera->front * delta * speed;
        }
        if (Events::pressed(GLFW_KEY_D)){
            camera->position += camera->right * delta * speed;
        }
        if (Events::pressed(GLFW_KEY_A)){
            camera->position -= camera->right * delta * speed;
        }
        
        if (Events::pressed(GLFW_KEY_1)){
            blockType = 1;
        }
        if (Events::pressed(GLFW_KEY_2)){
            blockType = 2;
        }
        if (Events::pressed(GLFW_KEY_3)){
            blockType = 3;
        }
        if (Events::pressed(GLFW_KEY_4)){
            blockType = 4;
        }
        if (Events::pressed(GLFW_KEY_5)){
            blockType = 5;
        }
        if (Events::pressed(GLFW_KEY_6)){
            blockType = 6;
        }
        if (Events::pressed(GLFW_KEY_7)){
            blockType = 7;
        }
        if (Events::pressed(GLFW_KEY_8)){
            blockType = 8;
        }
        if (Events::pressed(GLFW_KEY_9)){
            blockType = 9;
        }
       
        

        if (Events::_cursor_locked){
            camY += -Events::deltaY / Window::height * 2;
            camX += -Events::deltaX / Window::height * 2;

            if (camY < -radians(89.0f)){
                camY = -radians(89.0f);
            }
            if (camY > radians(89.0f)){
                camY = radians(89.0f);
            }

            camera->rotation = mat4(1.0f);
            camera->rotate(camY, camX, 0);
        }

        {
            vec3 end;
            vec3 norm;
            vec3 iend;
            voxel* vox = chunks->rayCast(camera->position, camera->front, 10.0f, end, norm, iend);
            if (vox != nullptr){
                if (Events::jclicked(GLFW_MOUSE_BUTTON_1)){
                    chunks->set((int)(iend.x)+(int)(norm.x), (int)(iend.y)+(int)(norm.y), (int)(iend.z)+(int)(norm.z), blockType);
                }
                if (Events::jclicked(GLFW_MOUSE_BUTTON_2)){
                    chunks->set((int)iend.x, (int)iend.y, (int)iend.z, 0);
                }
            }
        }

       

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader->use();
        shader->uniformMatrix("projview", camera->getProjection()*camera->getView());
        texture->bind();
        mat4 model(1.0f);
        for (size_t i = 0; i < chunks->volume; i++){
            Chunk* chunk = chunks->chunks[i];
            Mesh* mesh = meshes[i];
            model = glm::translate(mat4(1.0f), vec3(chunk->x*CHUNK_W+0.5f, chunk->y*CHUNK_H+0.5f, chunk->z*CHUNK_D+0.5f));
            shader->uniformMatrix("model", model);
            mesh->draw(GL_TRIANGLES);
        }

        crosshairShader->use();
        crosshair->draw(GL_LINES);

        glLineWidth(2.0f);

        Window::swapBuffers();
        Events::pullEvents();
    }

    delete shader;
    delete texture;
    delete chunks;
    delete crosshair;
    delete crosshairShader;

    Window::terminate();
    
    if (shouldMenuOpen) {
        drawWindow menu;
        menu.loadUIData();
        menu.draw();
    }
    return 0;
}
