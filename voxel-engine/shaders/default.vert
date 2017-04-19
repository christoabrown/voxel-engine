#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 normal;
layout (location = 4) in vec3 instancePosition;

out vec3 ourColor;
out vec2 TexCoord;
//out float sunLight;
out vec4 eyeSpacePos;

layout (location = 16) uniform mat4 m_model;
layout (location = 17) uniform mat4 m_view;
layout (location = 18) uniform mat4 m_projection;
uniform vec3 sunPos;

void main()
{
	vec4 eyeSpacePosVert = m_view * m_model * vec4(position + instancePosition, 1.0f);
	gl_Position = m_projection *  eyeSpacePosVert;

	eyeSpacePos = eyeSpacePosVert;
	float sunLight = 0.8 - dot(normal, vec3(0.0, -1.0, 0.0)) / 3;
	ourColor = color * sunLight;
	//ourColor = color;
	TexCoord = vec2(texCoord.x, 1.0 - texCoord.y);
	//sunLight = 0.8 - dot(normal, vec3(0.0, -1.0, 0.0)) / 3;
}