#version 430
#define RIGHT 4
#define TOP 2
#define BACK 1
#define MAX_STEP 100
#define MAX_DIST 20.0
#define CHUNK_SIZE 256
#define MAX_DEPTH 8
const uint LEAF_FLAG= 0x10000000;
const uint VALUE_MASK =0xEFFFFFFF;


struct Node{
    uint node_type;
    uint children[8];
};

layout(local_size_x = 1, local_size_y = 1) in;
layout(rgba32f, binding = 0) uniform image2D img_output;
layout(std430, binding = 1) buffer Nodes_input
{
    Node octree[];
};

layout(std430, binding = 2) buffer Materials_input
{
    vec4 materials[];
};
uniform vec2 resolution;
uniform mat4 view_rotation;
uniform vec4 view_position;
uniform float voxels_per_units;


bool isLeaf(uint node_index)
{
    return (node_index & LEAF_FLAG) == LEAF_FLAG;
}

uint getValue(uint node_index)
{
    return node_index & VALUE_MASK;
}

vec3 getBoxDistances(vec3 o, vec3 d, vec3 box_position, vec3 box_radius, ivec3 d_axis, out int min_index)
{
    min_index=-1;
    vec3 min=box_position-box_radius;
    vec3 max=box_position+box_radius;

    vec3 next=box_position + (box_radius*d_axis);

    vec3 ts=vec3(MAX_DIST);
    float min_t=MAX_DIST;
    for (int i =0;i<3;i++)
    {
        float t=(next[i]-o[i])/d[i];
        vec3 p=(o+(t*d));
        int j=int(mod(i+1, 3));
        int k=int(mod(i+2, 3));
        if (t>=0.&&
        t<min_t&&
        p[j] > min[j] && p[j] < max[j] &&
        p[k] > min[k] && p[k] < max[k])
        {
            min_t=t;
            ts[i]=t;
            min_index=i;
        }
    }
    return ts;
}

vec4 march(vec3 o, vec3 d)
{
    //Init variables
    int depth=0;

    uint parent_index_stack[MAX_DEPTH]={
    0, 0, 0, 0, 0, 0, 0, 0
    };

    vec3 parent_position_stack[MAX_DEPTH]={
    vec3(0, 0, 0), vec3(0, 0, 0),
    vec3(0, 0, 0), vec3(0, 0, 0),
    vec3(0, 0, 0), vec3(0, 0, 0),
    vec3(0, 0, 0), vec3(0, 0, 0)
    };

    uint current_index=0;
    vec3 world_current_position=vec3(0,0,0);
    ivec3 unit_current_position=ivec3(0,0,0);

    ivec3 d_axis=ivec3(
    d.x>0?1:-1,
    d.y>0?1:-1,
    d.z>0?1:-1);

    int min_dist_index;
    //Get the t closest to chunk
    vec3 distances=getBoxDistances(o, d, vec3(0, 0, 0), vec3((CHUNK_SIZE/voxels_per_units)/2.), -d_axis, min_dist_index);

    float t=distances[min_dist_index];

    vec3 p=o+(d*t);

    for (int i=0;i<MAX_STEP;i++)
    {
        //if did not it anything
        if (t>=MAX_DIST)
        break;

        //Go down the tree until found a leaf
        float world_size=CHUNK_SIZE/voxels_per_units;
        int unit_size=CHUNK_SIZE;
        while (!isLeaf(current_index))
        {
            depth++;
            parent_index_stack[depth]=current_index;

            int x_greater = int(p.x >= parent_position_stack[depth-1].x);
            int y_greater = int(p.y >= parent_position_stack[depth-1].y);
            int z_greater = int(p.z >= parent_position_stack[depth-1].z);

            unit_size=1<<(MAX_DEPTH-depth);
            world_size=(unit_size)/voxels_per_units;
            float world_radius=world_size/2.;
            parent_position_stack[depth].x = parent_position_stack[depth-1].x + ((x_greater)*world_radius) + ((x_greater-1)*world_radius);
            parent_position_stack[depth].y = parent_position_stack[depth-1].y + ((y_greater)*world_radius) + ((y_greater-1)*world_radius);
            parent_position_stack[depth].z = parent_position_stack[depth-1].z + ((z_greater)*world_radius) + ((z_greater-1)*world_radius);

            int child = (x_greater * RIGHT) + (y_greater * TOP) + (z_greater * BACK);
            current_index=octree[current_index].children[child];
        }
        //if leaf found is not empty return green
        if (getValue(current_index)!=0)
        {
            //return vec4(t/MAX_DIST, 0., 0., 1.);
            return materials[getValue(current_index)];

        }
        //Else find next t
        distances=getBoxDistances(o+(d*t), d, parent_position_stack[depth], vec3(world_size/2.), d_axis, min_dist_index);
        ivec3 move= ivec3(0);
        move[min_dist_index]=d_axis[min_dist_index];
        ivec3 next =ivec3(x, y, z)+move;
        float unit_radius=1<<(depth-1);
        t+=distances[min_dist_index];
        p=((d*t)+o);
        //return vec4(t/MAX_DIST,0.,0.,1.);
        //go up if parent is not common
        while ( mod(next.x, 2)!=mod(x, 2)&&
                mod(next.y, 2)!=mod(y, 2)&&
                mod(next.z, 2)!=mod(z, 2))
        {
            parent_index_stack[depth]=0;
            depth--;
            next.x=int(mod(floor(next.x/2.), 2));
            next.y=int(mod(floor(next.y/2.), 2));
            next.z=int(mod(floor(next.z/2.), 2));
        }
        x=next.x;
        y=next.y;
        z=next.z;
        current_index = octree[parent_index_stack[depth]].children[(x * RIGHT) + (y * TOP) + (z * BACK)];
    }
    return vec4(t/MAX_DIST, t/MAX_DIST, t/MAX_DIST, 1.);
}

void main() {
    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
    vec2 uv=gl_GlobalInvocationID.xy/resolution.xy;
    uv-=0.5;
    vec4 dir= vec4(uv.x, uv.y, 1., 0.);
    dir=normalize(dir);
    dir=view_rotation*dir;


    imageStore(img_output, pixel_coords, march(view_position.xyz, dir.xyz));
}
