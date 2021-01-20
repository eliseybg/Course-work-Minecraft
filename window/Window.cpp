#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Window.hpp"

GLFWwindow* Window::window;
int Window::width = 0;
int Window::height = 0;

int Window::initialize(int width, int height, const char* title){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (window == nullptr){
        std::cerr << "Failed to create GLFW Window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK){
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    Window::width = width;
    Window::height = height;
    return 0;
}

void Window::setCursorMode(int mode){
    glfwSetInputMode(window, GLFW_CURSOR, mode);
}

void Window::terminate(){
    glfwTerminate();
}

bool Window::isShouldClose(){
    return glfwWindowShouldClose(window);
}

void Window::setShouldClose(bool flag){
    glfwSetWindowShouldClose(window, flag);
}

void Window::swapBuffers(){
    glfwSwapBuffers(window);
}
