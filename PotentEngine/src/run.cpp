#include "Core/engineWindow.h"
#include "Core/engineFileSystem.h"
#include "Renderer/engineRenderer.h"
#include "Renderer/engineCamera.h"
#include "Physics/engineParticle.h"
#include "Physics/engineCollisions.h"
#include "Scene/engineScene.h"
#include "Scene/engineLights.h"
#include <thread>

class SmartRender {
private:
	std::vector<potent::Renderer> mRendereres;
};

int resolutionScale = 1;
int colorScale = 128;

const int G_PHYSICS_SPEED = 32;

class MainScene : public potent::Scene {
private:
	potent::Renderer renderer;
	std::vector<potent::RenderObjectData> models;
	std::vector<potent::RenderData> renderDatas;
	std::vector<potent::Texture> textures;

	bool escapePressed = false;
	bool f1Pressed = false;
	bool cameraLock = false;
	bool projectionOrtho = false;

	potent::Camera camera;
	potent::Particle player;

	potent::Shader vs, fs;
	potent::Texture skybox;
	potent::Texture snow;

	int frameCounter100 = 0;
	bool physicsModelsExist = false;
	potent::Light light;

public:
	MainScene() : Scene() {
		name = "MainScene";
	}

	virtual void awake() override {
		ENGINE_INFO("Awake scene " << name);
	}

	virtual void start() override {
		ENGINE_INFO("Started scene " << name);

		camera.zFar = 100.0f;

		vs.loadFromFile("assets/shaders/texture.vert", GL_VERTEX_SHADER);
		fs.loadFromFile("assets/shaders/texture.frag", GL_FRAGMENT_SHADER);

		renderer.attachShader(&vs);
		renderer.attachShader(&fs);
		renderer.relinkShaderProgram();

		renderer.useMaxTextureArray();

		std::uint32_t textureWidth, textureHeight;
		std::vector<std::uint8_t> pixels = potent::loadImageData("assets/textures/ph_skybox.png", &textureWidth, &textureHeight);

		skybox.textureArrayBuffer.bindData(pixels, textureWidth, textureHeight);
		skybox.name = "SKYBOX_TEXTURE";

		pixels = potent::loadImageData("assets/textures/ph_snow256x256.png", &textureWidth, &textureHeight);
		snow.textureArrayBuffer.bindData(pixels, textureWidth, textureHeight);
		snow.name = "SNOW256";

		models.push_back(potent::RenderObjectData());
		models[0].makeModel(potent::loadPLYMesh("assets/models/cube.ply"));
		models[0].name = "SKYBOX_CUBE";
		models[0].transform.SetScale(potent::RVec(10000.0f));
		std::fill(models[0].textureId.begin(), models[0].textureId.end(), 0.0f);

		models.push_back(potent::RenderObjectData());
		models[1].makeModel(potent::loadPLYMesh("assets/models/ph_map2.ply"));
		models[1].name = "MAP1";
		models[1].transform.SetScale(potent::RVec(100.0f));
		models[1].transform.SetRotation(potent::RVec(potent::ToRadians(90.0f), 0.0f));
		std::fill(models[1].textureId.begin(), models[1].textureId.end(), 0.0f);

		renderDatas.push_back(potent::RenderData());
		renderDatas[0].meshData.push_back(&models[0]);
		renderDatas[0].texturesPtr[0] = &skybox;

		renderDatas.push_back(potent::RenderData());
		renderDatas[1].meshData.push_back(&models[1]);
		renderDatas[1].texturesPtr[0] = &snow;

		renderDatas[0].joinAndBindData();
		renderDatas[1].joinAndBindData();

		physicsModelsExist = true;
	}

