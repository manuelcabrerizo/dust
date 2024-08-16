//
// Created by manue on 8/16/2024.
//

#ifndef DS_VULKAN_H
#define DS_VULKAN_H

#include <optional>

class QueueFamilyIndices {
public:
    std::optional<unsigned int> graphicsFamily;
    std::optional<unsigned int> presentFamily;

    bool IsComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

class VkContext {
public:
    explicit VkContext(const char *title, GLFWwindow *window);
    ~VkContext();

private:
    VkInstance instance{};
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice { VK_NULL_HANDLE };
    VkDevice device;

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    // validation layers
    static constexpr bool enableValidationLayers { true };
    static constexpr int validationLayersCount { 1 };
    const char *validationLayers[validationLayersCount] {
        "VK_LAYER_KHRONOS_validation",
    };
    static constexpr int deviceExtensionsCount { 1 };
    const char *deviceExtensions[deviceExtensionsCount] {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    void CreateInstance(const char *title);
    void CreateSurface(GLFWwindow *window);
    void GetPhysicalDevice();
    void CreateLogicalDevice();

    bool CheckValidationLayerSupport() const;
    bool CheckDeviceExtensionSupport() const;


    static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
};

#endif //DS_VULKAN_H
