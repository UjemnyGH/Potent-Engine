#include "Core/engineWindow.h"
#include "Core/engineFileSystem.h"
#include "Renderer/engineRenderer.h"
#include "Renderer/engineCamera.h"
#include "Physics/engineParticle.h"
#include "Physics/engineCollisions.h"
#include "Scene/engineScene.h"
#include "Scene/engineLights.h"
#include "UI/engineUIDebugText.h"
#include "Core/engineRuntimeInfo.h"
#include <thread>
#include <sstream>
#include <random>
#include <chrono>
#include <map>

// TODO:
/*class SmartRender {
private:
	std::vector<potent::Renderer> mRendereres;
};*/

class Profiler {
public:
	using profiler_list = std::vector<std::pair<std::string, std::chrono::high_resolution_clock::time_point>>;

private:
	std::chrono::high_resolution_clock::time_point mStartTime;
	profiler_list mProfilerTimePoints;

public:

	profiler_list* getPointsPointer() { return &mProfilerTimePoints; }

	std::chrono::high_resolution_clock::time_point getStartPoint() { return mStartTime; }

	void startProfiler() {
		mProfilerTimePoints.clear();
		mStartTime = std::chrono::high_resolution_clock::now();
	}

	void addTimePoint(std::string name) {
		mProfilerTimePoints.emplace_back(std::pair(name, std::chrono::high_resolution_clock::now()));
	}
};

int resolutionScale = 1;
int freeCamSpeed = 20;
int fixedIter = 0;

class MainScene : public potent::Scene {
private:
	potent::Renderer renderer;
	//potent::DebugTextRenderer text;

	std::vector<potent::RenderObjectData> models;
	std::vector<potent::RenderData> renderDatas;
	std::vector<potent::Texture> textures;

	bool escapePressed = false;
	bool f1Pressed = false;
	bool cameraLock = false;
	bool projectionOrtho = false;
	bool flashlightOn = false;
	bool flashlightOnPressed = false;

	bool freeCam = false;
	bool freeCamPressed = false;

	bool playerPushback = true;

	potent::Camera camera;
	potent::Particle player;

	potent::Shader vs, fs, gs;
	potent::Texture skybox;
	potent::Texture snow;
	potent::Texture treePlaceholderTexture;

	int frameCounterPerSecond = 0;
	bool physicsModelsExist = false;
	potent::Light light;
	potent::Light light2;
	potent::Light light3;
	potent::Light flashlight;

	potent::BigMeshCollision terrainCollision;

	potent::ParticleForceSolver particleSolver;

	potent::ParticleGravityGenerator gravity;
	potent::ParticleDragGenerator drag;

	potent::Shader postProcessLightVertexShader, postProcessLightFragmentShader;
	potent::PostProcessRenderer lightRenderer;

	potent::RVec testPoint;
	potent::RVec testCollPoint;

	Profiler profiler;

public:
	MainScene() : Scene() {
		name = "MainScene";
	}

	potent::RenderObjectData collisionLines;
	std::ofstream collisionFile;

	virtual void awake() override {
		ENGINE_INFO("Awake scene " << name);

		//gravity.gravity = potent::DVec(0.0, -20.0, 0.0);
		

		drag.linear = 0.01;
		drag.quadratic = 0.01;

		player.setMass(60.0);

		particleSolver.addParticle(&player, &gravity);
		particleSolver.addParticle(&player, &drag);
	}

