#version 430
#define RIGHT 4
#define TOP 2
#define BACK 1

#define MAX_DIST 256
#define AO_MAX_DIST 0.3
#define NB_AO_RAY 2
#define MAX_STEP 64
#define DEPTH_LIMIT 10
#define AMBIENT_LIGHT 0.2
#define GOLDEN_RATIO 1.61803398875
const uint LEAF_FLAG= 0x10000000;
const uint VALUE_MASK =0xEFFFFFFF;


struct Node{
    uint children[8];
};

layout(local_size_x = 32, local_size_y = 32) in;
layout(rgba32f, binding = 0) uniform image2D color_output;
layout(rgba32f, binding = 1) uniform image2D normal_output;
layout(binding = 2) uniform sampler2D noise_texture;
layout(std430, binding = 0) buffer Nodes_input
{
    Node octree[];
};

layout(std430, binding = 1) buffer Materials_input
{
    vec4 materials[];
};
uniform vec2 resolution;
uniform mat4 view_transform;

uniform float voxels_per_units;
uniform float time;


uint index_stack[DEPTH_LIMIT+1];
int child_type_stack[DEPTH_LIMIT];
vec3 position_stack[DEPTH_LIMIT];
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
    return (1<<(DEPTH_LIMIT-depth))/voxels_per_units;
}
bool isInside(vec3 p, vec3 box_position, vec3 box_radius)
{
    vec3 q = abs(p-box_position) - box_radius;
    return 0.>length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}
vec3 getBoxDistances(vec3 o, vec3 d, vec3 box_position, vec3 box_radius, float max_dist, ivec3 d_axis, out int min_index)
{
    min_index=-1;
    vec3 min=box_position-box_radius;
    vec3 max=box_position+box_radius;

    vec3 next_planes=box_position + (box_radius*d_axis);

    vec3 ts=(next_planes-o)/d;
    vec3 p=o+(d*ts.x);
    if (ts.x<0||
    p.y < min.y || p.y > max.y||
    p.z < min.z || p.z > max.z)
    {
        ts.x=max_dist;
    }
    p=o+(d*ts.y);
    if (ts.y<0||
    p.x < min.x || p.x > max.x||
    p.z < min.z || p.z > max.z)
    {
        ts.y=max_dist;
    }
    p=o+(d*ts.z);
    if (ts.z<0 ||
    p.y < min.y || p.y > max.y||
    p.x < min.x || p.x > max.x)
    {
        ts.z=max_dist;
    }
    min_index=ts.x<ts.y&&ts.x<ts.z?0:(ts.y<ts.z?1:2);

    return ts;
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
vec4 trace(vec3 o, vec3 d, float max_distance, out float t, out vec3 normal)
{
    t=0.;
    depth=0;

    position_stack[0]=vec3(0.,0.,0.);
    index_stack[0]=0;
    child_type_stack[0]=0;
    //Init variables
    if (abs(d.x)<0.00001)d.x=0.00001;
    if (abs(d.y)<0.00001)d.y=0.00001;
    if (abs(d.z)<0.00001)d.z=0.00001;
    //o+=d*t;
    ivec3 d_axis=ivec3(
    d.x>0?1:-1,
    d.y>0?1:-1,
    d.z>0?1:-1);

    int min_dist_index;
    vec3 distances;
    ivec3 move_dir=ivec3(0, 0, 0);
    //Get the t closest to chunk
    if (!isInside(o, vec3(0., 0., 0.), vec3(sizeAt(0)/2.)))
    {
        distances=getBoxDistances(o, d, vec3(0, 0, 0), vec3(sizeAt(0)/2.), max_distance, -d_axis, min_dist_index);
        t=distances[min_dist_index];
        move_dir[min_dist_index]=d_axis[min_dist_index];
    }
    vec3 ray_position=o+(d*t);
    int i=0;
    int j=0;
    //**********PUSH*************
    for (i=0;i<MAX_STEP;i++)
    {
        //if did not hit anything
        if (t>=max_distance)break;
        //Find the closest leaf in current octree
        ivec3 child_position;
        vec3 position;
        uint parent;
        uint current;
        float radius;
        for (depth;depth<=DEPTH_LIMIT;depth++)
        {
            j++;
            current=index_stack[depth];
            position=position_stack[depth];
            radius=sizeAt(depth+1)/2.;
            if (isLeaf(current))
            break;

            child_position=ivec3(
            int(ray_position.x >=position.x),
            int(ray_position.y >=position.y),
            int(ray_position.z >=position.z));
            int child = (child_position.x * RIGHT) + (child_position.y * TOP) + (child_position.z* BACK);

            child_type_stack[depth]=child;
            index_stack[depth+1]=octree[current].children[child];
            position_stack[depth+1] = position + (child_position*radius) + ((child_position-1)*radius);
        }

        //**********ADVANCE*************
        current=index_stack[depth];
        position=position_stack[depth];
        radius=sizeAt(depth)/2.;
        //if current leaf is not empty return its color
        if (getValue(current)!=0)
        {
            //return vec4(t/MAX_DIST, 0., 0., 1.);
            normal=-move_dir;
            return materials[getValue(current)];
        }
        //Else find next t

        distances=getBoxDistances(o, d, position, vec3(radius), max_distance, d_axis, min_dist_index);

        t=distances[min_dist_index];
        ray_position=o+(d*t);


        move_dir = ivec3(0);
        move_dir[min_dist_index]=d_axis[min_dist_index];

        bool out_of_bound=false;
        bool can_advance;

        //**********POP*************
        //find common parent
        do
        {
            j++;
            if (depth==0)
            {
                out_of_bound=true;
                break;
            }
            depth--;
            child_position=ivec3((child_type_stack[depth]>>2) & 1, (child_type_stack[depth]>>1) & 1, child_type_stack[depth] & 1);
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
                break;
            }

        } while (!can_advance);

        if (out_of_bound)
        break;

        //go to next children of common parent
        depth++;
        index_stack[depth] = octree[index_stack[depth-1]].children[child_type_stack[depth-1]];
        position_stack[depth]=position_stack[depth-1]+((child_position)*sizeAt(depth+1)) + ((child_position-1)*sizeAt(depth+1));

    }
    normal=vec3(0, 0, 0);
    t=max_distance;
    return vec4(j/float(MAX_STEP), j/float(MAX_STEP), j/float(MAX_STEP), 1.);
    return vec4(i/float(MAX_STEP), i/float(MAX_STEP), i/float(MAX_STEP), 1.);
    return vec4(1., 1., 1., 1.);
}

