#version 430

layout (local_size_x = 32, local_size_y = 32) in;

uniform mat4 invK;

layout(binding=0, r32f) uniform image2D InputImage;
layout(binding=4, rgba32f) uniform image2D OutputImage;

vec3 rotate(mat4 inputMat4, vec3 inputVec3)
{
    return vec3(dot(inputMat4[0].xyz, inputVec3),
                dot(inputMat4[1].xyz, inputVec3),
                dot(inputMat4[2].xyz, inputVec3));
}

void main()
{
    uvec2 pix = gl_GlobalInvocationID.xy;

    if (pix.x < 512 && pix.y < 424)
    {
        vec4 depth = imageLoad(InputImage, ivec2(pix));
        imageStore(OutputImage, ivec2(pix.x, pix.y), vec4(depth.x * rotate(invK, vec3(pix.x, pix.y, 1.0f)), 0.0f));
    }

}