#pragma once
#ifndef _POTENT_ENGINE_COLLISIONS_
#define _POTENT_ENGINE_COLLISIONS_

#include "../Core/engineMath.h"

namespace potent {
	struct Sphere {
		RVec position;
		real radius;
	};

	struct AABox {
		RVec position;
		RVec size;
	};

	/*struct Cylinder {
		RVec center;
		RVec up;
		real radius;
	};*/

	struct CollisionMesh {
		RVec origin;
		RVec size;
		std::vector<real> vertices;
		RTransform *pTransform;
	};

	const CollisionMesh NULL_COLLISION_MESH = {
		RVec(0.0f),
		RVec(0.0f),
		{0.0f},
		nullptr
	};

	/*
	Big meshed collision detection ( REMEMBER THAT THIS IS SLOW AND SHOULD BE CALCULATED BEFORE USAGE )
	*/
	class BigMeshCollision {
	public:
		std::vector<CollisionMesh> collisionMeshes;

		/*
		Make list of smmaller collision meshes with distinguishable by bounding box, with transform copyied from objectData
		*/
		void boundCollisionFromRenderObject(RenderObjectData & objectData, std::uint32_t size = 4, real margin = 10.0) noexcept {
			// Convert original mesh to collision mesh
			CollisionMesh originalMesh = {
				RVec(0.0f),
				RVec(1.0f),
				objectData.meshData.vertices,
				&objectData.transform
			};

			// Add original mesh to result as we need to read data from somewhere
			std::vector<CollisionMesh> result = { originalMesh };

			do {
				std::vector<CollisionMesh> temp;

				// get all results (begins with only 1)
				for (auto t : result) {
					RVec min = RVec(FLT_MAX, FLT_MAX, FLT_MAX, 0.0f);
					RVec max = RVec(FLT_MIN, FLT_MIN, FLT_MIN, 0.0f);

					// Calculate min and max of bounding box
					for (std::uint32_t i = 0; i < t.vertices.size() / 3; i++) {
						real* const dataPtr = &t.vertices[i * 3];

						RVec point(dataPtr[0], dataPtr[1], dataPtr[2]);

						if (point.x < min.x) {
							min.x = point.x;
						}

						if (point.y < min.y) {
							min.y = point.y;
						}

						if (point.z < min.z) {
							min.z = point.z;
						}

						if (point.x > max.x) {
							max.x = point.x;
						}

						if (point.y > max.y) {
							max.y = point.y;
						}

						if (point.z > max.z) {
							max.z = point.z;
						}
					}

					// Calculate model size and origin
					RVec size = (max - min).Abs() / 2.0;
					RVec origin = min + size;

					// Calculate 8 corners of mesh bounding box
					RVec corners[8] = {
						RVec(max.x, min.y, max.z),
						RVec(min.x, min.y, min.z),
						RVec(min.x, min.y, max.z),
						RVec(max.x, min.y, min.z),

						RVec(max.x, max.y, max.z),
						RVec(max.x, max.y, min.z),
						RVec(min.x, max.y, max.z),
						RVec(min.x, max.y, min.z)
					};

					// Run checks for all corners
					for (int cornerIterator = 0; cornerIterator < sizeof(corners) / sizeof(corners[0]); cornerIterator++) {
						// Calculate smaller bounding box size and origin 
						RVec currentCorner = corners[cornerIterator];
						RVec currentSize = (origin - currentCorner).Abs() / 2.0;
						RVec currentOrigin = currentCorner + currentSize;

						// If length to corner is shorter than to origin subtract current size
						if (currentCorner.Length() < origin.Length()) {
							currentOrigin = currentCorner - currentSize;
						}

						// Calculate current min and max values
						RVec currentMin = currentOrigin - currentSize;
						RVec currentMax = currentOrigin + currentSize;

						// Push new collision mesh object with proper setup
						temp.push_back(CollisionMesh());
						temp[temp.size() - 1].origin = currentOrigin;
						temp[temp.size() - 1].size = currentSize;
						temp[temp.size() - 1].pTransform = &objectData.transform;
						// Ease up some writing (and spell errors)
						std::vector<real>* tempMeshVertices = &temp[temp.size() - 1].vertices;

						for (std::uint32_t i = 0; i < t.vertices.size() / 3; i++) {
							real* const ptr = &t.vertices[i * 3];

							RVec point(ptr[0], ptr[1], ptr[2]);

							// If point is in currently calculated mesh bounding box, add it
							if (currentMin.x - margin <= point.x && currentMin.y - margin <= point.y && currentMin.z - margin <= point.z && currentMax.x + margin >= point.x && currentMax.y + margin >= point.y && currentMax.z + margin >= point.z) {
								tempMeshVertices->push_back(point.x);
								tempMeshVertices->push_back(point.y);
								tempMeshVertices->push_back(point.z);
							}
						}

						// Clear unused memory
						if (!temp.empty() && temp[temp.size() - 1].vertices.empty()) {
							temp.erase(temp.end());
						}
					} // End of corner checks

				} // End of this long for loop

				// Copy dara from temporary to result vector
				result.clear();
				std::copy(temp.begin(), temp.end(), std::back_inserter(result));
				// In theory useless, but still to make sure temp is clear
				temp.clear();

			} while (result.size() < size); // End of while loop

			// Clear current mesh collision mesh data
			collisionMeshes.clear();

			// And copy result to collision mesh data
			std::copy(result.begin(), result.end(), std::back_inserter(collisionMeshes));
		}