	virtual void start() override {
		ENGINE_INFO("Started scene " << name);
		collisionFile.open(("collisionFile.txt"), std::ios::binary);

		glfwSwapInterval(0);

		camera.zFar = 100.0f;
		camera.zNear = 0.0001f;

		//text.setup();

		postProcessLightVertexShader.loadFromFile("assets/shaders/post_light.vert", GL_VERTEX_SHADER);
		postProcessLightFragmentShader.loadFromFile("assets/shaders/post_light.frag", GL_FRAGMENT_SHADER);

		lightRenderer.recreateStage();
		lightRenderer.attachShader(&postProcessLightVertexShader);
		lightRenderer.attachShader(&postProcessLightFragmentShader);
		lightRenderer.makeStageRenderable();

		vs.loadFromFile("assets/shaders/texture.vert", GL_VERTEX_SHADER);
		fs.loadFromFile("assets/shaders/texture.frag", GL_FRAGMENT_SHADER);
		gs.loadFromFile("assets/shaders/texture.geom", GL_GEOMETRY_SHADER);

		renderer.attachShader(&vs);
		renderer.attachShader(&fs);
		renderer.attachShader(&gs);
		renderer.relinkShaderProgram();

		renderer.useMaxTextureArray();

		std::uint32_t textureWidth = 0, textureHeight = 0;
		std::vector<std::uint8_t> pixels = potent::loadImageData("assets/textures/ph_skybox.png", &textureWidth, &textureHeight);

		skybox.textureArrayBuffer.bindData(pixels, textureWidth, textureHeight);
		skybox.name = "SKYBOX_TEXTURE";

		pixels.clear();
		textureWidth = 0;
		textureHeight = 0;
		pixels = potent::loadImageData("assets/textures/ph_snow256x256.png", &textureWidth, &textureHeight);
		snow.textureArrayBuffer.bindData(pixels, textureWidth, textureHeight);
		snow.name = "SNOW256";

		models.push_back(potent::RenderObjectData());
		models[0].makeModel(potent::loadPLYMesh("assets/models/cube.ply"));
		models[0].name = "SKYBOX_CUBE";
		models[0].transform.SetScale(potent::RVec(10000.0f));
		//models[0].transform.SetRotation(potent::RVec(0.0f));
		std::fill(models[0].textureId.begin(), models[0].textureId.end(), 0.0f);

		models.push_back(potent::RenderObjectData());
		models[1].makeModel(potent::loadPLYMesh("assets/models/ph_map1.ply"));
		models[1].name = "MAP1";
		models[1].transform.SetScale(potent::RVec(10.0f));
		//models[1].transform.SetRotation(potent::RVec(0.0f));
		std::fill(models[1].textureId.begin(), models[1].textureId.end(), 0.0f);
		//potent::negateMeshNormals(&models[1].meshData);

		potent::RenderObjectData collisionMesh;
		collisionMesh.transform = models[1].transform;
		collisionMesh.name = "COLLISION_MESH";
		collisionMesh.colors.resize((collisionMesh.meshData.vertices.size() / 3) * 4);
		std::fill(collisionMesh.colors.begin(), collisionMesh.colors.end(), 1.0f);
		collisionMesh.textureId.resize((collisionMesh.meshData.vertices.size() / 3));
		std::fill(collisionMesh.textureId.begin(), collisionMesh.textureId.end(), 32.0f);

		for (std::size_t i = 0; i < models[1].meshData.vertices.size() / 9; i++) {
			potent::real* dataPtr = &models[1].meshData.vertices[i * 9];

			potent::RVec a = potent::RVec(dataPtr[0], dataPtr[1], dataPtr[2]);
			potent::RVec b = potent::RVec(dataPtr[3], dataPtr[4], dataPtr[5]);
			potent::RVec c = potent::RVec(dataPtr[6], dataPtr[7], dataPtr[8]);

			collisionMesh.meshData.vertices.push_back(a.x);
			collisionMesh.meshData.vertices.push_back(a.y);
			collisionMesh.meshData.vertices.push_back(a.z);
			collisionMesh.meshData.vertices.push_back(b.x);
			collisionMesh.meshData.vertices.push_back(b.y);
			collisionMesh.meshData.vertices.push_back(b.z);
			collisionMesh.meshData.vertices.push_back(c.x);
			collisionMesh.meshData.vertices.push_back(c.y);
			collisionMesh.meshData.vertices.push_back(c.z);

			collisionMesh.meshData.textureCoordinates.push_back(1.0f);
			collisionMesh.meshData.textureCoordinates.push_back(1.0f);
			collisionMesh.meshData.textureCoordinates.push_back(0.0f);
			collisionMesh.meshData.textureCoordinates.push_back(1.0f);
			collisionMesh.meshData.textureCoordinates.push_back(1.0f);
			collisionMesh.meshData.textureCoordinates.push_back(0.0f);

			potent::RVec normal = potent::RVec::PlaneNormal(a, b, c);

			collisionMesh.meshData.normals.push_back(normal.x);
			collisionMesh.meshData.normals.push_back(normal.y);
			collisionMesh.meshData.normals.push_back(normal.z);
			collisionMesh.meshData.normals.push_back(normal.x);
			collisionMesh.meshData.normals.push_back(normal.y);
			collisionMesh.meshData.normals.push_back(normal.z);
			collisionMesh.meshData.normals.push_back(normal.x);
			collisionMesh.meshData.normals.push_back(normal.y);
			collisionMesh.meshData.normals.push_back(normal.z);
		}

		pixels.clear();
		textureWidth = 0;
		textureHeight = 0;
		pixels = potent::loadImageData("assets/textures/ph_tree1_fin.png", &textureWidth, &textureHeight);
		potent::MeshRawData treePlaceholder = potent::loadPLYMesh("assets/models/ph_tree1.ply");

		treePlaceholderTexture.textureArrayBuffer.bindData(pixels, textureWidth, textureHeight);
		treePlaceholderTexture.name = "TREE_PLACEHOLDER_TEXTURE";

		std::vector<potent::RVec> treePoint;

		std::random_device rd;

		for (int i = 0; i < 1000; i++) {
			treePoint.push_back(potent::RVec((float)(rd() % 2000) - 1000.0f, 1000.0f, (float)(rd() % 2000) - 1000.0f));
		}

		//potent::negateMeshNormals(&models[2].meshData);
		std::size_t treeIter = 0;

		for (auto p : treePoint) {
			for (std::size_t i = 0; i < models[1].meshData.vertices.size() / 9; i++) {
				potent::real* dataPtr = &models[1].meshData.vertices[i * 9];

				potent::RVec a = models[1].transform.GetTransform() * potent::RVec(dataPtr[0], dataPtr[1], dataPtr[2]);
				potent::RVec b = models[1].transform.GetTransform() * potent::RVec(dataPtr[3], dataPtr[4], dataPtr[5]);
				potent::RVec c = models[1].transform.GetTransform() * potent::RVec(dataPtr[6], dataPtr[7], dataPtr[8]);

				/*potent::RVec playerPoint = potent::lineTriangleIntersectionPoint(potent::RVec(0.0f, 1000.0f, 0.0f), potent::RVec(0.0f, -1.0f, 0.0f).Normalize(), a, b, c);

				if (playerPoint.w != 1.0f) {
					player.position = playerPoint;
				}*/

				potent::RVec point = potent::lineTriangleIntersectionPoint(p, potent::RVec(0.0f, -1.0f, 0.0f).Normalize(), a, b, c);

				if (point.w == 0.0f) {
					models.push_back(potent::RenderObjectData());
					models[models.size() - 1].makeModel(treePlaceholder);
					models[models.size() - 1].name = "TREE_PLACEHOLDER";
					models[models.size() - 1].transform.SetScale(potent::RVec(1.0f));
					std::fill(models[models.size() - 1].textureId.begin(), models[models.size() - 1].textureId.end(), 1.0f);
					models[models.size() - 1].transform.SetPosition(point);
					treeIter++;

					ENGINE_INFO((double)treeIter / 10.0 << "%" << " Tree iter: " << treeIter << " Point: " << point);
				}
			}
		}

		ENGINE_INFO("Placed " << treePoint.size() << " trees");

		//terrainCollision.boundCollisionFromRenderObject(models[1]);

		//ENGINE_INFO("Terrain divided to " << terrainCollision.collisionMeshes.size() << " smaller meshes!");
		//
		//for (auto t : terrainCollision.collisionMeshes) {
		//	ENGINE_INFO("Origin : " << t.origin << " Size: " << t.size << " Vert: " << t.vertices.size());
		//}

		renderDatas.push_back(potent::RenderData());
		renderDatas[0].name = "SKYBOX_RENDER_DATA";
		renderDatas[0].meshData.push_back(&models[0]);
		renderDatas[0].texturesPtr[0] = &skybox;

		renderDatas.push_back(potent::RenderData());
		renderDatas[1].texturesPtr[0] = &snow;
		renderDatas[1].texturesPtr[1] = &treePlaceholderTexture;

		renderDatas[1].meshData.push_back(&models[1]);
		renderDatas[1].meshData.push_back(&collisionMesh);
		for (std::size_t i = 0; i < treeIter; i++) {
			renderDatas[1].meshData.push_back(&models[2 + i]);
		}
		
		renderDatas[0].joinAndBindData();
		renderDatas[1].joinAndBindData();

		physicsModelsExist = true;
	}

