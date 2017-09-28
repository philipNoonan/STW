#version 430 core
layout (location = 0) in vec3 positionColor;
layout (location = 1) in vec2 texCoordColor;
layout (location = 2) in vec3 positionDepth;
layout (location = 3) in vec2 texCoordDpeth;
layout (location = 4) in vec4 position4D;
layout (location = 5) in vec2 position2D;
//layout (location = 6) in vec4 color4D;

// Texture samplers
layout (binding=2) uniform sampler2D currentTextureColor;

uniform mat4 model;
uniform mat4 ViewProjection;
uniform mat4 projection;
uniform mat4 MVP;

out vec2 VTexCoord;
out float VvertCol;
out vec4 VTexColor;
out vec4 Vvert4D;
out int VTextureType;

int TextureType = 0;

subroutine vec4 getImagePosition();
subroutine uniform getImagePosition getPositionSelection;

subroutine(getImagePosition)
vec4 fromVertex3D()
{
	if (position4D.x == 2 || position4D.z == 0 || position4D.z == 4000.0f)
	{
		return vec4(0.0f,0.0f,20000.0f,0.0f); // hack to discard verts that we dont want, since they are beyond the clip plane now
	}
	else
	{
		return vec4(MVP * vec4(position4D.xyz, 1.0f));
		
	}
}

subroutine(getImagePosition)
vec4 fromDepth2D()
{
	TextureType = 1;
	return vec4(MVP * vec4(positionDepth, 1.0f));
}

subroutine(getImagePosition)
vec4 fromColor2D()
{
	TextureType = 1;
	return vec4(MVP * vec4(positionColor, 1.0f));
}

subroutine(getImagePosition)
vec4 fromPosition2D()
{
	return vec4(MVP * vec4(position2D.x, position2D.y, 0.0f, 1.0f));
}

void main()
{
	gl_Position = getPositionSelection();
	//gl_PointSize = -position4D.z / 500.0f;
	//gl_PointSize = 2.0f;
	// We swap the y-axis by substracing our coordinates from 1. This is done because most images have the top y-axis inversed with OpenGL's top y-axis.
	//TexCoord = texCoord;
	VTexCoord = vec2(texCoordColor.x, 1 - texCoordColor.y);
	VTexColor = vec4(texture(currentTextureColor, vec2(texCoordColor.x, 1 - texCoordColor.y)));
	//TexColor = color4D;
	VvertCol = position4D.z;
	Vvert4D = position4D;
	VTextureType = TextureType;
}