	virtual void update() override {
		light.lightingData.direction = potent::RVec(0.1f, -0.8f, 0.1f).Normalize();
		light.lightingData.ambient = potent::RVec(0.3f);
		light.lightingData.diffuse = potent::RVec(1.0f);
		light.lightingData.specular = potent::RVec(0.1f);
		light.lightingData.lightType = DIRECTIONAL_LIGHT;

		models[0].transform.SetPosition(camera.position);

		renderDatas[0].joinAndBindData();

		for (std::size_t i = 0; i < renderDatas.size(); i++) {
			renderer.render(&renderDatas[i], camera.getProjectionMatrix(), camera.getViewMatrix());
		}

		frameCounter100++;

		if (frameCounter100 > 100) {
			frameCounter100 = 0;

			ENGINE_INFO("FPS: " << 1.0 / potent::Window::engineDeltaTime);
		}

		//potent::debugDrawModel(&models[1], camera.getProjectionMatrix(), camera.getViewMatrix());

		glViewport(0, 0, potent::Window::windowWidth, potent::Window::windowHeight);

		potent::GLOBAL_SCENE_HANDLER.getPostProcessingSquareRenderDataPointer()->vertexArray.bind();

		getShaderLightStorage()->bindData((void*) & light.lightingData, sizeof(potent::LightingData), 1);

		potent::GLOBAL_SCENE_HANDLER.getPostProcessingSquareRenderDataPointer()->vertexArray.unbind();

		potent::GLOBAL_SCENE_HANDLER.getPostProcessingRendererPointer()->operator()()->use();

		glUniform1i(glGetUniformLocation(potent::GLOBAL_SCENE_HANDLER.getPostProcessingRendererPointer()->operator()()->id, "modifier"), colorScale);
		glUniform4f(glGetUniformLocation(potent::GLOBAL_SCENE_HANDLER.getPostProcessingRendererPointer()->operator()()->id, "uViewPosition"), camera.position.x, camera.position.y, camera.position.z, 0.0f);

		potent::GLOBAL_SCENE_HANDLER.getPostProcessingRendererPointer()->operator()()->unuse();
	}

	virtual void lateUpdate() override {

		const potent::real playerSpeed = 10.0f;

		if (potent::Window::instance->getKey(GLFW_KEY_ESCAPE) && !escapePressed) {
			escapePressed = true;
			cameraLock = !cameraLock;
		}
		else if (!potent::Window::instance->getKey(GLFW_KEY_ESCAPE) && escapePressed) {
			escapePressed = false;
		}

		if (potent::Window::instance->getKey(GLFW_KEY_F1) && !f1Pressed) {
			f1Pressed = true;
			projectionOrtho = !projectionOrtho;
		}
		else if (!potent::Window::instance->getKey(GLFW_KEY_F1) && f1Pressed) {
			f1Pressed = false;
		}

		if (potent::Window::instance->getKey('W')) {
			player.addForce(camera.front * playerSpeed);
		}
		else if (potent::Window::instance->getKey('S')) {
			player.addForce(camera.front.Negate() * playerSpeed);
		}

		if (potent::Window::instance->getKey('A')) {
			player.addForce(camera.right.Negate() * playerSpeed);
		}
		else if (potent::Window::instance->getKey('D')) {
			player.addForce(camera.right * playerSpeed);
		}

		if (potent::Window::instance->getKey(' ')) {
			player.addForce(potent::RVec(0.0f, 50.0f));
		}

		camera.position = player.position + potent::RVec(0.0f, 0.8f, 0.0f);

		if (cameraLock) {
			potent::Window::instance->disableCursor();

			camera.cameraViewMatrix(potent::Window::mouseX, potent::Window::mouseY);
		}
		else {
			potent::Window::instance->enableCursor();
		}

		camera.updateViewMatrix();

		if (projectionOrtho) {
			camera.cameraProjectionOrthographic(potent::Window::windowWidth, potent::Window::windowHeight);
		}
		else {
			camera.cameraProjectionPerspective(potent::Window::windowWidth, potent::Window::windowHeight);
		}
	}

