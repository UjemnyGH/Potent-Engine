#pragma once
#ifndef _POTENT_ENGINE_SCENE_
#define _POTENT_ENGINE_SCENE_

#include "../Core/engineWindow.h"
#include "../Renderer/engineRenderer.h"
#include <string>

namespace potent {
	class Scene {
	private:
		// Indexed by potent::LightType_X
		ShaderStorageBuffer mShaderLightDataStorage[3];

	public:
		std::vector<Component*> componentsPointer;
		bool awakend = false;
		bool started = false;
		
		std::string name = "default_scene";

		void addComponent(Component* pComponent) { componentsPointer.push_back(pComponent); }

		// Searches through all scene components to find desired component
		template<int id>
		Component* getComponent(std::string name) {
			for (auto component : componentsPointer) {
				if (component->getComponentId() == id && component->componentName == name) {
					return component;
				}
			}

			return nullptr;
		}

		// Search through childs to find component
		template<int id>
		Component* getComponentChilds(Component* pComponent, std::string name) {
			Component* result = nullptr;

			for (auto child : pComponent->pChilds) {
				if (child->getComponentId() == id && child->componentName == name) {
					result = child;

					break;
				}

				result = checkComponnetChilds<id>(child, name);

				if (result != nullptr) break;
			}

			return result;
		}

		// Searches through parents and childs to find component
		template<int id>
		Component* findComponent(std::string name) {
			Component* result = nullptr;

			for (auto component : componentsPointer) {
				Component* currentComponent = component;

				if (currentComponent->getComponentId() == id && currentComponent->componentName == name) {
					result = currentComponent;

					break;
				}

				result = checkComponnetChilds<id>(currentComponent, name);

				if (result != nullptr) break;
			}

			return result;
		}

		// Gets scene light storage buffer based on potent::LightType_X
		ShaderStorageBuffer* getLightStorage() { return mShaderLightDataStorage; }

		virtual void awake() {}
		virtual void start() {}
		virtual void update() {}
		virtual void lateUpdate() {}
		virtual void fixedUpdate() {}
	};

	class SceneHandler {
	private:
		const MeshRawData SQUARE_MESH = {
			{
				 1.0f,  1.0f, 0.0f,
				-1.0f,  1.0f, 0.0f,
				 1.0f, -1.0f, 0.0f,

				-1.0f,  1.0f, 0.0f,
				 1.0f, -1.0f, 0.0f,
				-1.0f, -1.0f, 0.0f
			},
			{
				0.0f, 0.0f, -1.0f,
				0.0f, 0.0f, -1.0f,
				0.0f, 0.0f, -1.0f,
				0.0f, 0.0f, -1.0f,
				0.0f, 0.0f, -1.0f,
				0.0f, 0.0f, -1.0f
			},
			{
				1.0f, 1.0f,
				0.0f, 1.0f,
				1.0f, 0.0f,

				0.0f, 1.0f,
				1.0f, 0.0f,
				0.0f, 0.0f,
			}
		};

		GeometryBuffer mSceneGeometryBuffer;
		RenderObjectData mSquareData;
		RenderData mSquareRender;
		Renderer mRenderer;
		std::array<Texture, 32> mPostProcessingTextures;
		bool mFirstTimeInitialization = true;

		std::vector<Scene*> mScenePtrs;
		Scene* mCurrentScene;
		bool mAwakeCalled = false;
		bool mStartCalled = false;

	public:
		bool postProcessLightRendering = true;

		SceneHandler() {
			mSquareData.name = "POST_PROCESS_SQUARE";
			mSquareData.makeModel(SQUARE_MESH);

			mSquareRender.name = "POST_PROCESS_RENDERING";
			mSquareRender.meshData.push_back(&mSquareData);

			mSquareRender.joinData();
		}

		void reinitializeSceneRenderer() {
			mFirstTimeInitialization = true;
		}

		Texture* getPostProcessingTexturePointer(int index) { return &mPostProcessingTextures[index]; }
		Renderer* getPostProcessingRendererPointer() { return &mRenderer; }
		RenderData* getPostProcessingSquareRenderDataPointer() { return &mSquareRender; }

