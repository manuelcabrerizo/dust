#ifndef DUST_LIBRARY_H
#define DUST_LIBRARY_H

#define DUST_API __declspec(dllexport)

struct GLFWwindow;
class VkContext;

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

class DUST_API Dust {
public:
	Dust(const char *title, int windowWidth, int windowHeight, bool fullscreen);
	virtual ~Dust();
	void Run();

	virtual void Update() = 0;
	virtual void Render() = 0;

private:
	GLFWwindow *window { nullptr };
	int windowWidth { 0 };
	int windowHeight { 0 };
	VkContext *vulkan { nullptr };
};

#endif //DUST_LIBRARY_H
