#include <iostream>

#include "Headers/InputManager.h"
#include "Headers/TimeManager.h"

InputCodes InputManager::toApplicationKey(int key) {
	switch (key) {
	case 256:
		return InputCodes::kEscape;
	case 87:
		return InputCodes::W;
	case 83:
		return InputCodes::x;
	case 120:
		return InputCodes::X;
	case 88:
		return InputCodes::r;
	case 114:
		return InputCodes::R;
	case 82:
		return InputCodes::S;
	case 65:
		return InputCodes::A;
	case 68:
		return InputCodes::D;
	case 265:
		return InputCodes::Up;
	case 264:
		return InputCodes::Down;
	case 263:
		return InputCodes::Left;
	case 262:
		return InputCodes::Right;
	}
}

State InputManager::toApplicationState(int state) {
	switch (state) {
	case 0:
		return State::RELESED;
	case 1:
		return State::PRESSED;
	}
}

MouseButtonIndex InputManager::toMouseButtonIndex(int button) {
	switch (button) {
	case 0:
		return MouseButtonIndex::LEFT;
	case 1:
		return MouseButtonIndex::RIGHT;
	case 2:
		return MouseButtonIndex::MIDDLE;
	}
}

void InputManager::keyPressed(InputCodes code, float deltaTime, State state) {
	keyState[code] = state;
}

void InputManager::mouseMoved(float mouseX, float mouseY) {
	if (mouseButtomState[MouseButtonIndex::LEFT]) {
		float xoffset = mouseX - lastMousePos.x;
		float yoffset = lastMousePos.y - mouseY;
		camera->ProcessMouseMovement(xoffset, yoffset);
	}
	lastMousePos.x = mouseX;
	lastMousePos.y = mouseY;
}

void InputManager::mouseClicked(MouseButtonIndex code, float mouseX,
		float mouseY, State state) {
	
	switch (code) {
	case RIGHT:
		std::cout << "lastMousePos.x:" << lastMousePos.x << std::endl;
		generateRay = true;
		break;
	case LEFT:
		std::cout << "lastMousePos.y:" << lastMousePos.y << std::endl;
		lastMousePos.x = mouseX;
		lastMousePos.y = mouseY;
		mouseButtomState[code] = state;
		break;
	case MIDDLE:
		std::cout << "lastMousePos.x:" << lastMousePos.x << std::endl;
		std::cout << "lastMousePos.y:" << lastMousePos.y << std::endl;
		break;
	}
}

void InputManager::mouseScroll(float yoffset) {
	camera->ProcessScroll(yoffset);
}

void InputManager::do_movement(float deltaTime) {
	if (keyState[InputCodes::W] || keyState[InputCodes::w] || keyState[InputCodes::Up])
		camera->ProcessKeyboard(C_FORWARD, deltaTime);
	if (keyState[InputCodes::X] || keyState[InputCodes::x] || keyState[InputCodes::Down])
		camera->ProcessKeyboard(C_BACKWARD, deltaTime);
	if (keyState[InputCodes::A] || keyState[InputCodes::a] || keyState[InputCodes::Left])
		camera->ProcessKeyboard(C_LEFT, deltaTime);
	if (keyState[InputCodes::D] || keyState[InputCodes::d] || keyState[InputCodes::Right])
		camera->ProcessKeyboard(C_RIGHT, deltaTime);
	if (keyState[InputCodes::S] || keyState[InputCodes::s])
		camera->ProcessKeyboard(C_REINICIO, deltaTime);
	if (keyState[InputCodes::R] || keyState[InputCodes::r])
		camera->ProcessKeyboard(C_CajaCaja, deltaTime);
	else{
		camera->ProcessKeyboard(C_CajaReles, deltaTime);
	}
}
