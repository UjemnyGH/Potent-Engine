#pragma once
#ifndef _POTENT_ENGINE_COMPONENT_
#define _POTENT_ENGINE_COMPONENT_

#include <string>
#include <vector>
#include "../Core/engineMath.h"
#include "../Renderer/engineMesh.h"
#include "engineComponentIds.h"
#include "../Core/engineFileSystem.h"

namespace potent {
	class Component {
	protected:
		std::uint32_t mComponentId = Component_Null;

	public:
		static std::uint32_t sComponentCounter;

		std::string componentName = "default";
		Component* pParent;
		std::vector<Component*> pChilds;
		RTransform componentTransform;

		Component() {
			sComponentCounter++;
		}

		void addDataToFile(FileFormat* pFile) {
			pFile->addNumber(componentName + "_ComponentId", mComponentId);
			pFile->addString(componentName + "_ParentName", pParent != nullptr ? pParent->componentName : "TREE_TOP_COMPONENT");
			pFile->addNumber(componentName + "_ParentId", pParent != nullptr ? pParent->getComponentId() : -1);
			pFile->addNumber(componentName + "_PositionXY", ((std::uint64_t)(*reinterpret_cast<std::uint32_t*>(&componentTransform.GetPosition()->x))) << 32 | (std::uint64_t)(*reinterpret_cast<std::uint32_t*>(&componentTransform.GetPosition()->y)));
			pFile->addNumber(componentName + "_PositionZScaleX", ((std::uint64_t)(*reinterpret_cast<std::uint32_t*>(&componentTransform.GetPosition()->z))) << 32 | (std::uint64_t)(*reinterpret_cast<std::uint32_t*>(&componentTransform.GetScale()->x)));
			pFile->addNumber(componentName + "_ScaleYZ", ((std::uint64_t)(*reinterpret_cast<std::uint32_t*>(&componentTransform.GetScale()->y))) << 32 | (std::uint64_t)(*reinterpret_cast<std::uint32_t*>(&componentTransform.GetScale()->z)));
			pFile->addNumber(componentName + "_RotationXY", ((std::uint64_t)(*reinterpret_cast<std::uint32_t*>(&componentTransform.GetRotation()->x))) << 32 | (std::uint64_t)(*reinterpret_cast<std::uint32_t*>(&componentTransform.GetRotation()->y)));
			pFile->addNumber(componentName + "_RotationZ", (*reinterpret_cast<std::uint32_t*>(&componentTransform.GetRotation()->z)));
		}

		std::uint32_t getComponentId() { return mComponentId; }

		template<int id>
		Component* getChildComponent(std::string name) {
			for (auto child : pChilds) {
				if (child->getComponentId() == id && child->componentName == name) {
					return child;
				}
			}

			return nullptr;
		}

		void transformChilds() {
			for (auto* child : pChilds) {
				*child->componentTransform() += componentTransform.GetTransform();
			}
		}
	};

	std::uint32_t Component::sComponentCounter = 0;
}

#endif