/*
* collision.h
*
*  Created on: 13/10/2016
*      Author: rey
*/

#ifndef HEADERS_COLLISION_H_
#define HEADERS_COLLISION_H_

#include <float.h>
#include <algorithm>
#include "Mesh.h"
#include "definition.h"

typedef struct _AABB{
	glm::vec3 min;
	glm::vec3 max;
} AABB;

typedef struct _SBB{
	glm::vec3 center;
	float ratio;
} SBB;

AABB getAABB(glm::vec3 min, glm::vec3 max){
	AABB aabb;
	aabb.min = min;
	aabb.max = max;
	return aabb;
}

AABB getAABB(std::vector<Mesh> meshes) {
	AABB aabb;
	aabb.min.x = FLT_MAX;
	aabb.min.y = FLT_MAX;
	aabb.min.z = FLT_MAX;
	aabb.max.x = -FLT_MAX;
	aabb.max.y = -FLT_MAX;
	aabb.max.z = -FLT_MAX;

	for (int i = 0; i < meshes.size(); i++) {
		for (int j = 0; j < meshes[i].vertices.size(); j++) {
			if (meshes[i].vertices[j].Position.x < aabb.min.x)
				aabb.min.x = meshes[i].vertices[j].Position.x;
			if (meshes[i].vertices[j].Position.x > aabb.max.x)
				aabb.max.x = meshes[i].vertices[j].Position.x;
			if (meshes[i].vertices[j].Position.y < aabb.min.y)
				aabb.min.y = meshes[i].vertices[j].Position.y;
			if (meshes[i].vertices[j].Position.y > aabb.max.y)
				aabb.max.y = meshes[i].vertices[j].Position.y;
			if (meshes[i].vertices[j].Position.z < aabb.min.z)
				aabb.min.z = meshes[i].vertices[j].Position.z;
			if (meshes[i].vertices[j].Position.z > aabb.max.z)
				aabb.max.z = meshes[i].vertices[j].Position.z;
		}
	}
	return aabb;
}

SBB getSBB(std::vector<Mesh> meshes) {
	AABB aabb = getAABB(meshes);
	SBB sbb;
	sbb.center = glm::vec3((aabb.min.x + aabb.max.x) / 2.0f,
		(aabb.min.y + aabb.max.y) / 2.0f, (aabb.min.z + aabb.max.z) / 2.0f);
	sbb.ratio = sqrt(
		pow(aabb.min.x - aabb.max.x, 2) + pow(aabb.min.y - aabb.max.y, 2)
		+ pow(aabb.min.z - aabb.max.z, 2)) / 2.0f;
	return sbb;
}

bool raySphereIntersect(glm::vec3 orig, glm::vec3 dest, glm::vec3 dir,
	glm::vec3 vSphereCenter, float sphereRadius, float &t) {
	// Vector del Origen del rayo al centro de la esfera.
	glm::vec3 vDirToSphere = vSphereCenter - orig;

	// Distancia del origen al destino del rayo.
	float fLineLength = glm::distance(orig, dest);

	// Proyección escalar de vDirToSphere sobre la direccion del rayo.
	t = glm::dot(vDirToSphere, dir);

	glm::vec3 vClosestPoint;
	// Si la distancia proyectada del origen es menor o igual que cero
	// Significa que el punto mas cercano al centro es el origen.
	if (t <= 0.0f)
		vClosestPoint = orig;
	// Si la proyección escalar del origen es mayor a distancia del origen
	// al destino, el punto mas cercano es el destino.
	else if (t >= fLineLength)
		vClosestPoint = dest;
	// En caso contrario de calcula el punto sobre la linea usando t.
	else
		vClosestPoint = orig + dir * (t);

	// Se pureba si el punto mas cercao esta contenido en el radio de la esfera.
	return glm::distance(vSphereCenter, vClosestPoint) <= sphereRadius;
}

bool testSphereSphereIntersection(SBB sbb1, SBB sbb2) {
	float d = glm::distance(sbb1.center, sbb2.center);
	/*float d = sqrt(
	pow(sbb1.center.x - sbb2.center.x, 2)
	+ pow(sbb1.center.y - sbb2.center.y, 2)
	+ pow(sbb1.center.y - sbb2.center.y, 2));
	std::cout << "d:" << d << std::endl;
	std::cout << "sum:" << sbb1.ratio + sbb2.ratio << std::endl;*/
	if (d <= (sbb1.ratio + sbb2.ratio))
		return true;
	return false;
}

bool testCubeCubeIntersection(AABB a, AABB b) {
	
	return (
			a.min.x <= b.max.x && a.max.x >= b.min.x &&
			a.min.y <= b.max.y && a.max.y >= b.min.y &&
			a.min.z <= b.max.z && a.max.z >= b.min.z
		);
}

bool testSphereCubeIntersection(AABB aabb, SBB sbb) {
	if (
		sbb.center.x >= aabb.min.x && sbb.center.x <= aabb.max.x &&
		sbb.center.y >= aabb.min.y && sbb.center.y <= aabb.max.y &&
		sbb.center.z >= aabb.min.z && sbb.center.z <= aabb.max.z
		){
		return true;
	}
	else{
		float aux, d = 0.0f;
		//x
		if (sbb.center.x < aabb.min.x){
			aux = sbb.center.x - aabb.min.x;
			d += aux*aux;
		}
		else if (sbb.center.x > aabb.max.x){
			aux = sbb.center.x - aabb.max.x;
			d += aux*aux;
		}
		//y
		if (sbb.center.y < aabb.min.y){
			aux = sbb.center.y - aabb.min.y;
			d += aux*aux;
		}
		else if (sbb.center.y > aabb.max.y){
			aux = sbb.center.y - aabb.max.y;
			d += aux*aux;
		}
		//z
		if (sbb.center.z < aabb.min.z){
			aux = sbb.center.z - aabb.min.z;
			d += aux*aux;
		}
		else if (sbb.center.z > aabb.max.z){
			aux = sbb.center.z - aabb.max.z;
			d += aux*aux;
		}

		return (d <= (sbb.ratio*sbb.ratio));
	}
	
}

bool rayCubeIntersect(glm::vec3 orig, glm::vec3 dest, glm::vec3 dir,
	AABB aabb) {
	
	double tmin = -DBL_MAX, tmax = DBL_MAX;

	if (dir.x != 0.0) {
		double tx1 = (aabb.min.x - orig.x) / dir.x;
		double tx2 = (aabb.max.x - orig.x) / dir.x;

		tmin = std::max(tmin, std::min(tx1, tx2));
		tmax = std::min(tmax, std::max(tx1, tx2));
	}

	if (dir.y != 0.0) {
		double ty1 = (aabb.min.y - orig.y) / dir.y;
		double ty2 = (aabb.max.y - orig.y) / dir.y;

		tmin = std::max(tmin, std::min(ty1, ty2));
		tmax = std::min(tmax, std::max(ty1, ty2));
	}

	if (dir.z != 0.0) {
		double tz1 = (aabb.min.z - orig.z) / dir.z;
		double tz2 = (aabb.max.z - orig.z) / dir.z;

		tmin = std::max(tmin, std::min(tz1, tz2));
		tmax = std::min(tmax, std::max(tz1, tz2));
	}

	return tmax >= tmin;
}


#endif /* HEADERS_COLLISION_H_ */
