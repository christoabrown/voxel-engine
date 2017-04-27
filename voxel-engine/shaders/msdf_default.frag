#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;
uniform float scale;

#define SQRT2_2 0.70710678118654757
#define SQRT2 1.414213562373

float median(float r, float g, float b) {
	return max(min(r, g), min(max(r, g), b));
}

void main()
{
	vec3 sampled = texture(text, TexCoords).rgb;

	ivec2 sz = textureSize( text, 0 );
	vec2 p = TexCoords.xy;
	vec2 dx = dFdx(p) * sz.x;
	vec2 dy = dFdy(p) * sz.y;
	float m = length (vec2 (length (dx), length (dy))) * SQRT2_2; 
	//I have no idea why these constants work well
	float sigDist = median( sampled.r, sampled.g, sampled.b ) / m - ((SQRT2_2/1.55) * scale);
	float opacity = smoothstep( -0.12, 0.12, sigDist );
	color = vec4(textColor, opacity);
}