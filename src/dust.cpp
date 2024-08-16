#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <dust.h>
#include <ds_vulkan.h>


Dust::Dust(const char *title_, int windowWidth_, int windowHeight_, bool fullscreen_) {
    windowWidth = windowWidth_;
    windowHeight = windowHeight_;

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(windowWidth, windowHeight, title_, nullptr, nullptr);

    // TODO: decide how to handle small allocations
    // for now we are using the standar new and delete
    vulkan = new VkContext(title_);
}

Dust::~Dust() {
    delete vulkan;
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