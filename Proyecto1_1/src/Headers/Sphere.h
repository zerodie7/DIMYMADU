/*
* Sphere.h
*
*  Created on: 06/09/2016
*      Author: rey
*/

#ifndef SPHERE_H_
#define SPHERE_H_

#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/glm.hpp>
#include <GL/glew.h> 

typedef struct _VertexColorText {

	_VertexColorText() {
	}

	_VertexColorText(glm::vec3 position, glm::vec3 color, glm::vec2 texCoord) {
		this->position = position;
		this->color = color;
		this->texCoord = texCoord;
	}

	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 texCoord;
} VertexColorText;

class Sphere {
public:
	Sphere(float ratio, int slices, int stacks, bool wire);
	void init();
	void load();
	void render();
	virtual ~Sphere();
private:
	std::vector<VertexColorText> vertex;
	std::vector<GLuint> index;
	float ratio;
	int slices;
	int stacks;
	bool wire;

	GLuint VAO, VBO, EBO;
};

#endif /* SPHERE_H_ */