		const CollisionMesh getCollisionMeshPointer(RVec const & point, const real margin = 10.0) noexcept {
			for (std::uint32_t i = 0; i < collisionMeshes.size(); i++) {
				RVec min = (collisionMeshes[i].pTransform != nullptr ? collisionMeshes[i].pTransform->GetTransform() : RMat::Identity()) * (collisionMeshes[i].origin - collisionMeshes[i].size);
				RVec max = (collisionMeshes[i].pTransform != nullptr ? collisionMeshes[i].pTransform->GetTransform() : RMat::Identity()) * (collisionMeshes[i].origin + collisionMeshes[i].size);

				bool inBoxX = min.x - margin <= point.x && max.x + margin >= point.x;
				bool inBoxY = min.y - margin <= point.y && max.y + margin >= point.y;
				bool inBoxZ = min.z - margin <= point.z && max.z + margin >= point.z;

				if (inBoxX && inBoxY && inBoxZ) {
					return collisionMeshes[i];
				}
			}

			return NULL_COLLISION_MESH;
		}
	};

	real calculateTriangleArea(RVec t1, RVec t2, RVec t3) {
		const real distTA = t1.Distance(t2);
		const real distTB = t2.Distance(t3);
		const real distTC = t1.Distance(t3);

		//ENGINE_INFO("A: " << distTA << " B: " << distTB << " C: " << distTC);

		const real triangleSemi = (distTA + distTB + distTC) * 0.5f;
		const real triangleArea = (triangleSemi - distTA) * (triangleSemi - distTB) * (triangleSemi - distTC);

		return triangleArea;
	}

	RVec lineTriangleIntersectionPoint(RVec rayPosition, RVec rayDirection, RVec t1, RVec t2, RVec t3) {
		// Normals needs to be NOT normalized
		RVec normal = (t2 - t1).Cross(t3 - t1);

		// nx * dx + ny * dy + nz * dz
		real denominator = normal.Dot(rayDirection);

		// Denominator needs to be higher than 0
		if (abs(denominator) < 1e-6) {
			return RVec(0.0f, 0.0f, 0.0f, 1.0f);
		}

		RVec rayPositionToT1 = rayPosition - t1;
		real distance = normal.Dot(rayPositionToT1) / denominator;

		if (distance < 0) {
			return RVec(0.0f, 0.0f, 0.0f, 1.0f);
		}

		RVec intersectionPoint = rayPosition + rayDirection.Normalize() * distance;
		intersectionPoint.w = 0.0f;

		/*RVec intersectionPointToT1 = intersectionPoint - t1;
		real area123 = normal.Dot(normal);
		real area12p = (t2 - t1).Cross(intersectionPointToT1).Dot(normal);
		real area13p = (t3 - t1).Cross(intersectionPointToT1).Dot(normal);*/

		const real triangleArea = calculateTriangleArea(t1, t2, t3);
		const real triangleAreaA = calculateTriangleArea(intersectionPoint, t2, t3);
		const real triangleAreaB = calculateTriangleArea(t1, intersectionPoint, t3);
		const real triangleAreaC = calculateTriangleArea(t1, t2, intersectionPoint);

		//if (area12p < 0.0f || area13p < 0.0f || (area12p + area13p) > area123) {
		if(triangleAreaA + triangleAreaB + triangleAreaC > triangleArea) {
			intersectionPoint.w = 1.0f;
		}

		//ENGINE_INFO("[AREA, M, A, B, C] " << (intersectionPoint.w == 0.0f ? "intersecting" : "no intersect") << triangleArea << " " << triangleAreaA << " " << triangleAreaB << " " << triangleAreaC);

		return intersectionPoint;
	}

