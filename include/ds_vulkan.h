//
// Created by manue on 8/16/2024.
//

#ifndef DS_VULKAN_H
#define DS_VULKAN_H

#include <optional>

struct QueueFamilyIndices {
    std::optional<unsigned int> graphicsFamily;
};

class VkContext {
public:
    explicit VkContext(const char *title);
    ~VkContext();

private:
    // the vulkan instance
    VkInstance instance{};
    // physical device
    VkPhysicalDevice physicalDevice { VK_NULL_HANDLE };


    // validation layers
    static constexpr bool enableValidationLayers { true };
    static constexpr int validationLayersCount { 1 };
    const char *validationLayers[validationLayersCount] {
        "VK_LAYER_KHRONOS_validation",
    };


    void CreateInstance(const char *title);
    bool CheckValidationLayerSupport() const;
    void GetPhysicalDevice();
    QueueFamilyIndices FindQueueFamilies();
};

#endif //DS_VULKAN_H
