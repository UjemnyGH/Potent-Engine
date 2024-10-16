#include "Core/engineWindow.h"
#include "Core/engineFileSystem.h"
#include "Renderer/engineRenderer.h"
#include "Renderer/engineCamera.h"
#include "Physics/engineParticle.h"
#include "Physics/engineCollisions.h"
#include <thread>

class SmartRender {
private:
	std::vector<potent::Renderer> mRendereres;
};

class Wnd : public potent::Window {
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
	const int physicsSpeed = 32;

public:
	void physicsThread() {
		for (std::size_t i = 0; i < models[1].meshData.vertices.size() / 9; i++) {
			potent::real* dataPtr = &models[1].meshData.vertices[i * 9];

			potent::RVec a = models[1].transform.GetTransform() * potent::RVec(dataPtr[0], dataPtr[1], dataPtr[2]);
			potent::RVec b = models[1].transform.GetTransform() * potent::RVec(dataPtr[3], dataPtr[4], dataPtr[5]);
			potent::RVec c = models[1].transform.GetTransform() * potent::RVec(dataPtr[6], dataPtr[7], dataPtr[8]);

			potent::RVec point = potent::pointOnPlane(player.position, a, b, c);

			if (point.w != 1.0f) {
				potent::AABox playerBox;
				playerBox.position = player.position;
				playerBox.size = potent::RVec(0.5f, 2.0f, 0.5f);

				if (potent::pointBoxCollision(playerBox, point)) {
					potent::RVec collisionNormal = potent::RVec::PlaneNormal(a, b, c);
					potent::resolveCollision(1.0 / (double)physicsSpeed, collisionNormal, &player);
					potent::resolvePenetration(1.0 / (double)physicsSpeed, point.Distance(player.position), collisionNormal, &player);
				}
			}
		}
		
		//player.addForce(potent::RVec(0.0f, -9.8f, 0.0f));

		player.updateParticle(1.0 / (double)physicsSpeed);
	}

	virtual void awake() override {
		printf("Awake\n");
	}

	virtual void start() override {
		printf("Start\n");
		camera.zFar = 100.0f;

		vs.loadFromFile("assets/shaders/texture.vert", GL_VERTEX_SHADER);
		fs.loadFromFile("assets/shaders/texture.frag", GL_FRAGMENT_SHADER);

		renderer.attachShader(&vs);
		renderer.attachShader(&fs);
		renderer.relinkShaderProgram();

		renderer.useMaxTexture();

		std::uint32_t textureWidth, textureHeight;
		std::vector<std::uint8_t> pixels = potent::loadImageData("assets/textures/ph_skybox.png", &textureWidth, &textureHeight);

		skybox.textureBuffer.bindData(pixels, textureWidth, textureHeight);
		skybox.name = "SKYBOX_TEXTURE";

		pixels = potent::loadImageData("assets/textures/ph_snow256x256.png", &textureWidth, &textureHeight);
		snow.textureBuffer.bindData(pixels, textureWidth, textureHeight);
		snow.name = "SNOW256";

		models.push_back(potent::RenderObjectData());
		models[0].makeModel(potent::loadPLYMesh("assets/models/cube.ply"));
		models[0].name = "SKYBOX_CUBE";
		models[0].transform.SetScale(potent::RVec(10000.0f));
		std::fill(models[0].textureId.begin(), models[0].textureId.end(), 0.0f);

		models.push_back(potent::RenderObjectData());
		models[1].makeModel(potent::loadPLYMesh("assets/models/ph_map1.ply"));
		models[1].name = "MAP1";
		models[1].transform.SetScale(potent::RVec(10.0f));
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

		glfwSwapInterval(1);
	}

