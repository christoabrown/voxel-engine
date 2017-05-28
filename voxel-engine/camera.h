#pragma once
#include "map3d.h"
#include "util.h"
#include "chunk.h"

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

struct Camera {
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 front = glm::vec3(0.0, 0.0, -1.0);
	glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);
	glm::vec3 worldUp = glm::vec3(0.0, 1.0, 0.0);
	glm::vec3 right = glm::vec3(1.0, 0.0, 0.0);
	glm::vec3 ntl, ntr, nbl, nbr, ftl, ftr, fbl, fbr;
	Plane planes[6];

	GLfloat nearD = 0.1f;
	GLfloat farD = CHUNK_SIZE * WORLD_SIZE * 1.2;
	GLfloat yaw = 0.0f;
	GLfloat pitch = 0.0f;
	GLfloat nearH, nearW, farH, farW;
	GLfloat ratio;

	GLfloat movementSpeed = 100.0f;
	GLfloat mouseSensitivity = 0.10f;
	GLfloat fov = 90.0f;
};

void TranslateCamera(Camera_Movement direction);
glm::mat4 GetViewMatrix();
glm::mat4 GetProjectionMatrix(const GLfloat screenWidth, const GLfloat screenHeight);
bool pointInFrustum(glm::vec3 &pt);
void RotateCamera(GLfloat xoffset, GLfloat yoffset);
Point getPosition();
glm::vec3 getForward();
float getFov();