	potent::Framebuffer shadow;

	int checkWidth = 0, checkHeight = 0;
	bool renewAllWindowSizeBasedData = false;

	std::chrono::high_resolution_clock::time_point c, l = std::chrono::high_resolution_clock::now();
	double d;

	virtual void update() override {
		c = std::chrono::high_resolution_clock::now();
		d = std::chrono::duration<long double>(c - l).count();
		l = c;

		profiler.startProfiler();

		renewAllWindowSizeBasedData = false;
		std::stringstream ss(std::string());

		profiler.addTimePoint("start");

		if (checkWidth != potent::Window::windowWidth || checkHeight != potent::Window::windowHeight) {
			renewAllWindowSizeBasedData = true;
			checkWidth = potent::Window::windowWidth;
			checkHeight = potent::Window::windowHeight;
		}

		if (renewAllWindowSizeBasedData) {
			lightRenderer.makeStageRenderable();
			lightRenderer.initStageBuffers(potent::Window::windowWidth, potent::Window::windowHeight);
		}

		lightRenderer.beginStage();
		profiler.addTimePoint("begin_light_pp");

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(0x4100);

		// Directional light
		light.directionalLightData.direction = potent::RVec(0.3f, -1.0f, 0.4f).Normalize();
		//light.directionalLightData.direction = potent::RVec(0.3f, sin(potent::Window::engineTime / 10.0f), cos(potent::Window::engineTime / 10.0f)).Normalize();
		light.directionalLightData.ambient = potent::RVec(0.001f);
		light.directionalLightData.diffuse = potent::RVec(1.0f);
		light.directionalLightData.specular = potent::RVec(0.01f);
		light.lightType = potent::LightType_Directional;

		light.pointLightData.position = potent::RVec(0.0f, -82.0f, 30.0f);
		light.pointLightData.ambient = potent::RVec(0.0f);
		light.pointLightData.diffuse = potent::RVec(0.0f, 1.0f);
		light.pointLightData.specular = potent::RVec(0.0f, 1.0f);
		light.pointLightData.constant = 1.0f;
		light.pointLightData.linear = 0.06f;
		light.pointLightData.quadratic = 0.06f;
		light.lightType = potent::LightType_Point;

		light2.pointLightData.position = potent::RVec(30.0f, -81.0f, 30.0f);
		light2.pointLightData.ambient = potent::RVec(0.0f);
		light2.pointLightData.diffuse = potent::RVec(0.0f, 0.0f, 1.0f);
		light2.pointLightData.specular = potent::RVec(0.0f, 0.0f, 1.0f);
		light2.pointLightData.constant = 1.0f;
		light2.pointLightData.linear = 0.06f;
		light2.pointLightData.quadratic = 0.06f;
		light2.lightType = potent::LightType_Point;

		light3.pointLightData.position = potent::RVec(20.0f, -82.0f, 0.0f);
		light3.pointLightData.ambient = potent::RVec(0.0f);
		light3.pointLightData.diffuse = potent::RVec(1.0f, 0.0f);
		light3.pointLightData.specular = potent::RVec(1.0f, 0.0f);
		light3.pointLightData.constant = 1.0f;
		light3.pointLightData.linear = 0.06f;
		light3.pointLightData.quadratic = 0.06f;
		light3.lightType = potent::LightType_Point;

		// Flashlight (Spot light)
		flashlight.spotLightData.direction = camera.cameraDirection;
		flashlight.spotLightData.position = camera.position;
		flashlight.spotLightData.ambient = potent::RVec(0.0f);
		flashlight.spotLightData.diffuse = flashlightOn ? potent::RVec(2.0f) : potent::RVec();
		flashlight.spotLightData.specular = potent::RVec(1.0f);
		flashlight.spotLightData.constant = 1.0f;
		flashlight.spotLightData.linear = 0.09f;
		flashlight.spotLightData.quadratic = 0.032f;
		flashlight.spotLightData.cutOff = cos(potent::ToRadians(12.5f));
		flashlight.spotLightData.outerCutOff = cos(potent::ToRadians(25.0f));
		flashlight.lightType = potent::LightType_Spot;

		potent::DirectionalLightData dirLightData[1] = { light.directionalLightData };
		potent::PointLightData pointLightData[3] = { light2.pointLightData, light3.pointLightData, light.pointLightData };
		potent::SpotLightData spotLightData = { flashlight.spotLightData };

		profiler.addTimePoint("set_light_data");

		//models[0].transform.SetPosition(camera.position);
		//models[1].debugDraw(camera.getProjectionMatrix(), camera.getViewMatrix());
		//profiler.addTimePoint("debug_draw_model_1");

		//renderDatas[0].joinAndBindData();

		profiler.addTimePoint("join_and_bind_skybox");

		for (std::size_t i = 0; i < renderDatas.size(); i++) {
			renderer.render(&renderDatas[i], camera.getProjectionMatrix(), camera.getViewMatrix());
		}

		profiler.addTimePoint("main_render_loop");

		//glPointSize(16.0f);
		//if (testPoint.w == 0.0f) {
		//	potent::debugDrawPoint(testPoint, camera.getProjectionMatrix(), camera.getViewMatrix(), potent::RMat::Identity(), potent::RVec(0.0f, 0.0f, 1.0f, 1.0f));
		//	potent::debugDrawPoint(testCollPoint, camera.getProjectionMatrix(), camera.getViewMatrix());
		//}
		//glPointSize(1.0f);
		//
		//glLineWidth(4.0f);
		//potent::debugDrawModel(&collisionLines, camera.getProjectionMatrix(), camera.getViewMatrix(), GL_LINE_STRIP);
		//glLineWidth(1.0f);
		//
		//profiler.addTimePoint("debug_draw_points_and_lines");

		//ss << "Hello world";
		
		//text.setText(ss.str());
		//text.setText(std::string() + camera.position);
		//text.setText("Hello world!");

		lightRenderer.getStageRenderDataPtr()->vertexArray.bind();
		
		getLightStorage()[potent::LightType_Directional].bindData((void*)&dirLightData, sizeof(potent::DirectionalLightData) * 1, potent::LightType_Directional);
		getLightStorage()[potent::LightType_Point].bindData((void*)&pointLightData, sizeof(potent::PointLightData) * 3, potent::LightType_Point);
		getLightStorage()[potent::LightType_Spot].bindData((void*)&spotLightData, sizeof(potent::SpotLightData) * 1, potent::LightType_Spot);
		
		lightRenderer.getStageRenderDataPtr()->vertexArray.unbind();

		lightRenderer()->use();

		if (lightRenderer()->linked) {
			glUniform1i(glGetUniformLocation(lightRenderer()->id, "uMaxDirectionalLights"), 1);
			glUniform1i(glGetUniformLocation(lightRenderer()->id, "uMaxPointLights"), 3);
			glUniform1i(glGetUniformLocation(lightRenderer()->id, "uMaxSpotLights"), 1);
			glUniform4f(glGetUniformLocation(lightRenderer()->id, "uViewPosition"), camera.position.x, camera.position.y, camera.position.z, 0.0f);
		}

		lightRenderer()->unuse();

		profiler.addTimePoint("pass_light_data_to_pp");

		lightRenderer.endStage();
		glClear(0x4100);
		//lightRenderer.getStageGeometryBufferPtr()->copyDepthToAnotherFramebuffer(potent::Window::windowWidth, potent::Window::windowHeight, potent::Window::windowWidth, potent::Window::windowHeight);
		lightRenderer.renderStage();

		profiler.addTimePoint("end_stage_and_render_lights");
		
		if (renewAllWindowSizeBasedData)
		glViewport(0, 0, potent::Window::windowWidth, potent::Window::windowHeight);

		//text.setWindowSize(potent::Window::windowWidth, potent::Window::windowHeight);
		//text.updateTextVertices(potent::RVec(-0.9f, 0.9f));
		//text.render();


		//glClear(0x4100);

		//potent::GLOBAL_SCENE_HANDLER.getGeometryBufferPointer()->copyDepthToAnotherFramebuffer(potent::Window::windowWidth, potent::Window::windowHeight, potent::Window::windowWidth, potent::Window::windowHeight);

		//glDisable(GL_DEPTH_TEST);
		//glBlendFunc(1, 0);

		/*potent::RMat debugTransform = potent::RMat::Scale(potent::RVec(0.4f, 1.0f, 0.4f)) * potent::RMat::Translate(camera.position - potent::RVec(0.0f, 0.8f, 0.0f));
		potent::debugDrawLinesBox(camera.getProjectionMatrix(), camera.getViewMatrix(), debugTransform);
		
		potent::CollisionMesh currentCollisionMesh = terrainCollision.getCollisionMeshPointer(player.position);
		potent::debugDrawCollisionMesh(&currentCollisionMesh, camera.getProjectionMatrix(), camera.getViewMatrix());*/
		
		//models[1].debugDraw(camera.getProjectionMatrix(), camera.getViewMatrix());

		/*glPointSize(16.0f);
		potent::debugDrawPoint(potent::RVec(10.0f, -81.0f, 10.0f), camera.getProjectionMatrix(), camera.getViewMatrix());

		glPointSize(1.0f);*/

		//glPointSize(1.0f);
		//glEnable(GL_DEPTH_TEST);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//

		/*shadow.bind();
		
		camera.position

		for (std::size_t i = 0; i < renderDatas.size(); i++) {
			renderer.render(&renderDatas[i], camera.getProjectionMatrix(), camera.getViewMatrix());
		}
		
		shadow.unbind();*/

		profiler.addTimePoint("update_end");
	}