	virtual void fixedUpdate() override {
		if (physicsModelsExist) {
			potent::RVec closestPoint = potent::RVec(models[1].meshData.vertices[0], models[1].meshData.vertices[1], models[1].meshData.vertices[2]);

			for (std::size_t i = 0; i < models[1].meshData.vertices.size() / 9; i++) {
				potent::real* dataPtr = &models[1].meshData.vertices[i * 9];

				potent::RVec a = models[1].transform.GetTransform() * potent::RVec(dataPtr[0], dataPtr[1], dataPtr[2]);

				if (player.position.Distance(a) > player.position.Distance(closestPoint)) {
					continue;
				}

				closestPoint = a;

				potent::RVec b = models[1].transform.GetTransform() * potent::RVec(dataPtr[3], dataPtr[4], dataPtr[5]);
				potent::RVec c = models[1].transform.GetTransform() * potent::RVec(dataPtr[6], dataPtr[7], dataPtr[8]);

				potent::RVec point = potent::pointOnPlane(player.position, a, b, c);

				if (point.w != 1.0f) {
					potent::AABox playerBox;
					playerBox.position = player.position;
					playerBox.size = potent::RVec(0.4f, 1.0f, 0.4f);

					potent::RVec usedPoint = point;

					if (potent::pointBoxCollision(playerBox, usedPoint) || potent::lineTriangleIntersectionPoint(player.lastPosition, player.velocity.Normalize(), a, b, c).Distance(player.lastPosition) <= player.lastPosition.Distance(player.position)) {
						potent::RVec collisionNormal = potent::RVec::PlaneNormal(a, b, c).Negate();
						potent::resolveCollision(1.0 / (double)G_PHYSICS_SPEED, collisionNormal, &player);
						potent::resolvePenetration(1.0 / (double)G_PHYSICS_SPEED, point.Distance(player.position - (collisionNormal * playerBox.size) + (player.velocity * collisionNormal * (1.0 / (double)G_PHYSICS_SPEED))), collisionNormal, &player);
					
						break;
					}
				}
			}
		}

		player.addForce(potent::RVec(0.0f, -9.8f, 0.0f));

		player.updateParticle(1.0 / (double)G_PHYSICS_SPEED);
	}
} GLOBAL_MAIN_SCENE;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	/*if (yoffset < 0.0 && resolutionScale > 1) {
		resolutionScale--;
	}

	if (yoffset > 0.0 && resolutionScale < 0x1000) {
		resolutionScale++;
	}*/

	if (yoffset < 0.0 && colorScale > 1) {
		colorScale--;
	}

	if (yoffset > 0.0 && colorScale < 0x100) {
		colorScale++;
	}
}

class Wnd : public potent::Window {
private:
	potent::Shader postProcessLightVertexShader, postProcessLightFragmentShader;

	const int physicsSpeed = G_PHYSICS_SPEED;

	std::jthread physicsThread;

public:
	void physicsUpdate() {
		while (1) {
			std::chrono::time_point start = std::chrono::high_resolution_clock::now();

			potent::GLOBAL_SCENE_HANDLER.fixedUpdateCurrent();

			while (std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start).count() < 1.0 / (double)physicsSpeed) continue;
		}
	}

	virtual void awake() override {
		printf("Awake\n");

		potent::GLOBAL_SCENE_HANDLER.awakeCurrent();
	}

	virtual void start() override {
		printf("Start\n");

		glfwSetScrollCallback(getPtr(), scroll_callback);

		potent::GLOBAL_SCENE_HANDLER.startCurrent();

		postProcessLightVertexShader.loadFromFile("assets/shaders/post_light.vert", GL_VERTEX_SHADER);
		postProcessLightFragmentShader.loadFromFile("assets/shaders/post_light.frag", GL_FRAGMENT_SHADER);

		physicsThread = std::jthread(&Wnd::physicsUpdate, this);
		physicsThread.detach();

		glfwSwapInterval(0);
	}

	virtual void update() override {		
		glViewport(0, 0, windowWidth / resolutionScale, windowHeight / resolutionScale);

		potent::GLOBAL_SCENE_HANDLER.updateCurrent(windowWidth / resolutionScale, windowHeight / resolutionScale, &postProcessLightVertexShader, &postProcessLightFragmentShader);
	}

	virtual void lateUpdate() override {
		potent::GLOBAL_SCENE_HANDLER.lateUpdateCurrent();
	}

} wnd;

int main(int argv, char** argc) {
	potent::GLOBAL_SCENE_HANDLER.attachScene(&GLOBAL_MAIN_SCENE);
	potent::GLOBAL_SCENE_HANDLER.switchScene("MainScene");

	wnd.run(800, 600, "Window");

	return 0;
}