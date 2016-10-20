/*
 * GLApplication.cpp
 *
 *  Created on: 07/08/2016
 *      Author: rey
 */

#include "Headers/GLApplication.h"

GLuint VAO, VBO, EBO;
Sphere sp(1.5, 50, 50, MODEL_MODE::VERTEX_LIGHT_TEXTURE);
Sphere sp2(1.5, 50, 50, MODEL_MODE::VERTEX_LIGHT_TEXTURE);
Shader lightingShader;
Shader lampShader;
Shader shader;
Shader sistemascoordenados;
Texture textureDifuse(GL_TEXTURE_2D, "Textures/SUN.jpg");
Texture textureSpecular(GL_TEXTURE_2D, "Textures/SUN_SPECULAR.JPG");

struct VertexTexture {
	glm::vec3 m_Pos;
	glm::vec2 m_TexCoord;
};

GLApplication::GLApplication() :
		windowManager(nullptr), camera(nullptr) {
}

GLApplication::~GLApplication() {
	destroy();
}

void GLApplication::GLMain() {
	initialize();
	applicationLoop();
}

void GLApplication::initialize() {
	if (!windowManager || !windowManager->initialize(800, 700, "Window GLFW", false)) {
		this->destroy();
		exit(-1);
	}

	glViewport(0, 0, WindowManager::screenWidth, WindowManager::screenHeight);
	glClearColor(0.2f, 0.2f, 0.2f, 0.0f);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	sp.init();
	sp.load();

	sp2.init();
	sp2.load();

	textureDifuse.load();
	textureSpecular.load();

	sistemascoordenados.initialize("Shaders/sistemascoordenados.vs", "Shaders/sistemascoordenados.fs");
	shader.initialize("Shaders/shader.vs", "Shaders/shader.fs");
	lightingShader.initialize("Shaders/lightingSpecularMap.vs","Shaders/lightingSpecularMap.fs");
	lampShader.initialize("Shaders/lampShader.vs", "Shaders/lampShader.fs");


}

