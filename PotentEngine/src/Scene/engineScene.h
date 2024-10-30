#pragma once
#ifndef _POTENT_ENGINE_SCENE_
#define _POTENT_ENGINE_SCENE_

#include "../Core/engineWindow.h"
#include "../Renderer/engineRenderer.h"
#include <string>

namespace potent {
	class Scene {
	private:
		ShaderStorageBuffer mShaderLightDataStorage;

	public:
		bool awakend = false;
		bool started = false;
		
		std::string name = "default_scene";

		ShaderStorageBuffer* getShaderLightStorage() { return &mShaderLightDataStorage; }

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
		SceneHandler() {
			mSquareData.name = "POST_PROCESS_SQUARE";
			mSquareData.makeModel(SQUARE_MESH);

			mSquareRender.name = "POST_PROCESS_RENDERING";
			mSquareRender.meshData.push_back(&mSquareData);

			mSquareRender.joinData();
		}

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

			mRenderer()->use();

			glUniform4f(glGetUniformLocation(mRenderer()->id, "uViewPosition"), 0.0f, 0.0f, 0.0f, 0.0f);

			mRenderer()->unuse();

			mSceneGeometryBuffer.initGraphicsBuffer(width, height);
			mSceneGeometryBuffer.startGraphicsBuffer();

			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(0x4100);

			mCurrentScene->update();

			mSceneGeometryBuffer.endGraphicsBuffer();

			glClear(0x4100);

			// TODO: Render

			if (mFirstTimeInitialization) {
				mRenderer.attachShader(pPostProcessLightVertexShader);
				mRenderer.attachShader(pPostProcessLightFragmentShader);
				mRenderer.relinkShaderProgram();

				mRenderer.useMaxTexture();

				mFirstTimeInitialization = false;
			}

			mSquareRender.bindData();

			mPostProcessingTextures[0].name = "GBUFFER_POSITION_TEXTURE";
			mPostProcessingTextures[1].name = "GBUFFER_NORMAL_TEXTURE";
			mPostProcessingTextures[2].name = "GBUFFER_DIFFUSE_SPECULAR_TEXTURE";

			for (std::uint32_t i = 0; i < mSceneGeometryBuffer.USED_TEXTURES; i++) {
				mPostProcessingTextures[i].textureBuffer = mSceneGeometryBuffer.textures[i];
				mSquareRender.texturesPtr[i] = &mPostProcessingTextures[i];
			}

			mRenderer.render(&mSquareRender, RMat::Identity(), RMat::Identity());
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