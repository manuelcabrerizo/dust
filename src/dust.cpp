#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <dust.h>
#include <iostream>
#include <stb_image.h>

using namespace ds;

static Input gInput;
static Input gLastInput;

void MousePosCallback(GLFWwindow* window, double xpos, double ypos) {

}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if(button < 3) {
        gInput.mouseButtons[button] = action;
    }
}

void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(key < 256) {
        gInput.keyboardButtons[key] = action;
    }
}

bool Input::KeyDown(int key) {
    return gInput.keyboardButtons[key];
}

bool Input::KeyJustDown(int key) {
    return  gInput.keyboardButtons[key] && !gLastInput.keyboardButtons[key];
}

bool Input::KeyJustUp(int key) {
    return  !gInput.keyboardButtons[key] && gLastInput.keyboardButtons[key];
}

bool Input::MouseDown(int button) {
    return gInput.mouseButtons[button];
}

bool Input::MouseJustDown(int button) {
    return gInput.mouseButtons[button] && !gLastInput.mouseButtons[button];
}

bool Input::MouseJustUp(int button) {
    return !gInput.mouseButtons[button] && gLastInput.mouseButtons[button];
}


Dust::Dust(const char *title_, int windowWidth_, int windowHeight_, bool fullscreen_) {
    windowWidth = windowWidth_;
    windowHeight = windowHeight_;

    glfwInit();
    window = glfwCreateWindow(windowWidth, windowHeight, title_, nullptr, nullptr);
    glfwMakeContextCurrent(window);
    gl3wInit();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    stbi_set_flip_vertically_on_load(true);

    glfwSetCursorPosCallback(window, MousePosCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetKeyCallback(window, KeyboardCallback);
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
    gLastInput = gInput;
}
