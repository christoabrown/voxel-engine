#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
in vec3 cameraFront;
layout (location = 2) uniform float winHeight;
layout (location = 3) uniform float winWidth;
layout (location = 4) uniform float fov;
const float PI = 3.14159;

out vec4 color;

void main() {
	float aspect = winWidth / winHeight;
	float Px = (2.0 * ((gl_FragCoord.x + 0.5) / winWidth) - 1.0) * tan(fov / 2.0 * PI / 180.0) * aspect; 
	float Py = (1.0 - 2.0 * ((gl_FragCoord.y + 0.5) / winHeight)) * tan(fov / 2.0 * PI / 180.0); 
	vec3 rayDir = cameraFront - vec3(Px, Py, 1);

	color = vec4(0.678, 0.847, 0.902, 1.0) / 2.0;
	color = color + rayDir.y / 6.0;
}