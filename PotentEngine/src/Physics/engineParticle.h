#pragma once
#ifndef _POTENT_ENGINE_PARTICLE_
#define _POTENT_ENGINE_PARTICLE_

#include "../Core/engineMath.h"
#include "../Scene/engineComponent.h"

namespace potent {
	// Particle class for handling mass aggregate particle system
	class Particle : public Component {
	public:
		DVec position = DVec();
		DVec lastPosition = DVec();
		DVec velocity = DVec();
		DVec acceleration = DVec();
		DVec accumulatedForce = DVec();
		double inverseMass = 1.0f;
		double damping = 0.9991f;

		Particle() : Component() {
			mComponentId = Component_Particle;
			componentName = "default_particle" + std::to_string(sComponentCounter);
		}

		// Updates particle variables
		void updateParticle(double dt) {
			if (dt <= 0.0f) { return; }

			lastPosition = position;
			position += velocity * dt;
			
			DVec calcAcceleration = acceleration + (accumulatedForce * inverseMass);

			velocity += calcAcceleration * dt;
			velocity *= damping;
			accumulatedForce = DVec();
		}

		// Set mass of particle
		void setMass(double mass) {
			inverseMass = 1.0 / mass;
		}

		// Returns mass of particle
		double getMass() { return 1.0 / inverseMass; }

		// Adds force vector to particle accumulated force
		void addForce(DVec force) {
			accumulatedForce += force * getMass();
		}
	};

	// Base class for particle force generator
	class ParticleForceGeneratorBase {
	public:
		virtual void generateForce(Particle* pPart, double dt) {}
	};

	class ParticleGravityGenerator : public ParticleForceGeneratorBase {
	public:
		DVec gravity = DVec(0.0, -9.81, 0.0);

		virtual void generateForce(Particle* pPart, double dt) override {
			if (pPart->inverseMass == 0.0) return;

			pPart->addForce(gravity);
		}
	};

	class ParticleDragGenerator : public ParticleForceGeneratorBase {
	public:
		double linear;
		double quadratic;

		virtual void generateForce(Particle* pPart, double dt) override {
			if (pPart->inverseMass == 0.0 || pPart->velocity == DVec()) return;

			DVec dragDir = pPart->velocity.Normalize();
			double length = pPart->velocity.Length();
			double dragCoefficient = (linear * length + quadratic * (length * length));

			pPart->addForce(dragDir * -dragCoefficient);
		}
	};

	class ParticleSpringGenerator : public ParticleForceGeneratorBase {
	public:
		double linear;
		double restLength;
		Particle* pOtherParticle;

		virtual void generateForce(Particle* pPart, double dt) override {
			DVec force = pPart->position - pOtherParticle->position;

			if (pPart->inverseMass == 0.0 || force == DVec()) return;

			double length = (abs(force.Length() - restLength)) * linear;

			pPart->addForce(force.Normalize() * -length);
		}
	};

	class ParticleSpringAnchorGenerator : public ParticleForceGeneratorBase {
	public:
		double linear;
		double restLength;
		DVec pAnchorPoint;

		virtual void generateForce(Particle* pPart, double dt) override {
			DVec force = pPart->position - pAnchorPoint;

			if (pPart->inverseMass == 0.0 || force == DVec()) return;

			double length = (abs(force.Length() - restLength)) * linear;

			pPart->addForce(force.Normalize() * -length);
		}
	};

	class ParticlePullbackSpringGenerator : public ParticleForceGeneratorBase {
	public:
		double linear;
		double restLength;
		Particle* pOtherParticle;

		virtual void generateForce(Particle* pPart, double dt) override {
			DVec force = pPart->position - pOtherParticle->position;

			if (pPart->inverseMass == 0.0 || force == DVec() || force.Length() <= restLength) return;

			double length = (abs(force.Length() - restLength)) * linear;

			pPart->addForce(force.Normalize() * -length);
		}
	};

	class ParticleBuoyancyGenerator : public ParticleForceGeneratorBase {
	public:
		double maxDepth;
		double volume;
		double waterHeight;
		double liquidDensity;

		virtual void generateForce(Particle* pPart, double dt) override {
			double depth = pPart->position.y;

			if (depth >= waterHeight + maxDepth) return;

			DVec force = DVec();

			if (depth <= waterHeight - maxDepth) {
				force.y = liquidDensity * volume;
			}
			else {
				force.y = liquidDensity * volume * (depth - maxDepth - waterHeight) / 2 * maxDepth;
			}

			pPart->addForce(force);
		}
	};

	class ParticleForceSolver {
	private:
		struct ParticleSolveGenerator {
			Particle* particlePointer;
			ParticleForceGeneratorBase* particleForceGeneratorPointer;

