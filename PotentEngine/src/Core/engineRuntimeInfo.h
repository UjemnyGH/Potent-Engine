#pragma once
#ifndef _POTENT_ENGINE_RUNTIME_INFO_
#define _POTENT_ENGINE_RUNTIME_INFO_

#include <cstdint>
#include <vector>
#include "engineBuffers.h"
#include "../Renderer/engineRenderer.h"

namespace potent {
	struct RuntimeInfo {
		std::uint32_t fixedUpdatesPerSecond = 0;
		std::uint32_t postProcessLayersCount = 1;

		struct PostProcessLayerData {
			PostProcessRenderer renderer = {};
			PostProcessLayerData* pParentLayer = nullptr;

			std::vector<std::string> shaderLocation = {};
			std::vector<Shader> shader = {};

			std::string name = {};
		};

		std::vector<PostProcessLayerData> postProcessLayers = {};
	};

	RuntimeInfo ENGINE_RUNTIME;
}

#endif