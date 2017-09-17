#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec4 position4D;


// Texture samplers
layout (binding=2) uniform sampler2D currentTextureColor;

uniform mat4 model;
uniform mat4 ViewProjection;
uniform mat4 projection;
uniform mat4 MVP;

out vec2 TexCoord;
out float vertCol;
out vec4 TexColor;

subroutine vec4 getImagePosition();
subroutine uniform getImagePosition getPositionSelection;

subroutine(getImagePosition)
vec4 fromVertex3D()
{
	if (position4D.x == 2 || position4D.x == 0)
	{
		return vec4(0.0f,0.0f,0.0f,0.0f);
	}
	else
	{
		return vec4(ViewProjection * vec4(position4D.xyz, 1.0f));
		
	}
}

subroutine(getImagePosition)
vec4 fromDepth2D()
{
	return vec4(MVP * vec4(position, 1.0f));
}

void main()
{
	gl_Position = getPositionSelection();
	//gl_PointSize = -position4D.z / 500.0f;
	gl_PointSize = 2.0f;
	// We swap the y-axis by substracing our coordinates from 1. This is done because most images have the top y-axis inversed with OpenGL's top y-axis.
	//TexCoord = texCoord;
	TexCoord = vec2(texCoord.x, 1 - texCoord.y);
	TexColor = vec4(texture(currentTextureColor, vec2(texCoord.x, 1 - texCoord.y)));

	vertCol = -position.z;
}