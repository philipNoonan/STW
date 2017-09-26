// input current vertex array (2d with float 3)
// input previous vertex array (2d with float 3)

// output depth to display

// if depth previous decay value < thresh
//  // output current depth.xyz, 1.0

// if depth current < depth previous
//  // output depth current

// if depth current > depth previous 
//  // output depth previous.xyz, 1.0 - decay value

#version 430

layout(local_size_x = 32, local_size_y = 32) in;

layout(binding= 0, rgba8) uniform image2D ColorTexturePrevious;
layout(binding= 1, rgba8) uniform image2D ColorTextureCurrent; // original image coming in, read from it, write to both previous and this image

layout(std430, binding= 0) buffer pos3D // ouput
{
    vec4 Position3D [];
};
layout(std430, binding= 1) buffer pos3Dcurrent // current
{
    vec4 Position3D_curr [];
};
layout(std430, binding= 2) buffer pos3Dprevious // previous
{
    vec4 Position3D_prev [];
};

uvec2 pix;
ivec2 size;
vec4 currentVert;
vec4 previousVert;
vec4 currentColor;
vec4 previousColor;

vec4 red = vec4(1.0f, 0.0f, 0.0f, 1.0f);
vec4 green = vec4(0.0f, 1.0f, 0.0f, 1.0f);

void useCurrentValues()
{
    Position3D[(pix.y * size.x) + pix.x] = vec4(currentVert.xyz, 0.01f);
    imageStore(ColorTextureCurrent, ivec2(pix.x, pix.y), vec4(currentColor.xyz, 1.0f));


    Position3D_prev[(pix.y * size.x) + pix.x] = vec4(currentVert.xy, currentVert.z, 0.01f);
    imageStore(ColorTexturePrevious, ivec2(pix.x, pix.y), vec4(currentColor.xyz, 1.0f));
}


void usePreviousValues()
{

    Position3D[(pix.y * size.x) + pix.x] = vec4(previousVert.xy, previousVert.z, previousVert.w + 0.01f);
    imageStore(ColorTextureCurrent, ivec2(pix.x, pix.y), vec4(previousColor.xyz, previousColor.w - 0.01f));


    Position3D_prev[(pix.y * size.x) + pix.x] = vec4(previousVert.xy, previousVert.z, previousVert.w + 0.01f);
    imageStore(ColorTexturePrevious, ivec2(pix.x, pix.y), vec4(previousColor.xyz, previousColor.w - 0.01f));


}



void main()
{
    pix = gl_GlobalInvocationID.xy;
    size = ivec2(1920, 1080);


    if (pix.x < size.x && pix.y < size.y)
    {
        //Position3D[(pix.y * size.x) + pix.x] = Position3D_curr[(pix.y * size.x) + pix.x];

        currentVert = Position3D_curr[(pix.y * size.x) + pix.x];
        previousVert = Position3D_prev[(pix.y * size.x) + pix.x];

        currentColor = imageLoad(ColorTextureCurrent, ivec2(pix));
        previousColor = imageLoad(ColorTexturePrevious, ivec2(pix));


        //vec4 currentColor = Color3D_curr[(pix.y * size.x) + pix.x];
        // vec4 previousColor = Color3D_prev[(pix.y * size.x) + pix.x];

        //if (currentVert.z < (previousVert.z))
        //{
        //    Position3D[(pix.y * size.x) + pix.x] = vec4(currentVert.xyz, 0.0f);
        //    Color3D[(pix.y * size.x) + pix.x] = vec4(currentColor.xyz, 1.0f);

        //    Position3D_prev[(pix.y * size.x) + pix.x] = vec4(currentVert.xyz, 0.0f);
        //    Color3D_prev[(pix.y * size.x) + pix.x] = vec4(currentColor.xyz, 1.0f);
        //}
        //else
        //{

        // for every pixel 
        // am i fresh?
        // yes : copy color and vert position to prev frame and output frame
        // // no : check current distance to previous z
        // no : copy previous vert and colour to output
        // vec4(1.0f,0.0f,0.0f,1.0f);

        if (previousVert.w > 0.2f)
        {
            useCurrentValues();
        }
        else
        {

            if (previousVert.z == 0 || previousVert.z > 4000.0f)
            {
                previousVert.z = 4000.0f;
            }

            if (currentVert.z < (previousVert.z + 5.0f))
            {
                useCurrentValues();
            }
            else
            {
                if (previousVert.w >= 0.01f) // newly not a foreground pixel
                {
                    usePreviousValues();
                }
            }

            if (currentVert.z == 0 || currentVert.z > 4000) // this was the problem!!!!
            {
                usePreviousValues();
            }
        }



        //    if (previousVert.w >= 0.2f)
        //    {
        //        useCurrentValues();
        //    }
        //    else
        //    {
        //        usePreviousValues();
        //    }
        //}














            //if (currentVert.z != 0 && currentVert.z < 8000.0f)
            //{
            //    if (previousVert.w == 0)
            //    {
            //        useCurrentValues();
            //    }
            //    else// if (previousVert.w > 0 && previousVert.w <= 0.2f)
            //    {
            //        if (currentVert.z < previousVert.z + 20.0f)
            //        {
            //            useCurrentValues();
            //        }
            //        else
            //        {
            //            usePreviousValues();
            //        }

            //    }
            //}
            //else
            //{
            //    if (previousVert.w > 0)
            //    {
            //        usePreviousValues();
            //    }
            //}







       




    }

}