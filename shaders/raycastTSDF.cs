#version 430

layout(local_size_x = 32, local_size_y = 32) in;

layout(binding= 0) uniform isampler3D volumeDataTexture;

layout(binding = 0, rg16i) uniform iimage3D volumeData; // Gimage3D, where G = i, u, or blank, for int, u int, and floats respectively

layout(std430, binding= 0) buffer pos3D // ouput
{
    vec4 PositionTSDF [];
};

layout(std430, binding= 4) buffer norm3D // ouput
{
    vec4 NormalTSDF [];
};


uniform mat4 view; // == raycast pose * invK
uniform float nearPlane;
uniform float farPlane;
uniform float step;
uniform float largeStep;
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

vec4 raycast(uvec2 pos)
{
    vec3 origin = vec3(view[0].w, view[1].w, view[2].w);
    vec3 direction = rotate(view, vec3(pos.x, pos.y, 1.0f));

    // intersect ray with a box
    // http://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm
    // compute intersection of ray with all six bbox planes
    vec3 invR = vec3(1.0f, 1.0f, 1.0f) / direction;
    vec3 tbot = -1.0f * invR * origin;
    vec3 ttop = invR * (volDim - origin);
    // re-order intersections to find smallest and largest on each axis
    vec3 tmin = min(ttop, tbot);
    vec3 tmax = max(ttop, tbot);
    // find the largest tmin and the smallest tmax
    float largest_tmin = max(max(tmin.x, tmin.y), max(tmin.x, tmin.z));
    float smallest_tmax = min(min(tmax.x, tmax.y), min(tmax.x, tmax.z));
    // check against near and far plane
    float tnear = max(largest_tmin, nearPlane);
    float tfar = min(smallest_tmax, farPlane);

    if (tnear < tfar)
    {
        // first walk with largesteps until we found a hit
        float t = tnear;
        float stepsize = largeStep;
        //float f_t = volume.interp(origin + direction * t); // do a sampler3D?

        ivec4 interpData = texture(volumeDataTexture, vec3(origin + direction * t));
        float f_t = interpData.x * 0.00003051944088f;


        float f_tt = 0;
        if (f_t > 0)
        {  // ups, if we were already in it, then don't render anything here
            for (; t < tfar; t += stepsize)
            {
                //f_tt = volume.interp(origin + direction * t); // another sampler3d
                ivec4 interpData = texture(volumeDataTexture, vec3(origin + direction * t));
                f_tt = interpData.x * 0.00003051944088f;
                if (f_tt < 0) // got it, jump out of inner loop
                {
                    break; // 
                }
                if (f_tt < 0.8f)
                {
                    stepsize = step;
                }
                f_t = f_tt;
            }
            if (f_tt < 0) // got it, calculate accurate intersection
            {
                t = t + stepsize * f_tt / (f_t - f_tt);
                return vec4(origin + direction * t, t);
            }
        }
    }

    return vec4(0.0f, 0.0f, 0.0f, 0.0f);

}

float vs(uvec3 pos)
{
    vec4 data = imageLoad(volumeData, ivec3(pos));
    return data.x * 0.00003051944088f; // convert short to float
}

