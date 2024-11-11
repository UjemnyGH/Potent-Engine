#pragma once
#ifndef _POTENT_ENGINE_ASSET_
#define _POTENT_ENGINE_ASSET_

#include "engineAllComponents.h"
#include "engineComponent.h"

namespace potent {
	class Asset : public Component {
	public:
		std::vector<Component*> pChilds;

		Asset() : Component() {
			mComponentId = Component_Asset;
			componentName = "default_asset" + std::to_string(sComponentCounter);
		}

		void saveAsset(std::string assetPath) {
			FileFormat assetFile;

			for (auto component : pChilds) {

			}
		}

		void loadAsset(std::string assetPath) {

		}
	};
}

#endif