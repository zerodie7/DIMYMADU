#ifndef CUBE_H_
#define CUBE_H_

#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/glm.hpp>
#include <GL/glew.h> 

typedef struct _VertexColorNormal {

	_VertexColorNormal() {
	}

	_VertexColorNormal(glm::vec3 position, glm::vec3 colorl, glm::vec3 normal) {
		this->position = position;
		this->color = colorl;
		this->normal = normal;
	}

	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 normal;
} VertexColorNormals;

class Cube {
public:
	Cube(float length, float height, float width, bool wire);
	void init();
	void load();
	void render();
	virtual ~Cube();
private:
	std::vector<VertexColorNormals> vertex;
	std::vector<GLuint> index;
	float length;
	float height;
	float width;
	bool wire;

	GLuint VAO, VBO, EBO;
};

#endif
