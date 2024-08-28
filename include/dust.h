#ifndef DUST_LIBRARY_H
#define DUST_LIBRARY_H

#define DUST_API __declspec(dllexport)
#include <memory>

struct GLFWwindow;

#include <glm/glm.hpp>

namespace ds {

	class DUST_API Input {
	public:
		static bool KeyDown(int key);
		static bool KeyJustDown(int key);
		static bool KeyJustUp(int key);

		static bool MouseDown(int button);
		static bool MouseJustDown(int button);
		static bool MouseJustUp(int button);

		int keyboardButtons[256];
		int mouseButtons[3];
	};

	class DUST_API Dust {
	public:
		Dust(const char *title, int windowWidth, int windowHeight, bool fullscreen);
		virtual ~Dust();

		bool ShouldClose();

		void BeingFrame();
		void EndFrame();

	private:
		GLFWwindow *window { nullptr };
		int windowWidth { 0 };
		int windowHeight { 0 };
	};
}
#endif //DUST_LIBRARY_H
