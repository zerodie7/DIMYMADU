/*
 * CameraFPS.h
 *
 *  Created on: 06/09/2016
 *      Author: rey
 */
#pragma once

// Std. Includes
#include <vector>

// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "definition.h";

enum Camera_Movement {
	C_FORWARD, C_BACKWARD, C_LEFT, C_RIGHT, C_REINICIO, C_CajaCaja, C_CajaReles
};

// Default camera values
const float YAW = -180.0f;
const float SPEED = 6.0f;
const float DISTANCE = 20.0f;
const float PITCH = 20.0f;
const float MOUSESENS = 0.1f;

class CameraFPS {
public:
	// Camera Attributes
	glm::vec3 CharPosition;
	glm::vec3 CamPosition;
	glm::vec3 CamDirection;
	glm::vec3 Front;
	glm::vec3 WorldUp;
	// Eular Angles
	float Yaw;
	float Pitch;
	// Camera options
	float MovementSpeed;
	float distanceFromPlayer;
	float angleAroundPlayer;

	// Constructor with vectors
	CameraFPS(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f)) :
		Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED) {
		this->CharPosition = position;

		this->WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
		this->Yaw = YAW;
		this->Pitch = PITCH;
		this->distanceFromPlayer = DISTANCE;
		this->angleAroundPlayer = 0.0f;

		//calcular posicion de la camara y su dirección
		//a partir de la distancia del jugador, y la posicion de el jugador
		this->calcCameraPositionDirection();
	}

	glm::mat4 GetViewMatrix() {
		return glm::lookAt(this->CamPosition, this->CamPosition + this->CamDirection, this->WorldUp);
	}

	void ProcessKeyboard(Camera_Movement direction, float deltaTime) {
		float velocity = this->MovementSpeed * deltaTime;
		if (direction == C_REINICIO){
			this->CharPosition = glm::vec3(0.0f, 0.0f, 0.0f);
			this->Front = glm::vec3(0.0f, 0.0f, -1.0f);
			this->Yaw = YAW;
			this->Pitch = PITCH;
			this->distanceFromPlayer = DISTANCE;
			this->angleAroundPlayer = 0.0f;
			ColBol::contactp1 = false;
			ColBol::contactp2 = false;
			ColBol::contactp3 = false;
			this->calcCameraPositionDirection();
		}
		if (direction == C_CajaCaja)
		{
			ColBol::npc4go = true;
		}
		if (direction == C_CajaReles)
		{
			ColBol::npc4go = false;
		}
		if (direction == C_FORWARD && !ColBol::contactp1 && !ColBol::contactp2 && !ColBol::contactp3){
			this->CharPosition += this->Front * velocity;
			this->CamPosition += this->Front * velocity;
		}
		if (direction == C_BACKWARD && !ColBol::contactp1 && !ColBol::contactp2 && !ColBol::contactp3){
			this->CharPosition -= this->Front * velocity;
			this->CamPosition -= this->Front * velocity;
		}
		if (direction == C_LEFT && !ColBol::contactp1 && !ColBol::contactp2 && !ColBol::contactp3){
			this->angleAroundPlayer -= 0.1f ;
			this->calcCameraPositionDirection();
		}
		if (direction == C_RIGHT && !ColBol::contactp1 && !ColBol::contactp2 && !ColBol::contactp3){
			this->angleAroundPlayer += 0.1f ;
			this->calcCameraPositionDirection();
		}
		
	}

	void ProcessMouseMovement(float xoffset, float yoffset,
			GLboolean constrainPitch = true) {

		this->angleAroundPlayer += xoffset*MOUSESENS;
		this->Pitch += yoffset*MOUSESENS;
		if (this->Pitch >= 80.0f){
			this->Pitch = 80.0f;
		}
		else if (this->Pitch <= 10.0f){
			this->Pitch = 10.0f;
		}
		calcCameraPositionDirection();
		
	}

	void ProcessScroll(float yoffset) {
		
		this->distanceFromPlayer += yoffset*MOUSESENS*3.0f;
		if (this->distanceFromPlayer >= 50.0f){
			this->distanceFromPlayer = 50.0f;
		}
		else if (this->distanceFromPlayer <= 10.0f){
			this->distanceFromPlayer = 10.0f;
		}
		calcCameraPositionDirection();

	}

private:
	void calcCameraPositionDirection(){
		
		//Calculate Horizontal distance
		float horizontalDistance = this->distanceFromPlayer * glm::cos(glm::radians(this->Pitch));
		//std::cout << "HORDI:" << horizontalDistance << std::endl;
		//Calculate Vertical distance
		float verticalDistance = distanceFromPlayer * glm::sin(glm::radians(this->Pitch));
		//std::cout << "VERDI:" << verticalDistance << std::endl;

		//Calculate camera position
		float offsetx = horizontalDistance * glm::sin(glm::radians(this->angleAroundPlayer));
		float offsetz = horizontalDistance * glm::cos(glm::radians(this->angleAroundPlayer));
		this->CamPosition.x = this->CharPosition.x - offsetx;
		this->CamPosition.z = this->CharPosition.z + offsetz;
		
		this->CamPosition.y = this->CharPosition.y + verticalDistance;
		this->Yaw = 0 - (180 + this->angleAroundPlayer);
		
		//Calcular dirección ya con la posicion
		this->CamDirection = glm::normalize(this->CharPosition - this->CamPosition);

		//sacar front
		glm::vec3 front;
		front.x = sin(glm::radians(this->Yaw));
		front.y = 0.0f;
		front.z = cos(glm::radians(this->Yaw));
		this->Front = glm::normalize(front);
	}
	
	void updateCameraVectors() {
	}
};