void GLApplication::applicationLoop() {
	bool processInput = true;
	glm::vec3 lightPos( -2.0f, -2.0f, -2.0f);
	Model objModel("objects/green_arrow/Green_Arrow_A.obj");
	Model objModel1("objects/flash/Flash.obj");
	Model objModel2("objects/death/Deathstroke.obj");

	double lastTime = TimeManager::Instance().GetTime();
	while (processInput) {
		processInput = windowManager->processInput(true);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




		//Rotar esfera de luz
		GLfloat timeValue = TimeManager::Instance().GetTime() - lastTime;
		GLfloat valX = sin((GLfloat)timeValue * 1.0f) * 3.0f;
		GLfloat valY = sin((GLfloat)timeValue * 3.1416f/2) * 6.0f;
		GLfloat valZ = cos((GLfloat)timeValue * 1.0f) * 3.0f;
		lightPos.x = valX;
		lightPos.y = valY;
		lightPos.z = valZ;


		//dibujar la esfera
		lightingShader.turnOn();
		GLint viewPosLoc = lightingShader.getUniformLocation("viewPos");
		glUniform3f(viewPosLoc, lightPos.x, lightPos.y, lightPos.z);

		// Transformation matrices
		glm::vec3 cameraPos = windowManager->inputManager.getCameraPos();
		glm::vec3 cameraLookAt = windowManager->inputManager.getCameraPos() + windowManager->inputManager.getCameraDirection();
		glm::mat4 view = glm::lookAt(cameraPos, cameraLookAt, glm::vec3(0, 0.5, 0.0));
		//glm::mat4 view = camera->GetViewMatrix();
		glm::mat4 projection;
		projection = glm::perspective(45.0f, (GLfloat)WindowManager::screenWidth / (GLfloat)WindowManager::screenHeight, 0.1f, 100.0f);


		// Get the uniform locations
		GLint modelLoc = lightingShader.getUniformLocation("model");
		GLint viewLoc = lightingShader.getUniformLocation("view");
		GLint projLoc = lightingShader.getUniformLocation("projection");
		
		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// Set material properties
		GLint matDiffuseLoc = lightingShader.getUniformLocation("material.diffuse");
		GLint matSpecularLoc = lightingShader.getUniformLocation("material.specular");
		GLint matShineLoc = lightingShader.getUniformLocation("material.shininess");
		glUniform1i(matDiffuseLoc, 0);
		glUniform1i(matSpecularLoc, 1);
		glUniform1f(matShineLoc, 32.0f);

		// Set lights properties
		GLint lightAmbientLoc = lightingShader.getUniformLocation("light.ambient");
		GLint lightDiffuseLoc = lightingShader.getUniformLocation("light.diffuse");
		GLint lightSpecularLoc = lightingShader.getUniformLocation("light.specular");
		GLint lightPosLoc = lightingShader.getUniformLocation("light.position");
		glUniform3f(lightAmbientLoc, 0.2f, 0.2f, 0.2f);
		glUniform3f(lightDiffuseLoc, 0.9f, 0.9f, 0.9f);
		glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
		//Fin codigo luz

		// Draw a sphere sol
		glm::mat4 model0;
		model0 = glm::translate(model0, lightPos);
		model0 = glm::scale(model0, glm::vec3(0.6, 0.6, 0.6));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model0));
		textureDifuse.bind(GL_TEXTURE0);
		textureSpecular.bind(GL_TEXTURE1);
		sp2.render();
		lightingShader.turnOff();
		//acabar con la esfera sol


		//Inicio codigo de modelos
		shader.turnOn();

		// Get the uniform locations
		modelLoc = shader.getUniformLocation("model");
		viewLoc = shader.getUniformLocation("view");
		projLoc = shader.getUniformLocation("projection");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// Set material properties
		// matDiffuseLoc = lightingShader.getUniformLocation("material.diffuse");
		// matSpecularLoc = lightingShader.getUniformLocation("material.specular");
		matShineLoc = lightingShader.getUniformLocation("material.shininess");
		// glUniform1i(matDiffuseLoc, 0);
		// glUniform1i(matSpecularLoc, 1);
		glUniform1f(matShineLoc, 32.0f);

		// Set lights properties
		lightAmbientLoc = lightingShader.getUniformLocation("light.ambient");
		lightDiffuseLoc = lightingShader.getUniformLocation("light.diffuse");
		lightSpecularLoc = lightingShader.getUniformLocation("light.specular");
		lightPosLoc = lightingShader.getUniformLocation("light.position");
		glUniform3f(lightAmbientLoc, 0.2f, 0.2f, 0.2f);
		glUniform3f(lightDiffuseLoc, 0.9f, 0.9f, 0.9f);
		glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);


		// Draw the loaded model
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		objModel.render(&shader);

		// Draw the loaded model1
		glm::mat4 model1;
		model1 = glm::translate(model1, glm::vec3(0.75f, -1.75f, 0.0f));
		model1 = glm::scale(model1, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model1));
		objModel1.render(&shader);

		// Draw the loaded model2
		glm::mat4 model2;
		model2 = glm::translate(model2, glm::vec3(-0.75f, -1.75f, 0.0f));
		model2 = glm::scale(model2, glm::vec3(0.2f, 0.2f, 0.2f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model2));
		objModel2.render(&shader);

		shader.turnOff();
	//Fin dibujo modelos

	//Dibujo esfera iluminacion
		lampShader.turnOn();

		// Create transformations
		modelLoc = lampShader.getUniformLocation("model");
		viewLoc = lampShader.getUniformLocation("view");
		projLoc = lampShader.getUniformLocation("projection");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		model0 = glm::scale(model0, glm::vec3(0.1, 0.1, 0.1));
		model0 = glm::translate(model0, lightPos);
		//model = glm::scale(model, glm::vec3(0.4, 0.4, 0.4));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model0));
		sp.render();
		lampShader.turnOff();
	

		windowManager->swapTheBuffers();
	}
}

void GLApplication::destroy() {
	if (windowManager) {
		delete windowManager;
		windowManager = nullptr;
	}

	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDeleteBuffers(1, &EBO);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAO);

}
