#pragma once
#include "../stdafx.h"
#include "../camera.h"
/// Singleton class
class Input
{
private:
	Input();
	~Input();
	/// Delete these two se we don't duplicate
	Input(Input const&);	
	void operator=(Input const&);
public:
	static Input& getInstance();

private:
	/// Keyboard
	std::array<bool, 1024> keys;
	/// Mouse
	std::array<bool, GLFW_MOUSE_BUTTON_LAST> mouseButtons;
	bool firstMousePos = true;
	bool captureMouse = false;
	double lastX, lastY;
	GLfloat deltaX, deltaY;
	GLfloat sensitivity = 0.20f;

	/// Act on inputs
	void _process();
	void _processKey();
	void _processMousePos();
	void _processMouseButton();
	/// update variables
	void _updateKeys(int key, int action);
	void _updateMousePos();
	void _updateMouseButton(int button, int action);
public:
	/// Main loop entry point
	static void process();
	/// Callbacks
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mousePosCallback(GLFWwindow* window, double xpos, double ypos);
	static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
};