vec3 getGradient(vec4 hit)
{
    vec3 scaled_pos = vec3((hit.x * volSize.x / volDim.x) - 0.5f, (hit.y * volSize.y / volDim.y) - 0.5f, (hit.z * volSize.z / volDim.z) - 0.5f);
    ivec3 baseVal = ivec3(floor(scaled_pos));
    vec3 factor = fract(scaled_pos);
    ivec3 lower_lower = max(baseVal - ivec3(1), ivec3(0));
    ivec3 lower_upper = max(baseVal, ivec3(0));
    ivec3 upper_lower = min(baseVal + ivec3(1), ivec3(volSize) - ivec3(1));
    ivec3 upper_upper = min(baseVal + ivec3(2), ivec3(volSize) - ivec3(1));
    ivec3 lower = lower_upper;
    ivec3 upper = upper_lower;

    vec3 gradient;

    gradient.x =
              (((vs(uvec3(upper_lower.x, lower.y, lower.z)) - vs(uvec3(lower_lower.x, lower.y, lower.z))) * (1 - factor.x)
            + (vs(uvec3(upper_upper.x, lower.y, lower.z)) - vs(uvec3(lower_upper.x, lower.y, lower.z))) * factor.x) * (1 - factor.y)
            + ((vs(uvec3(upper_lower.x, upper.y, lower.z)) - vs(uvec3(lower_lower.x, upper.y, lower.z))) * (1 - factor.x)
            + (vs(uvec3(upper_upper.x, upper.y, lower.z)) - vs(uvec3(lower_upper.x, upper.y, lower.z))) * factor.x) * factor.y) * (1 - factor.z)
            + (((vs(uvec3(upper_lower.x, lower.y, upper.z)) - vs(uvec3(lower_lower.x, lower.y, upper.z))) * (1 - factor.x)
            + (vs(uvec3(upper_upper.x, lower.y, upper.z)) - vs(uvec3(lower_upper.x, lower.y, upper.z))) * factor.x) * (1 - factor.y)
            + ((vs(uvec3(upper_lower.x, upper.y, upper.z)) - vs(uvec3(lower_lower.x, upper.y, upper.z))) * (1 - factor.x)
            + (vs(uvec3(upper_upper.x, upper.y, upper.z)) - vs(uvec3(lower_upper.x, upper.y, upper.z))) * factor.x) * factor.y) * factor.z;

    gradient.y =
          (((vs(uvec3(lower.x, upper_lower.y, lower.z)) - vs(uvec3(lower.x, lower_lower.y, lower.z))) * (1 - factor.x)
        + (vs(uvec3(upper.x, upper_lower.y, lower.z)) - vs(uvec3(upper.x, lower_lower.y, lower.z))) * factor.x) * (1 - factor.y)
        + ((vs(uvec3(lower.x, upper_upper.y, lower.z)) - vs(uvec3(lower.x, lower_upper.y, lower.z))) * (1 - factor.x)
        + (vs(uvec3(upper.x, upper_upper.y, lower.z)) - vs(uvec3(upper.x, lower_upper.y, lower.z))) * factor.x) * factor.y) * (1 - factor.z)
        + (((vs(uvec3(lower.x, upper_lower.y, upper.z)) - vs(uvec3(lower.x, lower_lower.y, upper.z))) * (1 - factor.x)
        + (vs(uvec3(upper.x, upper_lower.y, upper.z)) - vs(uvec3(upper.x, lower_lower.y, upper.z))) * factor.x) * (1 - factor.y)
        + ((vs(uvec3(lower.x, upper_upper.y, upper.z)) - vs(uvec3(lower.x, lower_upper.y, upper.z))) * (1 - factor.x)
        + (vs(uvec3(upper.x, upper_upper.y, upper.z)) - vs(uvec3(upper.x, lower_upper.y, upper.z))) * factor.x) * factor.y) * factor.z;

    gradient.z =
          (((vs(uvec3(lower.x, lower.y, upper_lower.z)) - vs(uvec3(lower.x, lower.y, lower_lower.z))) * (1 - factor.x)
        + (vs(uvec3(upper.x, lower.y, upper_lower.z)) - vs(uvec3(upper.x, lower.y, lower_lower.z))) * factor.x) * (1 - factor.y)
        + ((vs(uvec3(lower.x, upper.y, upper_lower.z)) - vs(uvec3(lower.x, upper.y, lower_lower.z))) * (1 - factor.x)
        + (vs(uvec3(upper.x, upper.y, upper_lower.z)) - vs(uvec3(upper.x, upper.y, lower_lower.z))) * factor.x) * factor.y) * (1 - factor.z)
        + (((vs(uvec3(lower.x, lower.y, upper_upper.z)) - vs(uvec3(lower.x, lower.y, lower_upper.z))) * (1 - factor.x)
        + (vs(uvec3(upper.x, lower.y, upper_upper.z)) - vs(uvec3(upper.x, lower.y, lower_upper.z))) * factor.x) * (1 - factor.y)
        + ((vs(uvec3(lower.x, upper.y, upper_upper.z)) - vs(uvec3(lower.x, upper.y, lower_upper.z))) * (1 - factor.x)
        + (vs(uvec3(upper.x, upper.y, upper_upper.z)) - vs(uvec3(upper.x, upper.y, lower_upper.z))) * factor.x) * factor.y) * factor.z;

    return gradient * vec3(volDim.x / volSize.x, volDim.y / volSize.y, volDim.z / volSize.z) * (0.5f * 0.00003051944088f);


}

void main()
{
    ivec2 depthSize = imageSize(depthImage);
    uvec2 pix = gl_GlobalInvocationID.xy;



    vec4 hit = raycast(pix);

    //barrier(); // do we need this???can we read from an imageLoad and sampler3D on same texture?



    if (hit.w > 0)
    {
        PositionTSDF[(pix.y * depthSize.x) + pix.x] = hit;

        vec3 surfNorm = getGradient(hit);// volume.grad(make_float3(hit));
        if (length(surfNorm) == 0)
        {
            NormalTSDF[(pix.y * depthSize.x) + pix.x] = vec4(0);
        }
        else
        {
            NormalTSDF[(pix.y * depthSize.x) + pix.x] = vec4(normalize(surfNorm), 0.0f);
        }
    }
    else
    {
        PositionTSDF[(pix.y * depthSize.x) + pix.x] = vec4(0);
        NormalTSDF[(pix.y * depthSize.x) + pix.x] = vec4(0, 0, 0, 0); // set x = 2??
    }


    //vec3 pos = opMul(invTrack, getVolumePosition(pix));
    //vec3 cameraX = opMul(K, pos);
    //vec3 delta = rotate(invTrack, vec3(0.0f, 0.0f, volDim.z / volSize.z));
    //vec3 cameraDelta = rotate(K, delta);
}

