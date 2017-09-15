#version 430

layout (local_size_x = 32, local_size_y = 32) in;

uniform mat4 invK;

layout(binding=1, r32f) uniform image2D InputImage;
layout(binding=2, rgba32f) uniform image2D OutputImage;
layout(std430, binding=0) buffer pos3D
{
    vec4 Position3D[];
};


vec3 rotate(mat4 inputMat4, vec3 inputVec3)
{
    return vec3(dot(inputMat4[0].xyz, inputVec3),
                dot(inputMat4[1].xyz, inputVec3),
                dot(inputMat4[2].xyz, inputVec3));
}

// int oneDindex = (row * length_of_row) + column; // Indexes

void main()
{
    uvec2 pix = gl_GlobalInvocationID.xy;
    ivec2 size = imageSize(InputImage);


    if (pix.x < size.x && pix.y < size.y)
    {
        vec4 depth = imageLoad(InputImage, ivec2(pix));
        //if (depth.x > 0)
        //{
            vec3 tPos = depth.x * rotate(invK, vec3(pix.x, pix.y, 1.0f));
            imageStore(OutputImage, ivec2(pix.x, pix.y), vec4(tPos.xyz, 0.0f));
            Position3D[(pix.y * size.x) + pix.x] = vec4(tPos.x, -tPos.y, -tPos.z, 2.0f);
            //Position3D[(pix.x * size.x) + pix.y] = vec3(pix.x / 100.0f, pix.y / 100.0f, -depth / 100.0f);

        //}
        //else
        //{
         //   imageStore(OutputImage, ivec2(pix.x, pix.y), vec4(0.0f, 0.0f, 0.0f, 0.0f));
         //   Position3D[(pix.x * size.x) + pix.y] = vec4(0, 0, 0, 0);
        //}

    }

}