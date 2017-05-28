#include "stdafx.h"
#include "camera.h"
#include "renderer.h"
#include "chunk.h"

Camera camera;
unsigned long long prevFrameCount = 0;

glm::mat4 GetViewMatrix()
{
	return glm::lookAt(camera.position, camera.position + camera.front, camera.up);
}

glm::mat4 GetProjectionMatrix(const GLfloat screenWidth, const GLfloat screenHeight)
{
	camera.ratio = screenWidth / screenHeight;
	return glm::perspective(glm::radians(camera.fov), camera.ratio, camera.nearD, camera.farD);
}

bool pointInFrustum(glm::vec3 &pt)
{
	for (int i = 0; i < 6; ++i)
	{
		if (camera.planes[i].signedDistance(pt) < 0)
			return(false);
	}
	return(true);
}

glm::vec3 getForward()
{
	return(camera.front);
}

float getFov()
{
	return(camera.fov);
}

void UpdateCamera()
{
	//update front rotation
	glm::vec3 front;
	
	front.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
	front.y = sin(glm::radians(camera.pitch));
	front.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
	camera.front = glm::normalize(front);
	//re-calculate up and right
	camera.right = glm::normalize(glm::cross(camera.front, camera.worldUp));
	camera.up = glm::normalize(glm::cross(camera.right, camera.front));

	//update clip plane points
	GLfloat ang = (GLfloat)tan(glm::radians(camera.fov) * 0.5);
	camera.nearH = camera.nearD * ang * camera.ratio;
	camera.nearW = camera.nearH * camera.ratio;
	camera.farH = camera.farD * ang * camera.ratio;
	camera.farW = camera.farH * camera.ratio;

	//centers of near and far plane
	glm::vec3 virtualPosition = camera.position - (camera.front * glm::vec3(camera.fov / 2 + CHUNK_SIZE / 2));
	glm::vec3 nc, fc;
	nc = virtualPosition - (-camera.front) * glm::vec3(camera.nearD);
	fc = virtualPosition - (-camera.front) * glm::vec3(camera.farD);
	//4 corners of near plane
	camera.ntl = nc + camera.up * camera.nearH - camera.right * camera.nearW;
	camera.ntr = nc + camera.up * camera.nearH + camera.right * camera.nearW;
	camera.nbl = nc - camera.up * camera.nearH - camera.right * camera.nearW;
	camera.nbr = nc - camera.up * camera.nearH + camera.right * camera.nearW;
	//4 corners of far plane
	camera.ftl = fc + camera.up * camera.farH - camera.right * camera.farW;
	camera.ftr = fc + camera.up * camera.farH + camera.right * camera.farW;
	camera.fbl = fc - camera.up * camera.farH - camera.right * camera.farW;
	camera.fbr = fc - camera.up * camera.farH + camera.right * camera.farW;
	//CCW order constructs planes
	camera.planes[0] = Plane(camera.ntr, camera.ntl, camera.ftl);
	camera.planes[1] = Plane(camera.nbl, camera.nbr, camera.fbr);
	camera.planes[2] = Plane(camera.ntl, camera.nbl, camera.fbl);
	camera.planes[3] = Plane(camera.nbr, camera.ntr, camera.fbr);
	camera.planes[4] = Plane(camera.ntl, camera.ntr, camera.nbr);
	camera.planes[5] = Plane(camera.ftr, camera.ftl, camera.fbl);
}

void TranslateCamera(Camera_Movement direction)
{
	GLfloat speed = camera.movementSpeed * DeltaTime();
	if (direction == FORWARD)
		camera.position += camera.front * speed;
	if (direction == BACKWARD)
		camera.position -= camera.front * speed;
	if (direction == LEFT)
		camera.position -= camera.right * speed;
	if (direction == RIGHT)
		camera.position += camera.right * speed;
	if (direction == UP)
		camera.position += camera.worldUp * speed;
	if (direction == DOWN)
		camera.position -= camera.worldUp * speed;
	UpdateCamera();
}

void RotateCamera(GLfloat xoffset, GLfloat yoffset)
{
	camera.yaw += xoffset;
	camera.pitch += yoffset;

	if (camera.pitch > 89.0f)
		camera.pitch = 89.0f;
	if (camera.pitch < -89.0f)
		camera.pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(camera.pitch)) * cos(glm::radians(camera.yaw));
	front.y = sin(glm::radians(camera.pitch));
	front.z = cos(glm::radians(camera.pitch)) * sin(glm::radians(camera.yaw));

	camera.front = glm::normalize(front);

	UpdateCamera();
}

Point getPosition()
{
	//TODO: should probably use glm::vec3 to preserve floats
	return(Point(camera.position.x / CHUNK_SIZE, camera.position.y / CHUNK_SIZE, camera.position.z / CHUNK_SIZE));
}