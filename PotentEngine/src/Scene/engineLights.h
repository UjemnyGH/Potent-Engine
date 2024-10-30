#pragma once
#ifndef _POTENT_ENGINE_LIGHTS_
#define _POTENT_ENGINE_LIGHTS_

#include "../Core/engineMath.h"
#include "engineComponent.h"

namespace potent {
#define DIRECTIONAL_LIGHT 0.0f
#define POINT_LIGHT 1.0f
#define SPOT_LIGHT 2.0f

	struct LightingData {
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
		// light type
		float lightType;
	};

	class Light : public Component {
	public:
		LightingData lightingData;

		Light() {
			mComponentId = Component_Light;
			componentName = "default_light";
		}
	};
}

#endif