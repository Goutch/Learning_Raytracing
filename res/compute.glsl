#version 430
#define RIGHT 4
#define TOP 2
#define BACK 1
#define LEAF_MASK 0x10000000
#define STEP 0.1
#define CHUNK_SCALE 1.0

struct Node{
    uint type;
    uint parent;
    uint children[8];
};

layout(local_size_x = 1, local_size_y = 1) in;
layout(rgba32f, binding = 0) uniform image2D img_output;
layout(std430, binding = 1) buffer Nodes_input
{
    Node octree[];
};

uniform vec2 resolution;
uniform mat4 view_matrix;
uniform vec4 position;

int getChild(vec3 o, vec3 d, float t, int depth)
{
    vec3 p=(d*t)+o;
    for depth
    float radius = CHUNK_SCALE/float(1 << depth-1);
    p+=radius;
    int x_greater = int(p.x >= radius);
    int y_greater = int(p.y >= radius);
    int z_greater = int(p.z >= radius);
    int child = (x_greater * RIGHT) + (y_greater * TOP) + (z_greater * BACK);
    return child;
}


vec4 trace(vec3 o, vec3 d)
{
    float t=0;
    int current_node=0;
    int depth=0;
    vec3 p=o;
    float bound=CHUNK_SCALE;
    while (t<CHUNK_SCALE){
        //go deeper until a leaf is found.
        while (current_node < LEAF_MASK)
        {
            depth +=1;

            current_node = octree[current_node].children[getChild(o,d,t,depth)];
        }
        //if it is air go next
        if(current_node-LEAF_MASK==0)
        {
            //reverse once position is outside of bounding.
            while ( abs(p.x) > bound ||
            abs(p.y) > bound ||
            abs(p.z) > bound)
            {
                //if outside of octree return default color
                depth -= 1;
                if (depth == -1)
                {
                    return vec4(d, 1.0);
                }
                current_node=octree[octree[current_node].parent];
            }
        }
        else
        {
            return vec4(1.,1.,1.,1.);
        }
    }
    return vec4(d, 1.0);
}

void main() {
    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
    vec2 uv=gl_GlobalInvocationID.xy/resolution;

    vec4 pixel_color = vec4(uv.x, uv.y, 0., 1.0);
    vec4 dir= vec4(uv.x-0.5, uv.y-0.5, 1., 1.);
    dir=normalize(dir);
    dir=view_matrix*dir;


    // output to a specific pixel in the image
    imageStore(img_output, pixel_coords, trace(position.xyz, dir.xyz));
}
