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
		std::vector<Scene*> mScenePtrs;
		Scene* mCurrentScene;
		bool mAwakeCalled = false;
		bool mStartCalled = false;

	public:
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

		void updateCurrent() {
			if (mCurrentScene == nullptr) return;

			mCurrentScene->update();
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