	virtual void lateUpdate() override {
		profiler.addTimePoint("late_update_start");

		if (potent::Window::scrollY < 0.0 && freeCamSpeed > 1) {
			freeCamSpeed--;
		}

		if (potent::Window::scrollY > 0.0 && freeCamSpeed < 0x1000) {
			freeCamSpeed++;
		}

		profiler.addTimePoint("scroll_check");

		if (potent::Window::instance->getKey(GLFW_KEY_ESCAPE) && !escapePressed) {
			escapePressed = true;
			cameraLock = !cameraLock;
		}
		else if (!potent::Window::instance->getKey(GLFW_KEY_ESCAPE) && escapePressed) {
			escapePressed = false;
		}

		if (potent::Window::instance->getKey('F') && !flashlightOnPressed) {
			flashlightOnPressed = true;
			flashlightOn = !flashlightOn;
		}
		else if (!potent::Window::instance->getKey('F') && flashlightOnPressed) {
			flashlightOnPressed = false;
		}

		if (potent::Window::instance->getKey(GLFW_KEY_F1) && !f1Pressed) {
			f1Pressed = true;
			projectionOrtho = !projectionOrtho;
		}
		else if (!potent::Window::instance->getKey(GLFW_KEY_F1) && f1Pressed) {
			f1Pressed = false;
		}

		if (potent::Window::instance->getKey(GLFW_KEY_F2) && !freeCamPressed) {
			freeCamPressed = true;
			freeCam = !freeCam;
		}
		else if (!potent::Window::instance->getKey(GLFW_KEY_F2) && freeCamPressed) {
			freeCamPressed = false;
		}

		profiler.addTimePoint("keys_checks");

		camera.position = player.position + potent::RVec(0.0f, 0.8f, 0.0f);

		if (cameraLock) {
			potent::Window::instance->disableCursor();

			camera.cameraViewMatrix(potent::Window::mouseX, potent::Window::mouseY);
		}
		else {
			potent::Window::instance->enableCursor();
		}

		profiler.addTimePoint("camera_cursor");

		camera.updateViewMatrix();

		profiler.addTimePoint("camera_update_view_mat");

		if (projectionOrtho) {
			camera.cameraProjectionOrthographic(potent::Window::windowWidth, potent::Window::windowHeight);
		}
		else {
			camera.cameraProjectionPerspective(potent::Window::windowWidth, potent::Window::windowHeight);
		}

		profiler.addTimePoint("camera_update_projection_mat");

		if (potent::Window::instance->getKey('R')) {
			lightRenderer.recreateStage();

			postProcessLightVertexShader.loadFromFile("assets/shaders/post_light.vert", GL_VERTEX_SHADER);
			postProcessLightFragmentShader.loadFromFile("assets/shaders/post_light.frag", GL_FRAGMENT_SHADER);
		}

		profiler.addTimePoint("late_update_end");

		frameCounterPerSecond++;

		if (frameCounterPerSecond > (int)(1.0 / potent::Window::engineDeltaTime)) {
			frameCounterPerSecond = 0;

			ENGINE_INFO("FPS: " << 1.0 / potent::Window::engineDeltaTime << " DT: " << potent::Window::engineDeltaTime * 1000.0f << "ms DT2: " << d * 1000.0f << "ms Fixed FPS: " << 1.0 / potent::Window::engineFixedDeltaTime << " Fixed iter: " << fixedIter);
			//ENGINE_INFO("\n" << camera.cameraDirection << "\n" << camera.position << "\n" << "Velocity: " << player.velocity.Length());
			Profiler::profiler_list* profilerPtr = profiler.getPointsPointer();

			for (std::uint32_t i = 0; i < profilerPtr->size(); i++) {
				std::cout << "[PROFILER]: " << (*profilerPtr)[i].first << " : \n" << "From start: " << std::chrono::duration<long double, std::milli>((*profilerPtr)[i].second - profiler.getStartPoint()).count() << " ms, ";

				if (i == 0) {
					std::cout << "From last: " << std::chrono::duration<long double, std::milli>((*profilerPtr)[i].second - profiler.getStartPoint()).count() << " ms" << std::endl;
				}
				else {
					std::cout << "From last: " << std::chrono::duration<long double, std::milli>((*profilerPtr)[i].second - (*profilerPtr)[i - 1].second).count() << " ms" << std::endl;
				}
			}

			fixedIter = 0;
		}
	}