		GeometryBuffer* getGeometryBufferPointer() { return &mSceneGeometryBuffer; }

		void attachScene(Scene* pScene) {
			mScenePtrs.push_back(pScene);
		}

		void switchScene(std::string name) {
			Scene* usedScenePtr = nullptr;

			for (Scene* scene : mScenePtrs) {
				if (scene->name == name) {
					usedScenePtr = scene;

					break;
				}
			}

			if (usedScenePtr == nullptr) {
				ENGINE_WARN("Scene named \"" << name << "\" could not exist!");
				
				return;
			}

			mCurrentScene = usedScenePtr;

			if (!mCurrentScene->awakend && mAwakeCalled) {
				mCurrentScene->awake();
				mCurrentScene->awakend = true;
			}

			if (!mCurrentScene->started && mStartCalled) {
				mCurrentScene->start();
				mCurrentScene->started = true;
			}
		}

		void switchScene(Scene* pScene) {
			if (pScene == nullptr) {
				ENGINE_WARN("Scene named \"" << pScene->name << "\" could not exist!");

				return;
			}

			mCurrentScene = pScene;

			if (!mCurrentScene->awakend && mAwakeCalled) {
				mCurrentScene->awake();
				mCurrentScene->awakend = true;
			}

			if (!mCurrentScene->started && mStartCalled) {
				mCurrentScene->start();
				mCurrentScene->started = true;
			}
		}

		void awakeCurrent() {
			if (mCurrentScene == nullptr) return;

			mCurrentScene->awake();
			mCurrentScene->awakend = true;
			mAwakeCalled = true;
		}

		void startCurrent() {
			if (mCurrentScene == nullptr) return;

			mCurrentScene->start();
			mCurrentScene->started = true;
			mStartCalled = true;
		}

		void updateCurrent(int width, int height, Shader* pPostProcessLightVertexShader, Shader* pPostProcessLightFragmentShader) {
			if (!pPostProcessLightFragmentShader || !pPostProcessLightVertexShader) {
				ENGINE_ERROR("Cannot load post processing shaders, exiting update loop immediately!");
			
				return;
			}
			
			if (mCurrentScene == nullptr) return;

			if (postProcessLightRendering) {

				mRenderer()->use();

				if (mRenderer()->linked) {
					glUniform4f(glGetUniformLocation(mRenderer()->id, "uViewPosition"), 0.0f, 0.0f, 0.0f, 0.0f);
				}

				mRenderer()->unuse();

				mSceneGeometryBuffer.initGraphicsBuffer(width, height);
				mSceneGeometryBuffer.startGraphicsBuffer();
			}

			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
			glClear(0x4100);

			mCurrentScene->update();

			if (postProcessLightRendering) {
				mSceneGeometryBuffer.endGraphicsBuffer();

				if (mFirstTimeInitialization) {
					mRenderer.attachShader(pPostProcessLightVertexShader);
					mRenderer.attachShader(pPostProcessLightFragmentShader);
					mRenderer.relinkShaderProgram();

					mRenderer.useMaxTexture();

					mSquareRender.bindData();
				
					mFirstTimeInitialization = false;
				}

				mPostProcessingTextures[0].name = "GBUFFER_POSITION_TEXTURE";
				mPostProcessingTextures[1].name = "GBUFFER_NORMAL_TEXTURE";
				mPostProcessingTextures[2].name = "GBUFFER_DIFFUSE_SPECULAR_TEXTURE";

				for (std::uint32_t i = 0; i < mSceneGeometryBuffer.USED_TEXTURES; i++) {
					mPostProcessingTextures[i].textureBuffer = mSceneGeometryBuffer.textures[i];
					mSquareRender.texturesPtr[i] = &mPostProcessingTextures[i];
				}

				mRenderer.render(&mSquareRender, RMat::Identity(), RMat::Identity());
			}
		}

		void lateUpdateCurrent() {
			if (mCurrentScene == nullptr) return;

			mCurrentScene->lateUpdate();
		}

		void fixedUpdateCurrent() {
			if (mCurrentScene == nullptr) return;

			mCurrentScene->fixedUpdate();
		}
	};

	SceneHandler GLOBAL_SCENE_HANDLER;
}

#endif