	bool triangleTriangleIntersection(RVec t1, RVec t2, RVec t3, RVec s1, RVec s2, RVec s3) {
		RVec pointA = lineTriangleIntersectionPoint(t1, (t2 - t1).Normalize(), s1, s2, s3);
		RVec pointB = lineTriangleIntersectionPoint(t2, (t3 - t2).Normalize(), s1, s2, s3);
		RVec pointC = lineTriangleIntersectionPoint(t3, (t1 - t3).Normalize(), s1, s2, s3);

		const real distA = pointA.Distance(t1);
		const real distB = pointB.Distance(t2);
		const real distC = pointC.Distance(t3);

		if (pointA.w != 1.0f || pointB.w != 1.0f || pointC.w != 1.0f) {
			if (t1.Distance(t2) <= distA || t2.Distance(t3) <= distB || t1.Distance(t3) <= distC) {
				return true;
			}
		}

		return false;
	}

	RVec pointOnPlane(RVec point, RVec t1, RVec t2, RVec t3) {
		/*
		 * Quick visualisation
		 * p = point
		 * imagine lines from p to t1, t2 and t3
		 * p to t1 = distPointTA
		 * p to t2 = distPointTB
		 * p to t3 = distPointTC
		 * 
		 * 			t1
		 * 			|\
		 * 			| \ distTA
		 * 			|  \
		 * 			|   \
		 *   distTC	| p / t2
		 *			|  /
		 * 			| / distTB
		 * 			|/
		 * 			t3
		 * 
		 * Remember that all distances need to be squared to represent true distance, but sqrt is to cpu expensive
		 */

		RVec normal = RVec::PlaneNormal(t1, t2, t3);
		RVec planePoint = ((t1 - point).Cross(normal)).Cross(normal) + t1;

		/*const real distTA = t1.Distance(t2);
		const real distTB = t2.Distance(t3);
		const real distTC = t1.Distance(t3);
		
		const real distPointTA = t1.Distance(planePoint);
		const real distPointTB = t2.Distance(planePoint);
		const real distPointTC = t3.Distance(planePoint);

		const real triangleSemi = (distTA + distTB + distTC) * 0.5f;
		const real triangleSemiPA = (distPointTA + distPointTB + distTA) * 0.5f;
		const real triangleSemiPB = (distPointTB + distPointTC + distTB) * 0.5f;
		const real triangleSemiPC = (distPointTC + distPointTA + distTC) * 0.5f;

		const real _triangleArea = triangleSemi * (triangleSemi - distTA) * (triangleSemi - distTB) * (triangleSemi - distTC);
		const real _triangleAreaA = triangleSemiPA * (triangleSemiPA - distPointTA) * (triangleSemiPA - distPointTB) * (triangleSemiPA - distTA);
		const real _triangleAreaB = triangleSemiPB * (triangleSemiPB - distPointTB) * (triangleSemiPB - distPointTC) * (triangleSemiPB - distTB);
		const real _triangleAreaC = triangleSemiPC * (triangleSemiPC - distPointTC) * (triangleSemiPC - distPointTA) * (triangleSemiPC - distTC);*/

		const real triangleArea = calculateTriangleArea(t1, t2, t3);
		const real triangleAreaA = calculateTriangleArea(planePoint, t2, t3);
		const real triangleAreaB = calculateTriangleArea(t1, planePoint, t3);
		const real triangleAreaC = calculateTriangleArea(t1, t2, planePoint);

		if (triangleAreaA + triangleAreaB + triangleAreaC <= triangleArea) {
			planePoint.w = 0.0f;
		}
		else {
			planePoint.w = 1.0f;
		}
		
		return planePoint;
	}

	bool sphereSphereCollision(Sphere sp1, Sphere sp2) {
		return sp1.position.Distance(sp2.position) <= sp1.radius + sp2.radius;
	}

	bool pointSphereCollision(Sphere sp, RVec point) {
		return sp.position.Distance(point) <= sp.radius;
	}

	bool boxBoxCollision(AABox b1, AABox b2) {
		bool collisionX = b1.position.x + b1.size.x <= b2.position.x - b2.size.x && b2.position.x + b2.size.x >= b1.position.x - b1.size.x;
		bool collisionY = b1.position.y + b1.size.y <= b2.position.y - b2.size.y && b2.position.y + b2.size.y >= b1.position.y - b1.size.y;
		bool collisionZ = b1.position.z + b1.size.z <= b2.position.z - b2.size.z && b2.position.z + b2.size.z >= b1.position.z - b1.size.z;

		return collisionX && collisionY && collisionZ;
	}

	bool pointBoxCollision(AABox b, RVec point) {
		bool collisionX = point.x >= b.position.x - b.size.x && point.x <= b.position.x + b.size.x;
		bool collisionY = point.y >= b.position.y - b.size.y && point.y <= b.position.y + b.size.y;
		bool collisionZ = point.z >= b.position.z - b.size.z && point.z <= b.position.z + b.size.z;

		return collisionX && collisionY && collisionZ;
	}

	/*bool CylinderCollision(Cylinder c1, Cylinder c2) {

	}*/
}

#endif