	virtual void fixedUpdate() override {

		fixedIter++;
		const potent::real playerSpeed = 20.0f;
		playerPushback = true;

		if (freeCam) {
			if (potent::Window::instance->getKey('W')) {
				playerPushback = false;
				player.addForce(camera.cameraDirection * freeCamSpeed);
			}
			else if (potent::Window::instance->getKey('S')) {
				playerPushback = false;
				player.addForce(camera.cameraDirection.Negate() * freeCamSpeed);
			}

			if (potent::Window::instance->getKey('A')) {
				playerPushback = false;
				player.addForce(camera.right.Negate() * freeCamSpeed);
			}
			else if (potent::Window::instance->getKey('D')) {
				playerPushback = false;
				player.addForce(camera.right * freeCamSpeed);
			}

			if (potent::Window::instance->getKey(' ')) {
				playerPushback = false;
				player.addForce(camera.right.Cross(camera.cameraDirection) * freeCamSpeed);
			}
			else if (potent::Window::instance->getKey('C')) {
				playerPushback = false;
				player.addForce(camera.right.Cross(camera.cameraDirection).Negate() * freeCamSpeed);
			}

			if (playerPushback) {
				player.addForce(player.velocity.Negate() * 10.0f);
			}
		}
		else {
			if (potent::Window::instance->getKey('W')) {
				playerPushback = false;
				player.addForce(camera.front * playerSpeed);
			}
			else if (potent::Window::instance->getKey('S')) {
				playerPushback = false;
				player.addForce(camera.front.Negate() * playerSpeed);
			}

			if (potent::Window::instance->getKey('A')) {
				playerPushback = false;
				player.addForce(camera.right.Negate() * playerSpeed);
			}
			else if (potent::Window::instance->getKey('D')) {
				playerPushback = false;
				player.addForce(camera.right * playerSpeed);
			}

			if (potent::Window::instance->getKey(' ')) {
				player.addForce(potent::RVec(0.0f, 50.0f));
			}

			if (playerPushback) {
				potent::DVec pushBack = potent::DVec(player.velocity.x, 0.0, player.velocity.z);

				player.addForce(pushBack.Negate());
			}
		}

		if (physicsModelsExist) {
			//potent::CollisionMesh currentCollisionMesh = terrainCollision.getCollisionMeshPointer(player.position);
			//std::cout << "Current origin : " << currentCollisionMesh.origin << std::endl; 
			//std::cout << "Current vertices size : " << currentCollisionMesh.vertices.size() << std::endl; 

			for (std::size_t i = 0; i < models[1].meshData.vertices.size() / 9; i++) {
				potent::real* dataPtr = &models[1].meshData.vertices[i * 9];

				potent::RVec a = models[1].transform.GetTransform() * potent::RVec(dataPtr[0], dataPtr[1], dataPtr[2]);
				potent::RVec b = models[1].transform.GetTransform() * potent::RVec(dataPtr[3], dataPtr[4], dataPtr[5]);
				potent::RVec c = models[1].transform.GetTransform() * potent::RVec(dataPtr[6], dataPtr[7], dataPtr[8]);

				//potent::RVec testPoint = potent::lineTriangleIntersectionPoint(player.position + potent::RVec(0.0f, 100.0f, 0.0f), potent::RVec(0.0f, -1.0f), a, b, c);
				potent::RVec tempTestPoint = potent::lineTriangleIntersectionPoint(camera.position, camera.cameraDirection.Normalize(), a, b, c);

				if (tempTestPoint.w != 1.0f) {
					testPoint = tempTestPoint;
				}

				if (player.position.Distance(a) < a.Distance(b) || player.position.Distance(a) < a.Distance(c)) {
					potent::RVec point = potent::pointOnPlane(player.position, a, b, c);

					if (point.w != 1.0f) {
						potent::AABox playerBox;
						playerBox.position = player.position;
						playerBox.size = potent::RVec(0.4f, 1.0f, 0.4f);
						testCollPoint = point;

						potent::RVec linePoint = potent::lineTriangleIntersectionPoint(player.lastPosition, player.velocity.Normalize(), a, b, c);

						if (potent::pointBoxCollision(playerBox, point) || linePoint.Distance(player.lastPosition) <= player.lastPosition.Distance(player.position)) {
							potent::RVec collisionNormal = potent::RVec::PlaneNormal(a, c, b);
							potent::resolveCollision(potent::Window::engineFixedDeltaTime, collisionNormal.Negate(), &player);
							potent::resolvePenetration(potent::Window::engineFixedDeltaTime, point.Distance(player.position - (collisionNormal.Negate() * playerBox.size) + (player.velocity * collisionNormal.Negate() * potent::Window::engineFixedDeltaTime)), collisionNormal, &player);
							collisionLines.meshData.vertices.emplace_back(player.position.x);
							collisionLines.meshData.vertices.emplace_back(player.position.y);
							collisionLines.meshData.vertices.emplace_back(player.position.z);
							collisionFile << potent::Window::engineTime << " sec : " << player.position << "\n";

							break;
						}
					}
				}
			}

			/*for (std::size_t i = 0; i < currentCollisionMesh.vertices.size() / 9; i++) {
				potent::real* dataPtr = &currentCollisionMesh.vertices[i * 9];

				potent::RVec a = currentCollisionMesh.pTransform->GetTransform() * potent::RVec(dataPtr[0], dataPtr[1], dataPtr[2]);
				potent::RVec b = currentCollisionMesh.pTransform->GetTransform() * potent::RVec(dataPtr[3], dataPtr[4], dataPtr[5]);

				if (player.position.Distance(a) < a.Distance(b) * 2.0) {
					potent::RVec c = models[1].transform.GetTransform() * potent::RVec(dataPtr[6], dataPtr[7], dataPtr[8]);

					potent::RVec point = potent::pointOnPlane(player.position, a, b, c);

					if (point.w != 1.0f) {
						potent::AABox playerBox;
						playerBox.position = player.position;
						playerBox.size = potent::RVec(0.4f, 1.0f, 0.4f);

						potent::RVec usedPoint = point;

						if (potent::pointBoxCollision(playerBox, usedPoint) || potent::lineTriangleIntersectionPoint(player.lastPosition, player.velocity.Normalize(), a, b, c).Distance(player.lastPosition) <= player.lastPosition.Distance(player.position)) {
							potent::RVec collisionNormal = potent::RVec::PlaneNormal(a, b, c).Negate();
							potent::resolveCollision(potent::Window::engineFixedDeltaTime, collisionNormal, &player);
							potent::resolvePenetration(potent::Window::engineFixedDeltaTime, point.Distance(player.position - (collisionNormal * playerBox.size) + (player.velocity * collisionNormal * potent::Window::engineFixedDeltaTime * 2.0)), collisionNormal, &player);
					
							break;
						}
					}
				}
			}*/
		}

		if (!freeCam) {
			particleSolver.solveParticleForces(potent::Window::engineFixedDeltaTime);
		}

		player.updateParticle(potent::Window::engineFixedDeltaTime);
	}

