#include "Core/engineWindow.h"
#include "Renderer/engineCamera.h"
#include "Core/engineCore.h"
#include "Renderer/engineRenderer.h"

class Wnd : public potent::Window {
private:
	potent::Camera editorCamera;

	bool projectionOrtho = false;

public:
	virtual void awake() override {

	}

	virtual void start() override {


		glfwSwapInterval(1);
	}

	virtual void update() override {
		glViewport(0, 0, windowWidth, windowHeight);


	}

	virtual void lateUpdate() override {
		if (getKey('W')) {
			editorCamera.position += editorCamera.cameraDirection;
		}
		else if (getKey('S')) {
			editorCamera.position -= editorCamera.cameraDirection;
		}

		if (getKey('A')) {
			editorCamera.position -= editorCamera.right;
		}
		else if (getKey('D')) {
			editorCamera.position += editorCamera.right;
		}

		if (glfwGetMouseButton(getPtr(), GLFW_MOUSE_BUTTON_2)) {
			disableCursor();

			editorCamera.cameraViewMatrix(mouseX, mouseY);
		}
		else {
			enableCursor();
		}

		if (projectionOrtho) {
			editorCamera.cameraProjectionOrthographic(windowWidth, windowHeight);
		}
		else {
			editorCamera.cameraProjectionPerspective(windowWidth, windowHeight);
		}
	}
} editorWindow;

int main() {
	editorWindow.run(800, 600, "Potent Editor");

	return 0;
}