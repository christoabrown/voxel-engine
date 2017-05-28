#include "../stdafx.h"
#include "input.h"
#include "window.h"

Input::Input()
{

}

Input::~Input()
{

}

Input& Input::getInstance()
{
	static Input instance;
	return(instance);
}

void Input::process() {Input::getInstance()._process();}
void Input::_process()
{
	_processKey();
	_processMousePos();
	_processMouseButton();
}

void Input::_processKey()
{
	/// Movement keys
	if(captureMouse)
	{
		if(keys[GLFW_KEY_W])
			TranslateCamera(FORWARD);
		if(keys[GLFW_KEY_S])
			TranslateCamera(BACKWARD);
		if(keys[GLFW_KEY_A])
			TranslateCamera(LEFT);
		if(keys[GLFW_KEY_D])
			TranslateCamera(RIGHT);
		if(keys[GLFW_KEY_SPACE])
			TranslateCamera(UP);
		if(keys[GLFW_KEY_LEFT_SHIFT])
			TranslateCamera(DOWN);
	}
	if(keys[GLFW_KEY_ESCAPE] && captureMouse)
	{
		captureMouse = false;
		glfwSetInputMode(Window::getGLFW(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

}

void Input::_processMousePos()
{
	_updateMousePos();
	if(deltaX != 0.0f || deltaY != 0.0f)
	{
		if(captureMouse)
		{
			RotateCamera(deltaX, deltaY);
		}
		deltaX = deltaY = 0.0f;
	}
}

void Input::_processMouseButton()
{
	if(mouseButtons[GLFW_MOUSE_BUTTON_LEFT] && !captureMouse)
	{
		captureMouse = true;
		firstMousePos = true;
		glfwSetInputMode(Window::getGLFW(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}
}

void Input::_updateKeys(int key, int action)
{
	if(key >= 0 && key < 1024)
	{
		if(action == GLFW_PRESS)
			keys[key] = true;
		else if(action == GLFW_RELEASE)
			keys[key] = false;
	}
}
void Input::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{Input::getInstance()._updateKeys(key, action);}

void Input::_updateMousePos()
{
	double xpos, ypos;
	glfwGetCursorPos(Window::getGLFW(), &xpos, &ypos);
	if(firstMousePos)
	{
		lastX = xpos;
		lastY = ypos;
		firstMousePos = false;
	}
	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;

	double repositionX, repositionY;
	if(captureMouse && (xoffset >= 1.0 || xoffset <= -1.0 || yoffset >= 1.0 || yoffset <= -1.0))
	{
		repositionX = Window::getWidth()/2;
		repositionY = Window::getHeight()/2;
		glfwSetCursorPos(Window::getGLFW(), repositionX, repositionY);
	}
	else
	{
		repositionX = xpos;
		repositionY = ypos;
	}
	lastX = repositionX;
	lastY = repositionY;
	deltaX += xoffset * sensitivity;
	deltaY += yoffset * sensitivity;
}
void Input::mousePosCallback(GLFWwindow* window, double xpos, double ypos)
{/*Input::getInstance()._updateMousePos(xpos, ypos);*/}

void Input::_updateMouseButton(int button, int action)
{
	if(action == GLFW_PRESS)
		mouseButtons[button] = true;
	else if(action == GLFW_RELEASE)
		mouseButtons[button] = false;
}
void Input::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{Input::getInstance()._updateMouseButton(button, action);}