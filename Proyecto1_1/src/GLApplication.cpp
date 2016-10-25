/*
 * GLApplication.cpp
 *
 *  Created on: 07/08/2016
 *  
 */

#include "Headers/GLApplication.h"
#include "Headers/collision.h"

GLuint VAO, VBO, EBO;
Cube cubeColis(1.0f, 1.0f, 1.0f, true);
Sphere spColis(1.0, 20, 20, true);
Sphere sp(20.0, 25, 25, false);
Shader lampShader;
Shader shader;
Shader ligtingshader;
Texture textureDifuse(GL_TEXTURE_2D, "Textures/SUN.jpg");
Texture textureTerrain(GL_TEXTURE_2D, "Textures/pasto.png");
GLuint VAOpiso, VBOpiso, EBOpiso;

std::vector<GLuint> rays;

//Rayo para colision 
bool ColBol::contactp1;
bool ColBol::contactp2;
bool ColBol::contactp3;
bool ColBol::npc4go;
float npc4down = 0.0f;


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

	//Esfera Colision
	spColis.init();
	spColis.load();

	//Cubo Colision 
	cubeColis.init();
	cubeColis.load();

	//Esfera sol
	sp.init();
	sp.load();

	//Cargar Texturas
	textureDifuse.load();
	textureTerrain.load();
	
	ColBol::contactp1 = false;
	ColBol::contactp2 = false;
	ColBol::contactp3 = false;
	ColBol::npc4go = false;

	//inicializar piso

	VertexNormalText vertex[4] =
	{
		{
			glm::vec3(1, 0, 1),
			glm::vec3(0, 1, 0),
			glm::vec2(10, 0)
		},
		{
			glm::vec3(1, 0, -1),
			glm::vec3(0, 1, 0),
			glm::vec2(10, 10)
		},
		{
			glm::vec3(-1, 0, -1),
			glm::vec3(0, 1, 0),
			glm::vec2(0, 10)
		},
		{
			glm::vec3(-1, 0, 1),
			glm::vec3(0, 1, 0),
			glm::vec2(0, 0)
		}
	};

	GLuint index[6] = { 0, 1, 3, 1, 2, 3 };

	glGenVertexArrays(1, &VAOpiso);
	glGenBuffers(1, &VBOpiso);
	glGenBuffers(1, &EBOpiso);

	glBindVertexArray(VAOpiso);

	glBindBuffer(GL_ARRAY_BUFFER, VBOpiso);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOpiso);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index,
		GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex[0]),
		(GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex[0]),
		(GLvoid*) sizeof(vertex[0].position));
	glEnableVertexAttribArray(1);
	// Texture attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex[0]),
		(GLvoid*)(sizeof(vertex[0].position) + sizeof(vertex[0].normal)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Unbind VAO

	ligtingshader.initialize("Shaders/lightingSpecularMap.vs", "Shaders/lightingSpecularMap.fs");
	lampShader.initialize("Shaders/lampShader.vs", "Shaders/lampShader.fs");
	shader.initialize("Shaders/shader.vs", "Shaders/shader.fs");
}

