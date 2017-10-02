#version 430

layout(local_size_x = 32, local_size_y = 32) in;

layout(binding = 0, rg16i) uniform iimage3D volumeData; // Gimage3D, where G = i, u, or blank, for int, u int, and floats respectively
layout(binding = 1, r32f) uniform image2D depthImage;

uniform mat4 invTrack;
uniform mat4 K;
uniform float mu;
uniform float maxWeight;
uniform vec3 volDim;
uniform vec3 volSize;

vec3 getVolumePosition(uvec3 p)
{
    return vec3((p.x + 0.5f) * volDim.x / volSize.x, (p.y + 0.5f) * volDim.y / volSize.y, (p.z + 0.5f) * volDim.z / volSize.z);
}

vec3 rotate(mat4 inputMat4, vec3 inputVec3)
{
    return vec3(dot(inputMat4[0].xyz, inputVec3),
                dot(inputMat4[1].xyz, inputVec3),
                dot(inputMat4[2].xyz, inputVec3));
}

vec3 opMul(mat4 M, vec3 v)
{
    return vec3(
        dot(M[0].xyz, v) + M[0].w,
        dot(M[1].xyz, v) + M[1].w,
        dot(M[2].xyz, v) + M[2].w);
}

vec4 vs(uvec3 pos)
{
    ivec4 data = imageLoad(volumeData, ivec3(pos));
    return vec4(data.x * 0.00003051944088f, data.y, data.zw); // convert short to float
}

void set(uvec3 pos, vec4 data)
{
    imageStore(volumeData, ivec3(pos), ivec4(data.x * 32766.0f, data.y, data.zw));
}

void main()
{
    ivec2 depthSize = imageSize(depthImage);
    uvec3 pix = gl_GlobalInvocationID.xyz;
    vec3 pos = opMul(invTrack, getVolumePosition(pix));
    vec3 cameraX = opMul(K, pos);
    vec3 delta = rotate(invTrack, vec3(0.0f, 0.0f, volDim.z / volSize.z));
    vec3 cameraDelta = rotate(K, delta);


    for (pix.z = 0; pix.z < volSize.z; ++pix.z, pos += delta, cameraX += cameraDelta)
    {
        if (pos.z > 0.0001f) 
        {
            vec2 pixel = vec2(cameraX.x / cameraX.z + 0.5f, cameraX.y / cameraX.z + 0.5f);
            if (pixel.x > 0 || pixel.x < depthSize.x - 1 || pixel.y > 0 || pixel.y < depthSize.y - 1)
            {
                uvec2 px = uvec2(pixel.x, pixel.y);
                vec4 depth = imageLoad(depthImage, ivec2(px));
                depth.x = depth.x / 1000.0f;
                if (depth.x > 0)
                {
                    float diff = (depth.x - cameraX.z) * sqrt(1 + pow(pos.x / pos.z, 2) + pow(pos.y / pos.z, 2));
                    if (diff > -mu)
                    {
                        float sdf = min(1.0f, diff / mu);

                        vec4 data = vs(pix);
                        data.x = clamp((data.y * data.x + sdf) / (data.y + 1), -1.0f, 1.0f);
                        data.y = min(data.y + 1, maxWeight);
                        set(pix, data);

                    }


                }
            }
        }
    }
}