			bool operator==(ParticleSolveGenerator const& partSolveGen) {
				return (particlePointer == partSolveGen.particlePointer && particleForceGeneratorPointer == partSolveGen.particleForceGeneratorPointer);
			}
		};

		std::vector<ParticleSolveGenerator> mSolveParticles;

	public:
		void addParticle(Particle* pParticle, ParticleForceGeneratorBase* pForceGenerator) {
			ParticleSolveGenerator generator = { .particlePointer = pParticle, .particleForceGeneratorPointer = pForceGenerator };

			mSolveParticles.push_back(generator);
		}

		void removeParticle(Particle* pParticle, ParticleForceGeneratorBase* pForceGenerator) {
			ParticleSolveGenerator generator = { .particlePointer = pParticle, .particleForceGeneratorPointer = pForceGenerator };

			auto iter = std::find(mSolveParticles.begin(), mSolveParticles.end(), generator);

			mSolveParticles.erase(iter);
		}

		void clearParticles() {
			mSolveParticles.clear();
		}

		void solveParticleForces(double dt) {
			for (std::vector<ParticleSolveGenerator>::iterator i = mSolveParticles.begin(); i != mSolveParticles.end(); i++) {
				i->particleForceGeneratorPointer->generateForce(i->particlePointer, dt);
			}
		}
	};

	DVec particlesCollisionNormal(Particle* pPart1, Particle* pPart2) {
		DVec collisionNormal;

		if (pPart1 != nullptr) collisionNormal = pPart1->position;
		if (pPart2 != nullptr) collisionNormal -= pPart2->position;

		return collisionNormal.Normalize();
	}

	/*bool resolveCollisionSimple(double dt, DVec collisionNormal, Particle* pPart1) {
		double separatingVelocity = pPart1->velocity.Dot(collisionNormal);

		if (separatingVelocity < 0.0) return false;

		double impulse = separatingVelocity / pPart1->inverseMass;
		DVec impulseMass = collisionNormal * impulse;

		//pPart1->velocity += impulseMass * pPart1->inverseMass;
		//pPart1->addForce(impulseMass * pPart1->inverseMass);
		pPart1->addForce(collisionNormal * pPart1->velocity.Length());

		return true;
	}*/

	bool resolveCollision(double dt, DVec collisionNormal, Particle* pPart1, Particle* pPart2 = nullptr, double restitution = 0.0) {
		DVec relativeVelocity;

		if (pPart1 != nullptr) relativeVelocity = pPart1->velocity;
		if (pPart2 != nullptr) relativeVelocity -= pPart2->velocity;

		double separatingVelocity = relativeVelocity.Dot(collisionNormal);

		if (separatingVelocity > 0.0) return false;

		double newSeparatingVelocity = -separatingVelocity * restitution;

		DVec accelerationVelocity;

		if (pPart1 != nullptr) accelerationVelocity = pPart1->acceleration;
		if (pPart2 != nullptr) accelerationVelocity -= pPart2->acceleration;

		double accelerationSeparatingVelocity = accelerationVelocity.Dot(collisionNormal) * dt;

		if (accelerationSeparatingVelocity < 0.0) {
			newSeparatingVelocity += restitution * accelerationSeparatingVelocity;

			if (newSeparatingVelocity < 0.0) newSeparatingVelocity = 0.0;
		}

		double deltaVelocity = newSeparatingVelocity - separatingVelocity;

		double totalMass = 0.0;
		if (pPart1 != nullptr) totalMass = pPart1->inverseMass;
		if (pPart2 != nullptr) totalMass += pPart2->inverseMass;
		if (totalMass <= 0.0) return false;

		double impulse = deltaVelocity / totalMass;
		DVec impulseMass = collisionNormal * impulse;

		if (pPart1 != nullptr) pPart1->velocity += impulseMass * pPart1->inverseMass;
		if (pPart2 != nullptr) pPart2->velocity += impulseMass * -pPart2->inverseMass;

		return true;
	}

	bool resolvePenetration(double dt, double penetrationDepth, DVec collisionNormal, Particle* pPart1, Particle* pPart2 = nullptr) {
		if (penetrationDepth <= 0.0) return false;

		double totalMass = 0.0;

		if (pPart1 != nullptr) totalMass = pPart1->inverseMass;
		if (pPart2 != nullptr) totalMass += pPart2->inverseMass;

		if (totalMass <= 0.0) return false;

		DVec moveMass = collisionNormal * (-penetrationDepth / totalMass);

		if (pPart1 != nullptr) pPart1->position += moveMass * pPart1->inverseMass;
		if (pPart2 != nullptr) pPart2->position += moveMass * pPart2->inverseMass;

		return true;
	}
}

#endif