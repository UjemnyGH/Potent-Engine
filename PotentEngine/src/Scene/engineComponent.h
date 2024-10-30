#pragma once
#ifndef _POTENT_ENGINE_COMPONENT_
#define _POTENT_ENGINE_COMPONENT_

#include <string>
#include <vector>
#include "../Core/engineMath.h"
#include "../Renderer/engineMesh.h"
#include "engineComponentIds.h"

namespace potent {
	class Component {
	protected:
		std::uint32_t mComponentId = Component_Null;

	public:
		std::string componentName = "default";
		Component* pParent;
		std::vector<Component*> pChilds;
		Transform componentTransform;

		std::uint32_t getComponentId() { return mComponentId; }

		void transformChilds() {
			for (auto* child : pChilds) {
				*child->componentTransform() += componentTransform.GetTransform();
			}
		}
	};
}

#endif