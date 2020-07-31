#version 430

layout(local_size_x = 1, local_size_y = 1) in;
layout(rgba32f, binding = 0) uniform image2D img_output;

uniform vec2 resolution;
uniform mat4 view_matrix;
uniform vec3 position;
const vec4 sphere=vec4(0.,0.,3.,1.);
#define MAX_DIST 30.0
#define STEP 0.1
vec4 trace(vec3 o,vec3 d)
{
    float t;
    while(t<MAX_DIST){
        if(distance((t*d)+o,sphere.xyz)<sphere.w)
        {
            return vec4(1.,1.,1.,1.);
        }
        t+=STEP;
    }
    return vec4(d,1.0);
}
void main() {
    // base pixel colour for image

    // get index in global work group i.e x,y position
    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
    vec2 uv=gl_GlobalInvocationID.xy/resolution;

    vec4 pixel_color = vec4(uv.x,uv.y,0., 1.0);
    vec4 dir= vec4(uv.x-0.5,uv.y-0.5,1.,1.);
    dir=normalize(dir);
    dir=view_matrix*dir;
    //
    // interesting stuff happens here later
    //

    // output to a specific pixel in the image
    imageStore(img_output, pixel_coords, trace(position,dir));
}
