#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <dust.h>
#include <iostream>

using namespace ds;


Dust::Dust(const char *title_, int windowWidth_, int windowHeight_, bool fullscreen_) {
    windowWidth = windowWidth_;
    windowHeight = windowHeight_;

    glfwInit();
    window = glfwCreateWindow(windowWidth, windowHeight, title_, nullptr, nullptr);
    glfwMakeContextCurrent(window);
    gl3wInit();

    glEnable(GL_DEPTH_TEST);
}

Dust::~Dust() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

bool Dust::ShouldClose() {
    return glfwWindowShouldClose(window);
}

void Dust::BeingFrame() {
    glfwPollEvents();

}
void Dust::EndFrame() {
    glfwSwapBuffers(window);
}