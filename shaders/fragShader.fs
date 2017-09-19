#version 430 core
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
	vec4 tDep = texture(currentTextureDepth, vec2(TexCoord.x + (tFlow.x / 1920.0f), TexCoord.y + (tFlow.y / 1080.0f)));

	//return vec4(tFlow.x * tFlow.y, 1.0f, 1.0f, 1.0f);
	return vec4(tDep.x/1000.0f, tDep.x / 1000.0f, tDep.x / 1000.0f, 1.0f);
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