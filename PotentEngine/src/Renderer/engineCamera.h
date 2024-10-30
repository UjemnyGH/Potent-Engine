#pragma once
#ifndef _POTENT_ENGINE_CAMERA_
#define _POTENT_ENGINE_CAMERA_

#include "../Core/engineMath.h"
#include "../Core/engineCore.h"
#include "../Scene/engineComponent.h"
#include <algorithm>

namespace potent {
	class Camera : public Component {
	private:
		RMat mViewMatrix = RMat::Identity();
		RMat mProjectionMatrix = RMat::Identity();
		int mLastOffX, mLastOffY;

	public:
		RVec position, front, right, cameraDirection, up = RVec(0.0f, 1.0f);
		real pitch, yaw;
		real lockPitchMin = -89.9f;
		real lockPitchMax = 89.9f;
		real fieldOfView = 90.0f;
		real zNear = 0.001f;
		real zFar = 100.0f;

		Camera() { 
			mComponentId = Component_Camera; 
			componentName = "default_camera";
		}

		RMat getViewMatrix() { return mViewMatrix; }
		RMat getProjectionMatrix() { return mProjectionMatrix; }

		void cameraViewMatrix(double mouseX, double mouseY) {
			float offX = mouseX - mLastOffX;
			float offY = mLastOffY - mouseY;

			mLastOffX = mouseX;
			mLastOffY = mouseY;

			offX *= 0.005f;
			offY *= 0.005f;

			yaw += offX;
			pitch += offY;

			pitch = std::clamp(pitch, potent::ToRadians(lockPitchMin), potent::ToRadians(lockPitchMax));

			cameraDirection.x = cos(pitch) * cos(yaw);
			cameraDirection.y = sin(pitch);
			cameraDirection.z = cos(pitch) * sin(yaw);

			right = cameraDirection.Normalize().Cross(up).Normalize();
			front = right.Cross(up).Normalize().Negate();
			cameraDirection = cameraDirection.Normalize();
			right.w = 0.0f;
		}

		void updateViewMatrix() {
			mViewMatrix = RMat::LookAt(position + cameraDirection, position, up);
		}

		void cameraProjectionPerspective(std::uint32_t width, std::uint32_t height) {
			mProjectionMatrix = RMat::PerspectiveFOV(ToRadians(fieldOfView), (real)width, (real)height, zNear, zFar);
		}

		void cameraProjectionOrthographic(std::uint32_t width, std::uint32_t height) {
			mProjectionMatrix = RMat::OrthographicSymmertical((real)width / (real)height, (real)height / (real)width, zNear, zFar);
		}
	};
}

#endif