//
// Created by manue on 8/16/2024.
//
#define GLFW_INCLUDE_VULKAN
#include <cassert>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <ds_vulkan.h>
#include  <iostream>
#include <cstring>

VkContext::VkContext(const char *title, GLFWwindow *window) {
    CreateInstance(title);
    CreateSurface(window);
    GetPhysicalDevice();
    CreateLogicalDevice();
}

VkContext::~VkContext() {
    vkDestroyDevice(device, nullptr);
    std::cout << "the vulkan logical device was destroy!\n";
    vkDestroySurfaceKHR(instance, surface, nullptr);
    std::cout << "the vulkan surface was destroy!\n";
    vkDestroyInstance(instance, nullptr);
    std::cout << "the vulkan instance was destroy!\n";
}

void VkContext::CreateInstance(const char *title) {
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = title;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Dust engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    unsigned int glfwExtensionCount = 0;
    const char **glfwExtensions { nullptr };
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    // check for validation layer support
    if(enableValidationLayers && !CheckValidationLayerSupport()) {
        std::cout << "validation layers requested but not available!\n";
    }

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;
    createInfo.enabledLayerCount = 0;
    createInfo.enabledLayerCount = validationLayersCount;
    createInfo.ppEnabledLayerNames = validationLayers;

    if(vkCreateInstance(&createInfo, nullptr, &instance) == VK_SUCCESS) {
        std::cout << "the vulkan instance was created!\n";
    }
}

void VkContext::CreateSurface(GLFWwindow *window) {
    if(glfwCreateWindowSurface(instance, window, nullptr, &surface) == VK_SUCCESS) {
        std::cout << "the surface was created\n";
    }
}

void VkContext::GetPhysicalDevice() {
    unsigned int deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    VkPhysicalDevice *devices = new VkPhysicalDevice[deviceCount];
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices);

    // enumarate all the devices in the computer
    // and choose one
    for(int i = 0; i < deviceCount; i++) {
        QueueFamilyIndices indices = FindQueueFamilies(devices[i], surface);
        if(indices.IsComplete()) {
            physicalDevice = devices[i];
            std::cout << "the vulkan physical device was founded!\n";
            break;
        }
    }

    delete devices;
}

void VkContext::CreateLogicalDevice() {
    QueueFamilyIndices indices = FindQueueFamilies(physicalDevice, surface);

    assert(indices.graphicsFamily.value() == indices.presentFamily.value());

    float queuePriorities[1] = { 1.0f };
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = queuePriorities;

    VkPhysicalDeviceFeatures deviceFeatures[1] = {};

    VkDeviceCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pEnabledFeatures = deviceFeatures;
    createInfo.enabledExtensionCount = 0;

    if(enableValidationLayers) {
        createInfo.enabledLayerCount = validationLayersCount;
        createInfo.ppEnabledLayerNames = validationLayers;
    }
    else {
        createInfo.enabledLayerCount = 0;
    }

    if(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) == VK_SUCCESS) {
        std::cout << "the vulkan logical device was created!\n";
    }

    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

bool VkContext::CheckValidationLayerSupport() const {
    unsigned int layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    VkLayerProperties *availableLayers = new VkLayerProperties[layerCount];
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

    for(int i = 0; i < validationLayersCount; i++) {
        const char *layerName = validationLayers[i];
        bool layerFound = false;
        for(int j = 0; j < layerCount; j++) {
            VkLayerProperties *layerProperties = availableLayers + j;
            if(strcmp(layerName, layerProperties->layerName) == 0) {
                std::cout << layerName << ": found\n" ;
                layerFound = true;
                break;
            }
        }
        if(!layerFound) {
            std::cout << layerName << ": not found\n" ;
            delete availableLayers;
            return false;
        }
    }
    delete availableLayers;
    return true;
}

bool VkContext::CheckDeviceExtensionSupport() const {
    unsigned int layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    VkLayerProperties *availableLayers = new VkLayerProperties[layerCount];
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

    for(int i = 0; i < validationLayersCount; i++) {
        const char *layerName = validationLayers[i];
        bool layerFound = false;
        for(int j = 0; j < layerCount; j++) {
            VkLayerProperties *layerProperties = availableLayers + j;
            if(strcmp(layerName, layerProperties->layerName) == 0) {
                std::cout << layerName << ": found\n" ;
                layerFound = true;
                break;
            }
        }
        if(!layerFound) {
            std::cout << layerName << ": not found\n" ;
            delete availableLayers;
            return false;
        }
    }
    delete availableLayers;
    return true;
}

QueueFamilyIndices VkContext::FindQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    QueueFamilyIndices indices;

    unsigned int queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    VkQueueFamilyProperties *queueFamilies = new VkQueueFamilyProperties[queueFamilyCount];
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies);

    for(int i = 0; i < queueFamilyCount; i++) {
        if(queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
        if(presentSupport) {
            indices.presentFamily = i;
        }

        if(indices.IsComplete()) {
            break;
        }
    }

    delete queueFamilies;
    return indices;
}