	virtual void update() override {		
		glViewport(0, 0, windowWidth, windowHeight);
		//projection = potent::RMat::OrthograpicSymmertical(2.0f, 2.0f, 0.01f, 100.0f);

		models[0].transform.SetPosition(camera.position);

		//models[0].transform.SetRotation(potent::RVec(potent::ToRadians(180.0f * sin(engineTime)), 0.0f));

		renderDatas[0].joinAndBindData();

		for (std::size_t i = 0; i < renderDatas.size(); i++) {
			renderer.render(&renderDatas[i], camera.getProjectionMatrix(), camera.getViewMatrix());
		}

		frameCounter100++;

		if (frameCounter100 > 100) {
			frameCounter100 = 0;

			ENGINE_INFO("FPS: " << 1.0 / engineDeltaTime);
		}

		potent::debugDrawModel(&models[1], camera.getProjectionMatrix(), camera.getViewMatrix());

		for (std::size_t i = 0; i < models[1].meshData.vertices.size() / 9; i++) {
			potent::real* dataPtr = &models[1].meshData.vertices[i * 9];

			potent::RVec a = models[1].transform.GetTransform() * potent::RVec(dataPtr[0], dataPtr[1], dataPtr[2]);
			potent::RVec b = models[1].transform.GetTransform() * potent::RVec(dataPtr[3], dataPtr[4], dataPtr[5]);
			potent::RVec c = models[1].transform.GetTransform() * potent::RVec(dataPtr[6], dataPtr[7], dataPtr[8]);

			potent::RVec point = potent::pointOnPlane(player.position /* - potent::RVec(0.4f, 1.0f, 0.4f)*/, a, b, c);
			//potent::RVec point2 = potent::pointOnPlane(player.position + potent::RVec(0.4f, 1.0f, 0.4f), a, b, c);

			if (point.w != 1.0f) {// || point2.w != 1.0f) {
				potent::AABox playerBox;
				playerBox.position = player.position;
				playerBox.size = potent::RVec(0.4f, 1.0f, 0.4f);

				potent::RVec usedPoint = point;

				glPointSize(16.0f);
				potent::debugDrawPoint(usedPoint, camera.getProjectionMatrix(), camera.getViewMatrix());
				glPointSize(1.0f);

				/*if (point.Distance(player.position - potent::RVec(0.4f, 1.0f, 0.4f)) > point2.Distance(player.position - potent::RVec(0.4f, 1.0f, 0.4f))) {
					usedPoint = point2;
				}*/

				if (potent::pointBoxCollision(playerBox, usedPoint)) {
					potent::RVec collisionNormal = potent::RVec::PlaneNormal(a, b, c).Negate();
					potent::resolveCollision(engineDeltaTime, collisionNormal, &player);
					potent::resolvePenetration(engineDeltaTime, point.Distance(player.position - (collisionNormal * playerBox.size) + (player.velocity * collisionNormal * engineDeltaTime)), collisionNormal, &player);
				}
				else if (potent::lineTriangleIntersectionPoint(player.lastPosition, player.velocity.Normalize(), a, b, c).Distance(player.lastPosition) <= player.lastPosition.Distance(player.position)) {
					potent::RVec collisionNormal = potent::RVec::PlaneNormal(a, b, c).Negate();
					potent::resolveCollision(engineDeltaTime, collisionNormal, &player);
					potent::resolvePenetration(engineDeltaTime, point.Distance(player.position - (collisionNormal * playerBox.size) + (player.velocity * collisionNormal * engineDeltaTime)), collisionNormal, &player);
				}
			}
		}
	}

	virtual void lateUpdate() override {
		//std::jthread physics(&Wnd::physicsThread, this);
		//physics.detach();

		player.addForce(potent::RVec(0.0f, -98.0f, 0.0f));

		player.updateParticle(engineDeltaTime);

		const potent::real playerSpeed = 100.0f;

		if (getKey(GLFW_KEY_ESCAPE) && !escapePressed) {
			escapePressed = true;
			cameraLock = !cameraLock;
		}
		else if (!getKey(GLFW_KEY_ESCAPE) && escapePressed) {
			escapePressed = false;
		}

		if (getKey(GLFW_KEY_F1) && !f1Pressed) {
			f1Pressed = true;
			projectionOrtho = !projectionOrtho;
		}
		else if (!getKey(GLFW_KEY_F1) && f1Pressed) {
			f1Pressed = false;
		}

		if (getKey('W')) {
			player.addForce(camera.front * playerSpeed);
		}
		else if (getKey('S')) {
			player.addForce(camera.front.Negate() * playerSpeed);
		}

		if (getKey('A')) {
			player.addForce(camera.right.Negate() * playerSpeed);
		}
		else if (getKey('D')) {
			player.addForce(camera.right * playerSpeed);
		}

		if (getKey(' ')) {
			player.addForce(potent::RVec(0.0f, 500.0f));
		}

		camera.position = player.position + potent::RVec(0.0f, 0.8f, 0.0f);

		if (cameraLock) {
			disableCursor();

			camera.cameraViewMatrix(mouseX, mouseY);
		}
		else {
			enableCursor();
		}

		camera.updateViewMatrix();

		if (projectionOrtho) {
			camera.cameraProjectionOrthographic(windowWidth, windowHeight);
		}
		else {
			camera.cameraProjectionPerspective(windowWidth, windowHeight);
		}
	}

} wnd;

int main(int argv, char** argc) {
	wnd.run(800, 600, "Window");

	return 0;
}