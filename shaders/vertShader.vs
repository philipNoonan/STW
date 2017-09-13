#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 MVP;

out vec2 TexCoord;

subroutine vec4 getImagePosition();
subroutine uniform getImagePosition getPositionSelection;

subroutine(getImagePosition)
vec4 fromVertex3D()
{
	return vec4(projection * vec4(position, 1.0f));
}

subroutine(getImagePosition)
vec4 fromDepth2D()
{
	return vec4(MVP * vec4(position, 1.0f));
}

void main()
{
	//vec4 tPos = getPositionSelection();
	//gl_Position = tPos;
	gl_Position = MVP * vec4(position, 1.0f);
	// gl_Position = vec4(position, 1.0f);
	// We swap the y-axis by substracing our coordinates from 1. This is done because most images have the top y-axis inversed with OpenGL's top y-axis.
	//TexCoord = texCoord;
	TexCoord = vec2(texCoord.x, 1 - texCoord.y);
}