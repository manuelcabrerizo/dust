#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <dust.h>
#include <iostream>

Dust::Dust(const char *title_, int windowWidth_, int windowHeight_, bool fullscreen_) {
    windowWidth = windowWidth_;
    windowHeight = windowHeight_;

    glfwInit();
    window = glfwCreateWindow(windowWidth, windowHeight, title_, nullptr, nullptr);
    glfwMakeContextCurrent(window);

    gl3wInit();
    if(gl3wIsSupported(4, 5)) {
        std::cout << "Opengl 4.5 supported\n";
    }
    else {
        throw std::runtime_error("Opengl 4.5 not supported");
    }


}

Dust::~Dust() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Dust::Run() {
    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        Update();
        Render();
    }
}