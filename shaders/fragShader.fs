#version 430 core
const float PI = 3.1415926535897932384626433832795f;


in vec2 TexCoord;
in float vertCol;
in vec4 TexColor;

layout(location = 0) out vec4 color;

uniform mat4 model;
uniform vec3 ambient;
uniform vec3 light;
uniform float irBrightness = 1.0f;

// Texture samplers
layout (binding=0) uniform sampler2D currentTextureDepth;
layout (binding=1) uniform sampler2D currentTextureInfra;
layout (binding=2) uniform sampler2D currentTextureColor;
layout (binding=3) uniform sampler2D currentTextureFlow;
layout (binding=4) uniform sampler2D currentTextureVertex;
layout (binding=5) uniform sampler2D currentTextureNormal;

subroutine vec4 getColor();
subroutine uniform getColor getColorSelection;

subroutine(getColor)
vec4 fromDepth()
{
	vec4 redChnl = texture(currentTextureDepth, TexCoord);
	return vec4(redChnl.x/5000.0f, redChnl.x / 5000.0f, redChnl.x / 5000.0f, 1.0f);
}

subroutine(getColor)
vec4 fromColor()
{
	return vec4(texture(currentTextureColor, TexCoord));
}

subroutine(getColor)
vec4 fromFlow()
{
	vec4 tFlow = texture(currentTextureFlow, TexCoord);
	//vec4 tDep = texture(currentTextureDepth, vec2(TexCoord.x + (tFlow.x / 1920.0f), TexCoord.y + (tFlow.y / 1080.0f)));

	// cart to polar
	// sqrt(x^2 + y^2) = magnitude
	// atan (y / x) = angle from x axis

	float x2 = tFlow.x * tFlow.x;
	float y2 = tFlow.y * tFlow.y;

	float mag = sqrt(x2 + y2);

	float ang;

	float tmp;
	if (tFlow.y >= 0.0f)
	{
		tmp = 0.0f;
	}
	else
	{
		tmp = PI * 2.0f; // take all these PI multiplications out of the shader!!!
	}

	if (tFlow.x < 0)
	{
		tmp = PI;
	}

	float tmp1;
	if (tFlow.y >= 0.0f)
	{
		tmp1 = PI * 0.5f;
	}
	else
	{
		tmp1 = PI * 1.5f;
	}

	if (y2 < x2)
	{
		ang = (tFlow.x * tFlow.y) / (x2 + 0.28f * y2) + tmp;
	}
	else
	{
		ang = (tmp1 - (tFlow.x * tFlow.y) / (y2 + 0.28f * x2));
	}

	//ang = ang * 180.0f / PI;



	float smoothMag = smoothstep(1.0f, 10.0f, mag);
	


	// ang to rgb taken from https://stackoverflow.com/questions/15095909/from-rgb-to-hsv-in-opengl-glsl
	vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(ang.xxx + K.xyz) * 6.0 - K.www);

    vec3 rgb = mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), mag / 10.0f);

	return vec4(rgb, smoothMag);
	//return vec4(tDep.x/1000.0f, tDep.x / 1000.0f, tDep.x / 1000.0f, 1.0f);

	//return vec4(tFlow.xyz, 1.0f);

}

subroutine(getColor)
vec4 fromInfra()
{
	vec4 redChnl = texture(currentTextureInfra, TexCoord);
	//float tMod = mod(redChnl.x / 10000.0f, 1.0f); 

	return vec4(redChnl.x / irBrightness / 10.0f, redChnl.x / irBrightness / 10.0f, redChnl.x / irBrightness / 10.0f, 1.0f);
}

subroutine(getColor)
vec4 fromVertex()
{

	vec4 tVertex = texture(currentTextureVertex, TexCoord);
	vec4 tNormal = texture(currentTextureNormal, TexCoord);

	//vec3 proj = model * vec4(tVertex.xyz, 1.0f);
	if (tVertex.z > 4000.0f || tNormal.x > 1.5f)
	{
		return vec4(0.0f,0.0f,0.0f,0.0f);
	}
	else
	{
		vec3 diff = normalize(light - tVertex.xyz);
		float dir = max(dot(tNormal.xyz, diff), 0.0f);

		vec3 col = clamp(vec3(dir, dir, dir) + ambient, 0.0f, 1.0f);

		return vec4(col, 1.0f);

	}

	//return vec4(tNormal.xyz, 1.0f);

}

subroutine(getColor)
vec4 fromPoints()
{
	float tMod = mod(vertCol / 100.0f, 1.0f); 
	//if (vertCol > 0)
	//{
		//vec4 tCol = vec4(texture(currentTextureColor,vec2(TexCoord.x, TexCoord.y)));
		return vec4(TexColor.xyz, 1.0f);
		//		return vec4(1.0f, 0.0f,  0.0f, 1.0f);


	//}
	//else
	//{
	//	return vec4(1.0f,0.0f,0.0f,1.0f);
	//}
}

subroutine(getColor)
vec4 fromCalibration()
{
	return vec4(1.0f, 0.0f, 0.0f, 1.0f);
}

void main()
{
	color = getColorSelection();
}