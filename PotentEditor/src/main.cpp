#include "Core/engineWindow.h"
#include "Renderer/engineCamera.h"
#include "Core/engineCore.h"
#include "Renderer/engineRenderer.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_opengl3.h"

void StyleColorsMonochrome(ImGuiStyle* dst = (ImGuiStyle*)0) {
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.85f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.11f, 0.11f, 0.14f, 0.92f);
    colors[ImGuiCol_Border] = ImVec4(0.50f, 0.50f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.43f, 0.43f, 0.43f, 0.39f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.70f, 0.70f, 0.70f, 0.40f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.83f, 0.83f, 0.83f, 0.69f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.35f, 0.35f, 0.35f, 0.83f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.17f, 0.17f, 0.17f, 0.87f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.20f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.35f, 0.35f, 0.35f, 0.80f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.23f, 0.23f, 0.23f, 0.60f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.64f, 0.64f, 0.64f, 0.30f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.86f, 0.86f, 0.86f, 0.40f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 0.50f);
    colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
    colors[ImGuiCol_Button] = ImVec4(0.45f, 0.45f, 0.45f, 0.62f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.56f, 0.56f, 0.56f, 0.79f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.38f, 0.38f, 0.38f, 0.45f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.65f, 0.65f, 0.65f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.77f, 0.77f, 0.77f, 0.80f);
    colors[ImGuiCol_Separator] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.88f, 0.88f, 0.88f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.92f, 0.92f, 0.92f, 0.90f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.72f, 0.72f, 0.72f, 0.80f);
    colors[ImGuiCol_Tab] = ImVec4(0.62f, 0.62f, 0.62f, 0.79f);
    colors[ImGuiCol_TabSelected] = ImVec4(0.32f, 0.32f, 0.32f, 0.84f);
    colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.00f, 0.00f, 0.00f, 0.80f);
    colors[ImGuiCol_TabDimmed] = ImVec4(0.09f, 0.09f, 0.09f, 0.82f);
    colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.15f, 0.15f, 0.15f, 0.84f);
    colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.17f, 0.17f, 0.17f, 0.80f);
    colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.26f, 0.26f, 0.28f, 1.00f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.07f);
    colors[ImGuiCol_TextLink] = ImVec4(0.00f, 0.00f, 1.00f, 0.80f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.89f, 0.89f, 0.89f, 0.80f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

class Wnd : public potent::Window {
private:
	potent::Camera editorCamera;

	bool projectionOrtho = false;

public:
	virtual void awake() override {

	}

	virtual void start() override {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.IniFilename = NULL;

        // Setup Dear ImGui style
        StyleColorsMonochrome();
        ImGui::GetStyle().GrabMinSize = 10;
        ImGui::GetStyle().ScrollbarSize = 16;
        ImGui::GetStyle().WindowBorderSize = 0;
        ImGui::GetStyle().WindowRounding = 0;
        ImGui::GetStyle().ChildRounding = 0;
        ImGui::GetStyle().FrameRounding = 0;
        ImGui::GetStyle().PopupRounding = 0;
        ImGui::GetStyle().ScrollbarRounding = 0;
        ImGui::GetStyle().GrabRounding = 0;
        ImGui::GetStyle().TabRounding = 0;

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(getPtr(), true);
        ImGui_ImplOpenGL3_Init("#version 450");

		glfwSwapInterval(1);
	}

	virtual void update() override {
		glViewport(0, 0, windowWidth, windowHeight);

        ImGui::SetClipboardText(glfwGetClipboardString(getPtr()));

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow();


        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

	return 0;
}