#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;
uniform float dimScale;
uniform float range;
uniform float fontScale;
uniform vec3 borderColor;
uniform float borderSize;

#define SQRT2_2 0.70710678118654757
#define SQRT2 1.414213562373

float median(float r, float g, float b) {
	return max(min(r, g), min(max(r, g), b));
}

void main()
{
	float smoothing;
	if(dimScale < 1.) smoothing = 1/10.0 * fontScale;
	else smoothing = 1/8.0 * fontScale;
	if(borderSize > 0) smoothing = smoothing*2;
	vec3 sampled = texture(text, TexCoords).rgb;
	ivec2 sz = textureSize( text, 0 );
	vec2 p = TexCoords.xy;
	vec2 dx = dFdx(p) * sz.x;
	vec2 dy = dFdy(p) * sz.y;
	float m = length (vec2 (length (dx), length (dy))) * SQRT2_2; 
	float v = (SQRT2_2 - (1/range)) * dimScale;
	float sigDist = median( sampled.r, sampled.g, sampled.b ) / m - v;

	float opacity = smoothstep( -smoothing, smoothing, sigDist );
	vec4 innerCol = vec4(textColor, opacity);
	vec4 finalTextCol;
	if(borderSize > 0)
	{
		float borderSize_ = borderSize/m/sz.x;
		vec4 borderColor_ = vec4(borderColor, 0.);
		if(sigDist < borderSize) borderColor_ += vec4(0, 0, 0, smoothstep(-borderSize_, borderSize_, sigDist)/m / SQRT2_2);
		float outAlpha = innerCol.a + borderColor_.a*(1 - innerCol.a);
		finalTextCol = vec4(innerCol.rgb*innerCol.a + borderColor_.rgb*borderColor_.a*(1-innerCol.a) / outAlpha * 2, outAlpha);
	}
	else finalTextCol = innerCol;
	color = finalTextCol;
}