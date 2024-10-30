#pragma once
#ifndef _POTENT_ENGINE_WINDOW_
#define _POTENT_ENGINE_WINDOW_

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "engineCore.h"

namespace potent {
	class Window {
	private:
		GLFWwindow* mWindowPtr;

	public:
		static Window* instance;
		static double mouseX, mouseY;
		static int windowWidth, windowHeight;
		static double engineTime, engineDeltaTime, engineLastTime;

		virtual void awake() {}
		virtual void start() {}
		virtual void update() {}
		virtual void lateUpdate() {}

		Window() {
			if (Window::instance) {
				ENGINE_WARN("Window static instance already exist @ " << Window::instance << ", exiting from setting new one!");

				return;
			}

			Window::instance = this;
		}

		GLFWwindow* getPtr() { return mWindowPtr; }

		void fullscreenMode() { glfwSetWindowMonitor(mWindowPtr, glfwGetPrimaryMonitor(), 0, 0, glfwGetVideoMode(glfwGetPrimaryMonitor())->width, glfwGetVideoMode(glfwGetPrimaryMonitor())->height, GLFW_DONT_CARE); }
		void windowMode() { glfwSetWindowMonitor(mWindowPtr, NULL, 0, 0, glfwGetVideoMode(glfwGetPrimaryMonitor())->width, glfwGetVideoMode(glfwGetPrimaryMonitor())->height, GLFW_DONT_CARE); }

		bool getKey(std::int32_t key, std::int32_t pressing = GLFW_PRESS) { return glfwGetKey(mWindowPtr, key) == pressing; }

		void disableCursor() { glfwSetInputMode(mWindowPtr, GLFW_CURSOR, GLFW_CURSOR_DISABLED); }
		void enableCursor() { glfwSetInputMode(mWindowPtr, GLFW_CURSOR, GLFW_CURSOR_NORMAL); }

		void run(int width, int height, const char* title) {
			awake();

			glfwInit();

			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_SAMPLES, 16);

			mWindowPtr = glfwCreateWindow(width, height, title, nullptr, nullptr);

			if (!mWindowPtr) {
				ENGINE_ERROR("Cannot create GLFW window!");
				
				return;
			}

			glfwMakeContextCurrent(mWindowPtr);

			if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
				ENGINE_ERROR("Cannot load OpenGL 4.5 core context!");

				return;
			}

			glEnable(GL_MULTISAMPLE);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glfwSwapInterval(1);

			start();

			while (!glfwWindowShouldClose(mWindowPtr)) {
				engineTime = glfwGetTime();
				engineDeltaTime = engineTime - engineLastTime;
				engineLastTime = engineTime;

				glfwGetCursorPos(mWindowPtr, &mouseX, &mouseY);
				glfwGetWindowSize(mWindowPtr, &windowWidth, &windowHeight);

				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

				glClear(0x4100);

				update();
				
				glfwSwapBuffers(mWindowPtr);

				lateUpdate();

				glfwPollEvents();
			}

			glfwTerminate();
		}
	};

	Window* Window::instance = nullptr;

	double Window::mouseX = 0.0;
	double Window::mouseY = 0.0;

	int Window::windowWidth = 0;
	int Window::windowHeight = 0;

	double Window::engineTime = 0.0;
	double Window::engineDeltaTime = 0.0;
	double Window::engineLastTime = 0.0f;
}

#endif