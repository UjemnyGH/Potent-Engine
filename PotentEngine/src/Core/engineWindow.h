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
		double mouseX, mouseY;
		int windowWidth, windowHeight;
		double engineTime, engineDeltaTime, engineLastTime;

		virtual void awake() {}
		virtual void start() {}
		virtual void update() {}
		virtual void lateUpdate() {}

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
			glfwWindowHint(GLFW_SAMPLES, 4);

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

				update();
				
				glfwSwapBuffers(mWindowPtr);

				lateUpdate();

				glfwPollEvents();

				glClear(0x4100);

				glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			}

			glfwTerminate();
		}
	};
}

#endif