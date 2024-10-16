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

	real calculateTriangleArea(RVec t1, RVec t2, RVec t3) {
		const real distTA = t1.Distance(t2);
		const real distTB = t2.Distance(t3);
		const real distTC = t1.Distance(t3);

		const real triangleSemi = (distTA + distTB + distTC) * 0.5f;
		const real triangleArea = triangleSemi * (triangleSemi - distTA) * (triangleSemi - distTB) * (triangleSemi - distTC);

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

		RVec intersectionPoint = rayPosition + rayDirection * distance;
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

		return intersectionPoint;
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