#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
in vec3 ourColor;
in vec2 TexCoord;
//in float sunLight;
in vec4 eyeSpacePos;

out vec4 color;

//texture samplers
layout (location = 48) uniform sampler2D ourTexture;

void main()
{
	float fogCoord = abs(eyeSpacePos.z/eyeSpacePos.w);
	float fogEnd = 450.0;
	float fogStart = fogEnd - fogEnd/2.0;
	float fogFactor = 1.0 - clamp((fogEnd - fogCoord)/(fogEnd - fogStart), 0.0, 1.0);
	vec4 fogColor = vec4(0.678, 0.847, 0.902, 1.0) / 2.0;
	vec4 outputColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0f);
	color = mix(outputColor, fogColor, vec4(fogFactor));
}