	~MainScene() {
		collisionFile.close();
	}
} GLOBAL_MAIN_SCENE;

//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	//if (yoffset < 0.0 && resolutionScale > 1) {
	//	resolutionScale--;
	//}
	//
	//if (yoffset > 0.0 && resolutionScale < 0x1000) {
	//	resolutionScale++;
	//}

	

	/*if (yoffset < 0.0 && colorScale > 1) {
		colorScale--;
	}

	if (yoffset > 0.0 && colorScale < 0x100) {
		colorScale++;
	}*/
//}

class Wnd : public potent::Window {
private:
	std::jthread physicsThread;

public:
	void physicsUpdate() {
		while (potent::Window::windowRunning) {
			std::chrono::time_point start = std::chrono::high_resolution_clock::now();

			potent::GLOBAL_SCENE_HANDLER.fixedUpdateCurrent();

			if(potent::ENGINE_RUNTIME.fixedUpdatesPerSecond > 0)
			while (std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start).count() < 1.0 / (double)potent::ENGINE_RUNTIME.fixedUpdatesPerSecond) continue;

			potent::Window::engineFixedDeltaTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start).count();
		}
	}

	virtual void awake() override {
		potent::GLOBAL_SCENE_HANDLER.awakeCurrent();
	}

	virtual void start() override {
		glfwSwapInterval(0);

		potent::GLOBAL_SCENE_HANDLER.startCurrent();

		physicsThread = std::jthread(&Wnd::physicsUpdate, this);
		physicsThread.detach();

		// Gamma without control
		//glEnable(GL_FRAMEBUFFER_SRGB);
	}

	virtual void update() override {		
		glViewport(0, 0, windowWidth / resolutionScale, windowHeight / resolutionScale);

		potent::GLOBAL_SCENE_HANDLER.updateCurrent();
	}

	virtual void lateUpdate() override {
		potent::GLOBAL_SCENE_HANDLER.lateUpdateCurrent();
	}

} wnd;

int main(int argv, char** argc) {
	std::cout << std::fixed;

	potent::GLOBAL_SCENE_HANDLER.attachScene(&GLOBAL_MAIN_SCENE);
	potent::GLOBAL_SCENE_HANDLER.switchScene("MainScene");

	wnd.run(800, 600, "Window");

	return 0;
}