void GLApplication::applicationLoop() {
	bool processInput = true;
	glm::vec3 lightPos( 0.0f, 0.0f, 0.0f);

	//Posicion de modelos
	glm::vec3 pos_play;
	glm::vec3 pos_npc1(-15.0f, 0.0f, -10.0f);
	glm::vec3 pos_npc2(-10.0f, 0.0f, 0.0f);
	glm::vec3 pos_npc3(-20.0f, 0.0f, -20.0f);
	glm::vec3 pos_npc4(-10.0f, 5.0f, 0.0f);

	Model objModel_Player("objects/green_arrow/Green_Arrow_A.obj");
	Model objModel_P1("objects/death/Deathstroke.obj");
	Model objModel_P2("objects/cat/cat.obj");
	Model objModel_P3("objects/flash/Flash.obj");
	Model objModel_P4("objects/Rabbit/Rabbit.obj");


	//Definir volumenes envolventes al modelo
	SBB sbb1 = getSBB(objModel_Player.getMeshes());
	SBB sbb2 = getSBB(objModel_P3.getMeshes());
	AABB aabb1 = getAABB(objModel_P1.getMeshes());
	AABB aabb2 = getAABB(objModel_P2.getMeshes());
	AABB aabb3 = getAABB(objModel_P4.getMeshes());

	float angplay = 0.0f;
	float angnpc1 = 0.0f;
	float angnpc2 = 0.0f;
	float angnpc3 = 0.0f;
	float angnpc4 = 0.0f;

	double lastTime = TimeManager::Instance().GetTime();

	while (processInput) {
		processInput = windowManager->processInput(true);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::vec4 viewport = glm::vec4(0.0f, 0.0f, WindowManager::screenWidth,
			WindowManager::screenHeight);


		//Rotar esfera de luz
		GLfloat timeValue = TimeManager::Instance().GetTime() - lastTime;
		GLfloat valX = sin((GLfloat)timeValue * 1.0f) * 70.0f;
		GLfloat valY = sin((GLfloat)timeValue * 3.1416f / 2) * 70.0f;
		GLfloat valZ = cos((GLfloat)timeValue * 1.0f) * 70.0f;
		lightPos.x = valX;
		lightPos.y = valY;
		lightPos.z = valZ;

		ligtingshader.turnOn();

		GLint viewPosLoc = ligtingshader.getUniformLocation("viewPos");
		glUniform3f(viewPosLoc, camera->CamPosition.x, camera->CamPosition.y, camera->CamPosition.z);

		// Set material properties
		GLint matShineLoc = ligtingshader.getUniformLocation("material.shininess");
		glUniform1f(matShineLoc, 50.0f);

		// Set lights properties
		GLint lightAmbientLoc = ligtingshader.getUniformLocation("light.ambient");
		GLint lightDiffuseLoc = ligtingshader.getUniformLocation("light.diffuse");
		GLint lightSpecularLoc = ligtingshader.getUniformLocation("light.specular");
		GLint lightPosLoc = ligtingshader.getUniformLocation("light.position");
		glUniform3f(lightAmbientLoc, 0.2f, 0.2f, 0.2f);
		glUniform3f(lightDiffuseLoc, 0.5f, 0.5f, 0.5f); // Let's darken the light a bit to fit the scene
		glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);

		// Transformation matrices
		glm::mat4 view = camera->GetViewMatrix();
		glm::mat4 projection;
		projection = glm::perspective(45.0f, (GLfloat)WindowManager::screenWidth / (GLfloat)WindowManager::screenHeight, 0.1f, 100.0f);
		// Get the uniform locations
		GLint modelLoc = ligtingshader.getUniformLocation("model");
		GLint viewLoc = ligtingshader.getUniformLocation("view");
		GLint projLoc = ligtingshader.getUniformLocation("projection");
		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));


		// Draw the loaded model
		glm::mat4 model;

		//Player 1
		pos_play = glm::vec3(camera->CharPosition.x, camera->CharPosition.y, camera->CharPosition.z);

		glm::mat4 model_play;
		model_play = glm::translate(model_play, pos_play);
		model_play = glm::rotate(model_play, (GLfloat)glm::radians(180.0f + angplay - camera->angleAroundPlayer), glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::rotate(model, (GLfloat) 3.1416f, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_play));
		objModel_Player.render(&ligtingshader);

		// Draw the loaded model1
		glm::mat4 model_P1;
		model_P1 = glm::translate(model_P1, pos_npc1);
		model_P1 = glm::rotate(model_P1, (GLfloat)glm::radians(angnpc1), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_P1));
		objModel_P1.render(&ligtingshader);

		// Draw the loaded model2
		glm::mat4 model_P2;
		model_P2 = glm::translate(model_P2, pos_npc2);
		model_P2 = glm::rotate(model_P2, (GLfloat)glm::radians(angnpc2), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_P2));
		objModel_P2.render(&ligtingshader);
	
		// Draw the loaded model3
		glm::mat4 model_P3;
		model_P3 = glm::translate(model_P3, pos_npc3);
		model_P3 = glm::rotate(model_P3, (GLfloat)glm::radians(angnpc3), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_P3));
		objModel_P3.render(&ligtingshader);

		// Draw the loaded model4
		glm::mat4 model_P4;
		model_P4 = glm::translate(model_P4, pos_npc4);
		model_P4 = glm::rotate(model_P4, (GLfloat)glm::radians(angnpc4), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_P4));
		objModel_P4.render(&ligtingshader);
		//Fin dibujo modelos

		/*Dibujo Terreno*/
		model = glm::scale(glm::mat4(), glm::vec3(40.0, 40.0, 40.0));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		GLint unitTextureLocation = shader.getUniformLocation("material.texture_diffuse1");
		unitTextureLocation = shader.getUniformLocation("material.texture_specular1");
		glUniform1i(unitTextureLocation, 0);
		// And finally bind the textur
		textureTerrain.bind(GL_TEXTURE0);

		glBindVertexArray(VAOpiso);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (GLvoid*)(sizeof(GLuint) * 0));
		glBindVertexArray(0);

		ligtingshader.turnOff();
		/*Fin Terreno*/



		/*Dibujo Sol*/
		lampShader.turnOn();

		// Create transformations
		modelLoc = lampShader.getUniformLocation("model");
		viewLoc = lampShader.getUniformLocation("view");
		projLoc = lampShader.getUniformLocation("projection");
		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::scale(glm::mat4(), glm::vec3(0.5, 0.5, 0.5));
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2, 0.2, 0.2));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		//unir la textura
		textureDifuse.bind(GL_TEXTURE0);
		glUniform1i(lampShader.getUniformLocation("ourTexture"), 0);

		//poner la esfera
		sp.render();
		lampShader.turnOff();

		/*Fin Sol*/


		/*Inicio algoritmo de Colisiones*/
		ligtingshader.turnOn();

		modelLoc = ligtingshader.getUniformLocation("model");
		viewLoc = ligtingshader.getUniformLocation("view");
		projLoc = ligtingshader.getUniformLocation("projection");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		model_play = glm::translate(model_play, glm::vec3(sbb1.center.x, sbb1.center.y, sbb1.center.z));
		model_play = glm::scale(model_play, glm::vec3(sbb1.ratio, sbb1.ratio, sbb1.ratio));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_play));
		spColis.render();

		model_P3 = glm::translate(model_P3, glm::vec3(sbb2.center.x, sbb2.center.y, sbb2.center.z));
		model_P3 = glm::scale(model_P3, glm::vec3(sbb2.ratio, sbb2.ratio, sbb2.ratio));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_P3));
		spColis.render();

		model_P1 = glm::translate(model_P1, glm::vec3(
			(aabb1.max.x + aabb1.min.x) / 2.0f,
			(aabb1.max.y + aabb1.min.y) / 2.0f,
			(aabb1.max.z + aabb1.min.z) / 2.0f
			));
		model_P1 = glm::scale(model_P1, glm::vec3(
			(aabb1.max.x - aabb1.min.x),
			(aabb1.max.y - aabb1.min.y),
			(aabb1.max.z - aabb1.min.z)
			));
		model_P1 = glm::scale(model_P1, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_P1));
		cubeColis.render();
		
		model_P2 = glm::translate(model_P2, glm::vec3(
			(aabb2.max.x + aabb2.min.x) / 2.0f,
			(aabb2.max.y + aabb2.min.y) / 2.0f,
			(aabb2.max.z + aabb2.min.z) / 2.0f
			));

		model_P2 = glm::scale(model_P2, glm::vec3(
			(aabb2.max.x - aabb2.min.x),
			(aabb2.max.y - aabb2.min.y),
			(aabb2.max.z - aabb2.min.z)
			));
		model_P2 = glm::scale(model_P2, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_P2));
		cubeColis.render();

		model_P4 = glm::translate(model_P4, glm::vec3(
			(aabb3.max.x + aabb3.min.x) / 2.0f,
			(aabb3.max.y + aabb3.min.y) / 2.0f,
			(aabb3.max.z + aabb3.min.z) / 2.0f
			));
		model_P4 = glm::scale(model_P4, glm::vec3(
			(aabb3.max.x - aabb3.min.x),
			(aabb3.max.y - aabb3.min.y),
			(aabb3.max.z - aabb3.min.z)
			));
		model_P4 = glm::scale(model_P4, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model_P4));
		cubeColis.render();

		/*Fin algoritmo de Colisiones*/

		/*Calculo de Colisiones*/

		//colisiones de movimiento
		AABB a1, a2, a3;
		SBB s1, s2;

		//esfera y esfera
		s1.center = glm::vec3(model_play * glm::vec4(0, 0, 0, 1));
		s1.ratio = sbb1.ratio;
		s2.center = glm::vec3(model_P3 * glm::vec4(0, 0, 0, 1));
		s2.ratio = sbb2.ratio;
		if (testSphereSphereIntersection(s1, s2)){
			ColBol::contactp3 = true;
			std::cout << "Model collision:" << std::endl;
		}
		else{
			ColBol::contactp3 = false;
		}

		//cubo y esfera
		a1 = getAABB(
			glm::vec3(aabb1.min.x + pos_npc1.x, aabb1.min.y + pos_npc1.y, aabb1.min.z + pos_npc1.z),
			glm::vec3(aabb1.max.x + pos_npc1.x, aabb1.max.y + pos_npc1.y, aabb1.max.z + pos_npc1.z)
			);

		a2 = getAABB(
			glm::vec3(aabb2.min.x + pos_npc2.x, aabb2.min.y + pos_npc2.y, aabb2.min.z + pos_npc2.z),
			glm::vec3(aabb2.max.x + pos_npc2.x, aabb2.max.y + pos_npc2.y, aabb2.max.z + pos_npc2.z)
			);

		if (testSphereCubeIntersection(a1, s1)){
			ColBol::contactp1 = true;
			std::cout << "Model collision:" << std::endl;
		}

		if (testSphereCubeIntersection(a2, s1)){
			ColBol::contactp2 = true;
			std::cout << "Model collision:" << std::endl;
		}

		//cubo y cubo

		if (ColBol::npc4go){
			pos_npc4.y -= 0.005f;
		}

		a3 = getAABB(
			glm::vec3(aabb3.min.x + pos_npc4.x, aabb3.min.y + pos_npc4.y, aabb3.min.z + pos_npc4.z),
			glm::vec3(aabb3.max.x + pos_npc4.x, aabb3.max.y + pos_npc4.y, aabb3.max.z + pos_npc4.z)
			);

		if (testCubeCubeIntersection(a2, a3)){
			pos_npc4.y = 10.0f;
		}

		if (windowManager->inputManager.isGenerateRay()) {

			glm::vec3 o = glm::unProject(glm::vec3(windowManager->inputManager.getLastMousePos().x,
				WindowManager::screenHeight - windowManager->inputManager.getLastMousePos().y,
				0.0f), camera->GetViewMatrix(), projection, viewport);
			glm::vec3 t = glm::unProject(glm::vec3(windowManager->inputManager.getLastMousePos().x,
				WindowManager::screenHeight - windowManager->inputManager.getLastMousePos().y,
				1.0f), camera->GetViewMatrix(), projection,viewport);

			VertexColor vertex[2] = { { o, glm::vec3(0.0) }, { t, glm::vec3(0.0) } };
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex[0]), (GLvoid*)0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

			rays.push_back(VAO);


			glm::vec3 c1 = glm::vec3(model_play * glm::vec4(0, 0, 0, 1));
			glm::vec3 c2 = glm::vec3(model_P3 * glm::vec4(0, 0, 0, 1));
			glm::vec3 dir = glm::normalize(t - o);
			float t1;

			glm::vec3 vDirToSphere = c1 - o;
			float fLineLength = glm::distance(o, t);
			t1 = glm::dot(vDirToSphere, dir);

			if (raySphereIntersect(o, t, dir, c1, sbb1.ratio, t1)){
				if (angplay == 0.0f){
					angplay = 180.0f;
				}
				else{
					angplay = 0.0f;
				}
				std::cout << "Picking" << std::endl;
			}

			vDirToSphere = c2 - o;
			fLineLength = glm::distance(o, t);
			t1 = glm::dot(vDirToSphere, dir);

			if (raySphereIntersect(o, t, dir, c2, sbb2.ratio, t1)){
				if (angnpc3 == 0.0f){
					angnpc3 = 180.0f;
				}
				else{
					angnpc3 = 0.0f;
				}
				std::cout << "Picking" << std::endl;
			}

			if (rayCubeIntersect(o, t, dir, a1)){
				if (angnpc1 == 0.0f){
					angnpc1 = 180.0f;
				}
				else{
					angnpc1 = 0.0f;
				}
				std::cout << "Picking" << std::endl;
			}
			if (rayCubeIntersect(o, t, dir, a2)){
				if (angnpc2 == 0.0f){
					angnpc2 = 180.0f;
				}
				else{
					angnpc2 = 0.0f;
				}
				std::cout << "Picking" << std::endl;
			}
			if (rayCubeIntersect(o, t, dir, a3)){
				if (angnpc4 == 0.0f){
					angnpc4 = 180.0f;
				}
				else{
					angnpc4 = 0.0f;
				}
				std::cout << "Picking" << std::endl;
			}

			windowManager->inputManager.setGenerateRay(false);
		}
		/*Fin del calculo de Colisiones*/


		// Create transformations
		modelLoc = lampShader.getUniformLocation("model");
		viewLoc = lampShader.getUniformLocation("view");
		projLoc = lampShader.getUniformLocation("projection");

		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		// Draw the loaded model
		glm::mat4 modelL = glm::mat4();
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelL));

		for (int i = 0; i < rays.size(); i++) {
			glBindVertexArray(rays[i]);
			glDrawArrays(GL_LINES, 0, 2);
		}

		ligtingshader.turnOn();

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

