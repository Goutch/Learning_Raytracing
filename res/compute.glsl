#version 430
#define RIGHT 4
#define TOP 2
#define BACK 1
#define MAX_STEP 64
#define MAX_DIST 3.0
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

uint index_stack[MAX_DEPTH]={
0, 0, 0, 0, 0, 0, 0, 0
};
int child_type_stack[MAX_DEPTH-1]={
0, 0, 0, 0, 0, 0, 0
};
vec3 position_stack[MAX_DEPTH]={
vec3(0, 0, 0), vec3(0, 0, 0),
vec3(0, 0, 0), vec3(0, 0, 0),
vec3(0, 0, 0), vec3(0, 0, 0),
vec3(0, 0, 0), vec3(0, 0, 0)
};
int depth=0;

bool isLeaf(uint node_index)
{
    return (node_index & LEAF_FLAG) == LEAF_FLAG;
}

uint getValue(uint node_index)
{
    return node_index & VALUE_MASK;
}
float sizeAt(int depth)
{
    return (1<<(MAX_DEPTH-depth))/voxels_per_units;
}
bool isInside(vec3 p,vec3 box_position,vec3 box_radius)
{
    vec3 q = abs(p-box_position) - box_radius;
    return 0.>length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}
vec3 getBoxDistances(vec3 o, vec3 d, vec3 box_position, vec3 box_radius, ivec3 d_axis, out int min_index)
{
    min_index=-1;
    vec3 min=box_position-box_radius;
    vec3 max=box_position+box_radius;

    vec3 next_planes=box_position + (box_radius*d_axis);

    vec3 ts=(next_planes-o)/d;
    vec3 p=o+(d*ts.x);
    if(ts.x<0||
    p.y < min.y || p.y > max.y||
    p.z < min.z || p.z > max.z)
    {
        ts.x=MAX_DIST;
    }
    p=o+(d*ts.y);
    if(ts.y<0||
    p.x < min.x || p.x > max.x||
    p.z < min.z || p.z > max.z)
    {
        ts.y=MAX_DIST;
    }
    p=o+(d*ts.z);
    if(ts.z<0||
    p.y < min.y || p.y > max.y||
    p.x < min.x || p.x > max.x)
    {
        ts.z=MAX_DIST;
    }
    min_index=ts.x<ts.y&&ts.x<ts.z?0:(ts.y<ts.z?1:2);

    return ts;
}
int getChild(vec3 position, vec3 octree_position, out ivec3 child_position)
{
    child_position=ivec3(
    int(position.x >= octree_position.x),
    int(position.y >= octree_position.y),
    int(position.z >= octree_position.z));
    return (child_position.x * RIGHT) + (child_position.y * TOP) + (child_position.z* BACK);
}
//find closest t to bounding box
//decend the octree to the closest voxel of the found t
//if voxel is not empty juste return its color
//else find next voxel
//get 3 next ts to next planes of this depth
//get the min t
//pop to find common parent
//ounce found, go to child of common parent that contain next voxel.
//repeat
vec4 march(vec3 o, vec3 d)
{
    float t=0.;
    //Init variables
    ivec3 d_axis=ivec3(
    d.x>0?1:-1,
    d.y>0?1:-1,
    d.z>0?1:-1);

    int min_dist_index;
    vec3 distances;
    //Get the t closest to chunk
    if(!isInside(o,vec3(0.,0.,0.),vec3(sizeAt(0)/2.)))
    {
        distances=getBoxDistances(o, d, vec3(0, 0, 0), vec3(sizeAt(0)/2.), -d_axis, min_dist_index);
        t=distances[min_dist_index];
    }


    for (int i=0;i<MAX_STEP;i++)
    {
        //if did not hit anything
        if(t>=MAX_DIST)break;
        if (i==MAX_STEP-1)
        {
            return vec4(0., 0., 0., 1.);
        }
        //Find the closest leaf in current octree
        while (!isLeaf(index_stack[depth]))
        {
            ivec3 child_position;
            int child = getChild(o+(d*t),  position_stack[depth], child_position);
            child_type_stack[depth]=child;
            depth++;
            index_stack[depth]=octree[index_stack[depth-1]].children[child];

            float radius=sizeAt(depth)/2.;

            position_stack[depth] = position_stack[depth-1] + ((child_position)*radius) + ((child_position-1)*radius);
        }

        //if current leaf is not empty return its color
        if (getValue(index_stack[depth])!=0)
        {
            //return vec4(t/MAX_DIST, 0., 0., 1.);
            return materials[getValue(index_stack[depth])];
        }

        //Else find next t

        distances=getBoxDistances(o, d, position_stack[depth], vec3(sizeAt(depth)/2.), d_axis, min_dist_index);
        //PROBLEM SECOND TIME COMMING HERE AT i==1 t IS NOW MAX_DIST

        t=distances[min_dist_index];



        ivec3 move_dir = ivec3(0);
        move_dir[min_dist_index]=d_axis[min_dist_index];

        bool out_of_bound=false;
        bool can_advance;
        ivec3 child_position;
        //find common parent
        do
        {
            if (depth==0)
            {
                out_of_bound=true;
                break;
            }
            depth--;
            child_position=ivec3((child_type_stack[depth]>>2) & 1,(child_type_stack[depth]>>1) & 1, child_type_stack[depth] & 1);
            child_position+=move_dir;
            can_advance=!(
            child_position.x>1||
            child_position.y>1||
            child_position.z>1||
            child_position.x<0||
            child_position.y<0||
            child_position.z<0);

            if (can_advance)
            {
                child_type_stack[depth]=
                ((child_position.x)* RIGHT) +
                ((child_position.y) * TOP) +
                ((child_position.z)* BACK);
            }

        } while (!can_advance);

        if (out_of_bound)
            break;

        //go to next children of common parent
        depth++;
        index_stack[depth] = octree[index_stack[depth-1]].children[child_type_stack[depth-1]];
        position_stack[depth]=position_stack[depth-1]+((child_position)*sizeAt(depth+1)) + ((child_position-1)*sizeAt(depth+1));

    }
    return vec4(1., 1., 1., 1.);
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