void main() {
    uvec3 id=gl_WorkGroupID * gl_WorkGroupSize + gl_LocalInvocationID;

    ivec2 pixel_coords = ivec2(id.xy);
    vec2 uv=id.xy/resolution.xy;
    uv-=0.5;
    uv*=1.3;
    float aspect_ratio=resolution.y/resolution.x;
    uv.y*=aspect_ratio;

    vec3 dir= vec3(uv.x, uv.y, 1.);
    dir=normalize(dir);
    dir=mat3(view_transform)*dir;

    vec3 view_position=view_transform[3].xyz;

    float t;
    vec3 normal;
    vec4 color=trace(view_position.xyz, dir.xyz, MAX_DIST, t, normal);
    if (t<MAX_DIST)
    {
         vec3 hit_position=view_position.xyz+(dir.xyz*(t-0.00001));
        vec3 shadow_hit_normal;
        vec3 to_light_dir=vec3(cos(1.)*0.5, sin(1.), -0.3);
        if (to_light_dir.y>=0.)
        {
            trace(hit_position, normalize(to_light_dir), MAX_DIST, t, shadow_hit_normal);
            if (t<MAX_DIST)
            {
                color.rgb*=AMBIENT_LIGHT;
                /*float ao=float(NB_AO_RAY);
                for (int i=0;i<NB_AO_RAY;i++)
                {
                    vec3 ao_hit_normal;
                    vec2 uv_x=mod((i/NB_AO_RAY)+(mod(time, 100.)*GOLDEN_RATIO)+uv, 1.);
                    vec2 uv_y=mod((i/NB_AO_RAY)+(mod(time, 100.)*GOLDEN_RATIO)+uv+0.33, 1.);
                    vec2 uv_z=mod((i/NB_AO_RAY)+(mod(time, 100.)*GOLDEN_RATIO)+uv+0.66, 1.);
                    vec3 noise=vec3(
                    texture(noise_texture, uv_x).r,
                    texture(noise_texture, uv_y).r,
                    texture(noise_texture, uv_z).r);

                    vec3 ao_ray_dir=normalize(normal+(((noise-0.5)*2).rgb));
                    trace(hit_position, normalize(ao_ray_dir), AO_MAX_DIST, t, ao_hit_normal);

                    ao-=(-0.1*pow(11, t/AO_MAX_DIST))+1.1;
                }

                color*=max(0.5,(ao/float(NB_AO_RAY)));*/
            }
            else
            {
                color.rgb*=AMBIENT_LIGHT+max(0., dot(normal, to_light_dir));
            }
        }
        else
        {
            color.rgb*=AMBIENT_LIGHT;
        }

    }
    imageStore(color_output, pixel_coords,color);

    imageStore(normal_output, pixel_coords,vec4(abs(normal),1.));
}
