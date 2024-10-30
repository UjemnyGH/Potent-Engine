#pragma once
#ifndef _POTENT_ENGINE_PARTICLE_
#define _POTENT_ENGINE_PARTICLE_

#include "../Core/engineMath.h"
#include "../Scene/engineComponent.h"

namespace potent {
	class Particle : public Component {
	public:
		RVec position = RVec();
		RVec lastPosition = RVec();
		RVec velocity = RVec();
		RVec acceleration = RVec();
		RVec accumulatedForce = RVec();
		real inverseMass = 1.0f;
		real damping = 0.995f;

		Particle() {
			mComponentId = Component_Particle;
			componentName = "default_particle";
		}

		void updateParticle(real dt) {
			if (dt <= 0.0f) { return; }

			lastPosition = position;
			position += velocity * dt;
			
			RVec calcAcceleration = acceleration + (accumulatedForce * inverseMass);

			velocity += calcAcceleration * dt;
			velocity *= damping;
			accumulatedForce = RVec();
		}

		void setMass(real mass) {
			inverseMass = 1.0 / mass;
		}

		real getMass() { return 1.0 / inverseMass; }

		void addForce(RVec force) {
			accumulatedForce += force;
		}
	};

	RVec particlesCollisionNormal(Particle* pPart1, Particle* pPart2) {
		RVec collisionNormal;

		if (pPart1 != nullptr) collisionNormal = pPart1->position;
		if (pPart2 != nullptr) collisionNormal -= pPart2->position;

		return collisionNormal.Normalize();
	}

	bool resolveCollisionSimple(real dt, RVec collisionNormal, Particle* pPart1) {
		real separatingVelocity = pPart1->velocity.Dot(collisionNormal);

		if(separatingVelocity < 0.0)

		return true;
	}

	bool resolveCollision(real dt, RVec collisionNormal, Particle* pPart1, Particle* pPart2 = nullptr, real restitution = 0.01f) {
		RVec relativeVelocity;

		if (pPart1 != nullptr) relativeVelocity = pPart1->velocity;
		if (pPart2 != nullptr) relativeVelocity -= pPart2->velocity;

		real separatingVelocity = relativeVelocity.Dot(collisionNormal);

		if (separatingVelocity > 0.0) return false;

		real newSeparatingVelocity = -separatingVelocity * restitution;

		RVec accelerationVelocity;

		if (pPart1 != nullptr) accelerationVelocity = pPart1->acceleration;
		if (pPart2 != nullptr) accelerationVelocity -= pPart2->acceleration;

		real accelerationSeparatingVelocity = accelerationVelocity.Dot(collisionNormal) * dt;

		if (accelerationSeparatingVelocity < 0.0) {
			newSeparatingVelocity += restitution * accelerationSeparatingVelocity;

			if (newSeparatingVelocity < 0.0) newSeparatingVelocity = 0.0;
		}

		real deltaVelocity = newSeparatingVelocity - separatingVelocity;

		real totalMass = 0.0;
		if (pPart1 != nullptr) totalMass = pPart1->inverseMass;
		if (pPart2 != nullptr) totalMass += pPart2->inverseMass;
		if (totalMass <= 0.0) return false;

		real impulse = deltaVelocity / totalMass;
		RVec impulseMass = collisionNormal * impulse;

		if (pPart1 != nullptr) pPart1->velocity += impulseMass * pPart1->inverseMass;
		if (pPart2 != nullptr) pPart2->velocity += impulseMass * -pPart2->inverseMass;

		return true;
	}

	bool resolvePenetration(real dt, real penetrationDepth, RVec collisionNormal, Particle* pPart1, Particle* pPart2 = nullptr) {
		if (penetrationDepth <= 0.0) return false;

		real totalMass = 0.0;

		if (pPart1 != nullptr) totalMass = pPart1->inverseMass;
		if (pPart2 != nullptr) totalMass += pPart2->inverseMass;

		if (totalMass <= 0.0) return false;

		RVec moveMass = collisionNormal * (penetrationDepth / totalMass);

		if (pPart1 != nullptr) pPart1->position += moveMass * pPart1->inverseMass;
		if (pPart2 != nullptr) pPart2->position += moveMass * pPart2->inverseMass;

		return true;
	}
}

#endif