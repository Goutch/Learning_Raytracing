#version 430
#define RIGHT 4
#define TOP 2
#define BACK 1
#define MAX_STEP 100
#define MAX_DIST 100.0
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
int getChild(vec3 position, vec3 octree_position, out ivec3 greater)
{
    greater=ivec3(
    int(position.x >= octree_position.x),
    int(position.y >= octree_position.y),
    int(position.z >= octree_position.z));
    return (greater.x* RIGHT) + (greater.y * TOP) + (greater.z* BACK);
}

vec4 march(vec3 o, vec3 d)
{
    //Init variables
    int depth=0;

    uint index_stack[MAX_DEPTH]={
    0, 0, 0, 0, 0, 0, 0, 0
    };

    vec3 position_stack[MAX_DEPTH]={
    vec3(0, 0, 0), vec3(0, 0, 0),
    vec3(0, 0, 0), vec3(0, 0, 0),
    vec3(0, 0, 0), vec3(0, 0, 0),
    vec3(0, 0, 0), vec3(0, 0, 0)
    };

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
        if(i==MAX_STEP-1)
        {
            return vec4(1.,0.,1.,0.);
        }
        /*if (t>=MAX_DIST)
        {
            return vec4(0.,1.,1.,0.);
        }*/


        //Go down the tree until find a leaf
        float world_size=(1<<(MAX_DEPTH-depth))/voxels_per_units;
        while (!isLeaf(index_stack[depth]))
        {
            depth++;
            ivec3 greater;
            int child = getChild(p, position_stack[depth-1], greater);
            index_stack[depth]=octree[index_stack[depth-1]].children[child];

            world_size=(1<<(MAX_DEPTH-depth))/voxels_per_units;
            float world_radius=world_size/2.;

            position_stack[depth].x = position_stack[depth-1].x + ((greater.x)*world_radius) + ((greater.x-1)*world_radius);
            position_stack[depth].y = position_stack[depth-1].y + ((greater.y)*world_radius) + ((greater.y-1)*world_radius);
            position_stack[depth].z = position_stack[depth-1].z + ((greater.z)*world_radius) + ((greater.z-1)*world_radius);
        }
        //if leaf found is not empty return green
        if (getValue(index_stack[depth])!=0)
        {
            //return vec4(t/MAX_DIST, 0., 0., 1.);
            return materials[getValue(index_stack[depth])];
        }
        //Else find next t
        distances=getBoxDistances(o+(d*t), d, position_stack[depth], vec3(world_size/2.), d_axis, min_dist_index);

        t+=distances[min_dist_index];
        p=(d*t)+o;
        //return vec4(t/MAX_DIST,0.,0.,1.);
        //return vec4(depth/MAX_DEPTH,0.,0.,1.);

        ivec3 move= ivec3(0);
        move[min_dist_index]=d_axis[min_dist_index];

        vec3 target_octree_pos=position_stack[depth]+(move*world_size);

        vec3 min;
        vec3 max;
        bool out_of_bound=false;
        //go up if parent does not contain target_octree_posiition
        do
        {
            if (depth==0)
            {
                out_of_bound=true;
                break;
            }
            depth--;
            world_size=(1<<(MAX_DEPTH-depth))/voxels_per_units;
            min=position_stack[depth]-(world_size/2.);
            max=position_stack[depth]+(world_size/2.);
        } while ((out_of_bound||
        target_octree_pos.x<min.x||target_octree_pos.x>max.x&&
        target_octree_pos.y<min.y||target_octree_pos.y>max.y&&
        target_octree_pos.z<min.z||target_octree_pos.z>max.z));

        if (out_of_bound)
            break;

        //once common parent found go to parent target position
        ivec3 greater;
        int child =getChild(target_octree_pos, position_stack[depth], greater);

        depth++;
        index_stack[depth] = octree[index_stack[depth-1]].children[child];

        world_size=(1<<(MAX_DEPTH-depth))/voxels_per_units;
        float world_radius=world_size/2.;
        position_stack[depth].x = position_stack[depth-1].x + ((greater.x)*world_radius) + ((greater.x-1)*world_radius);
        position_stack[depth].y = position_stack[depth-1].y + ((greater.y)*world_radius) + ((greater.y-1)*world_radius);
        position_stack[depth].z = position_stack[depth-1].z + ((greater.z)*world_radius) + ((greater.z-1)*world_radius);
        if(isLeaf(index_stack[depth])&&getValue(index_stack[depth])!=0)
        {
            return vec4( position_stack[depth]+0.5,1.);
        }

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
