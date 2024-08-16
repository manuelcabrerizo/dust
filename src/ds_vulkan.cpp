//
// Created by manue on 8/16/2024.
//
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <ds_vulkan.h>
#include  <iostream>
#include <cstring>

VkContext::VkContext(const char *title) {
    CreateInstance(title);
}

VkContext::~VkContext() {
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

    GetPhysicalDevice();
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

void VkContext::GetPhysicalDevice() {
    unsigned int deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    VkPhysicalDevice *devices = new VkPhysicalDevice[deviceCount];
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices);

    // enumarate all the devices in the computer
    std::cout << deviceCount << " devices found!\n";
    for(int i = 0; i < deviceCount; i++) {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(devices[i], &deviceProperties);
        std::cout << i << ": " << deviceProperties.deviceName << "\n";
    }

    // pick the first device if any
    if(deviceCount > 0) {
        physicalDevice = devices[0];
    }

    delete devices;
}

QueueFamilyIndices VkContext::FindQueueFamilies() {
    QueueFamilyIndices indices;
    // TODO: ...
    return indices;
}




