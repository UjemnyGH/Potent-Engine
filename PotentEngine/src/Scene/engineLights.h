#pragma once
#ifndef _POTENT_ENGINE_LIGHTS_
#define _POTENT_ENGINE_LIGHTS_

#include "../Core/engineMath.h"
#include "../Core/engineFileSystem.h"
#include "engineComponent.h"

namespace potent {
	enum LightType {
		LightType_Directional = 0,
		LightType_Point = 1,
		LightType_Spot = 2
	};

	enum PostProcessFlags {
		PostProcess_DisableLighting = 0x1,
	};

	struct DirectionalLightData {
		RVec direction;
		RVec ambient;
		RVec diffuse;
		RVec specular;
	};

	struct PointLightData {
		// point and spot light
		RVec position;

		RVec ambient;
		RVec diffuse;
		RVec specular;

		// point/spot light
		float constant;
		float linear;
		float quadratic;
	};

	struct SpotLightData {
		RVec direction;
		// point and spot light
		RVec position;
		RVec ambient;
		RVec diffuse;
		RVec specular;

		// point/spot light
		float constant;
		float linear;
		float quadratic;
		// spotlight
		float cutOff;
		float outerCutOff;
	};

	class Light : public Component {
	public:
		DirectionalLightData directionalLightData;
		PointLightData pointLightData;
		SpotLightData spotLightData;
		int lightType;

		Light() : Component() {
			mComponentId = Component_Light;
			componentName = "default_light" + std::to_string(sComponentCounter);
		}
	};
}

#endif