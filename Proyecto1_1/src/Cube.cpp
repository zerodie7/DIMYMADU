/*
* Sphere.cpp
*
*  Created on: 06/09/2016
*      Author: rey
*/

#include "Headers/Cube.h"

Cube::Cube(float length, float height, float width, bool wire) :
length(length), height(height), width(width), wire(wire), VAO(0), VBO(0), EBO(0) {
}

Cube::~Cube() {

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDeleteBuffers(1, &EBO);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAO);

}

void Cube::init() {
	vertex.resize(24);
	index.resize(36);
	vertex =
	{
		// Face front
		{ glm::vec3(1, 1, 1), glm::vec3(1.0, 0.0, 0.0), glm::vec3(0,
		0, 1) }, { glm::vec3(-1, 1, 1), glm::vec3(0.0, 1.0,
		0.0), glm::vec3(0, 0, 1) }, { glm::vec3(-1, -1, 1),
		glm::vec3(0.0, 0.0, 1.0), glm::vec3(0, 0, 1) }, {
			glm::vec3(1, -1, 1), glm::vec3(1.0, 1.0, 0.0),
			glm::vec3(0, 0, 1) },
			// Face right
			{ glm::vec3(1, 1, -1), glm::vec3(0.0, 0.0, 1.0), glm::vec3(
			1, 0, 0) }, { glm::vec3(1, 1, 1), glm::vec3(1.0,
			0.0, 1.0), glm::vec3(1, 0, 0) }, { glm::vec3(1, -1,
			1), glm::vec3(0.0, 1.0, 0.0), glm::vec3(1, 0, 0) },
			{ glm::vec3(1, -1, -1), glm::vec3(0.0, 1.0, 1.0), glm::vec3(
			1, 0, 0) },
			// Face back
			{ glm::vec3(-1, 1, -1), glm::vec3(0.0, 1.0, 1.0), glm::vec3(
			0, 0, -1) }, { glm::vec3(1, 1, -1), glm::vec3(1.0,
			0.0, 1.0), glm::vec3(0, 0, -1) },
			{ glm::vec3(1, -1, -1), glm::vec3(1.0, 0.0, 1.0), glm::vec3(
			0, 0, -1) }, { glm::vec3(-1, -1, -1), glm::vec3(1.0,
			0.0, 1.0), glm::vec3(0, 0, -1) },
			//Face Left
			{ glm::vec3(-1, 1, 1), glm::vec3(1.0, 0.0, 0.0), glm::vec3(
			-1, 0, 0) }, { glm::vec3(-1, 1, -1), glm::vec3(1.0,
			1.0, 0.0), glm::vec3(-1, 0, 0) }, { glm::vec3(-1,
			-1, -1), glm::vec3(0.0, 0.0, 1.0), glm::vec3(-1, 0,
			0) }, { glm::vec3(-1, -1, 1), glm::vec3(0.0, 1.0,
			0.0), glm::vec3(-1, 0, 0) },
			// Face up
			{ glm::vec3(1, 1, 1), glm::vec3(1.0, 0.0, 1.0), glm::vec3(0,
			1, 0) }, { glm::vec3(1, 1, -1), glm::vec3(0.0, 0.0,
			1.0), glm::vec3(0, 1, 0) }, { glm::vec3(-1, 1, -1),
			glm::vec3(1.0, 1.0, .0), glm::vec3(0, 1, 0) }, {
				glm::vec3(-1, 1, 1), glm::vec3(0.0, 1.0, 1.0),
				glm::vec3(0, 1, 0) },
				// Face down
				{ glm::vec3(1, -1, 1), glm::vec3(1.0, 0.0, 1.0), glm::vec3(
				0, -1, 0) }, { glm::vec3(-1, -1, 1), glm::vec3(1.0,
				1.0, 1.0), glm::vec3(0, -1, 0) }, { glm::vec3(-1,
				-1, -1), glm::vec3(0.0, 1.0, 1.0), glm::vec3(0, -1,
				0) }, { glm::vec3(1, -1, -1), glm::vec3(1.0, 1.0,
				0.0), glm::vec3(0, -1, 0) }
	};

	index = { 0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4, 8, 9, 10, 10, 11,
		8, 12, 13, 14, 14, 15, 12, 16, 17, 18, 18, 19, 16, 20, 21, 22, 22,
		23, 20 };

}

void Cube::load() {

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertex.size() * (sizeof(glm::vec3) * 3),
		vertex.data(),
		GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(GLuint),
		index.data(),
		GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex[0]),
		(GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex[0]),
		(GLvoid*) sizeof(vertex[0].position));
	glEnableVertexAttribArray(1);
	// Normal Attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex[0]),
		(GLvoid*)(sizeof(vertex[0].color) + sizeof(vertex[0].position)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

}

void Cube::render() {

	glBindVertexArray(VAO);
	if (wire){
		glDrawElements(GL_LINES, index.size(), GL_UNSIGNED_INT, (GLvoid*)(sizeof(GLuint) * 0));
	}
	else{
		glDrawElements(GL_TRIANGLES, index.size(), GL_UNSIGNED_INT, (GLvoid*)(sizeof(GLuint) * 0));
	}
	
	glBindVertexArray(0);

}

