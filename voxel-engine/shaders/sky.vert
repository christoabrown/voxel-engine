#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
const vec2 quadVertices[4] = vec2[]( vec2(-1.0, -1.0), vec2(1.0, -1.0), vec2(-1.0, 1.0), vec2(1.0, 1.0) );

layout (location = 1) uniform vec3 front;
out vec3 cameraFront;

void main()
{
	gl_Position = vec4(quadVertices[gl_VertexID], 0.9999999, 1.0);
	cameraFront = front;
}