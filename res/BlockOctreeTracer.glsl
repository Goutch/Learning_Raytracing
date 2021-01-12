#version 430

#define BLOCK_SIZE 16
#define MAX_DIST 64.
layout(local_size_x = 32, local_size_y = 32) in;
struct Node{
    bool is_leaf;
    uint child_pointer;
};
struct Block{
    int materials[BLOCK_SIZE][BLOCK_SIZE][BLOCK_SIZE];
};
layout(std430, binding = 0) buffer Nodes_input{ Node nodes[]; };
layout(std430, binding =1) buffer Blocks_input{ Block blocks[]; };
layout(rgba32f, binding = 0) uniform image2D color_output;
uniform vec2 resolution;
uniform mat4 view_transform;

uniform int chunk_size;
bool isInside(vec3 p, vec3 box_position, vec3 box_radius)
{
    vec3 q = abs(p-box_position) - box_radius;
    return 0.>length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}
vec3 getBoxDistances(vec3 o, vec3 d, vec3 box_position, vec3 box_radius, float max_dist, ivec3 axis_directions, out int min_index)
{
    min_index=-1;
    vec3 min=box_position-box_radius;
    vec3 max=box_position+box_radius;

    vec3 next_planes=box_position + (box_radius*axis_directions);

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
vec4 trace(vec3 o, vec3 d)
{
    float t=0.;
    //make sur no divisions by 0
    if (abs(d.x)<0.00001)d.x=0.00001;
    if (abs(d.y)<0.00001)d.y=0.00001;
    if (abs(d.z)<0.00001)d.z=0.00001;

    ivec3 axis_directions=ivec3(
    d.x>0?1:-1,
    d.y>0?1:-1,
    d.z>0?1:-1);
    //figure out if the origin is outside the chunk.
    if(!isInside(o,vec3(0.,0.,0.),vec3(chunk_size/2.)))
    {
        int min_dist_index;
        t=getBoxDistances(o, d, vec3(0, 0, 0), vec3(chunk_size/2.), MAX_DIST, -axis_directions, min_dist_index)[min_dist_index];
    }
    return vec4(t/MAX_DIST,0.,0.,1.);

}
void main() {
    uvec3 id=gl_WorkGroupID * gl_WorkGroupSize + gl_LocalInvocationID;
    ivec2 pixel_coords = ivec2(id.xy);
    vec2 uv=id.xy/resolution.xy;
    uv-=0.5;
    float aspect_ratio=resolution.y/resolution.x;
    uv.y*=aspect_ratio;
    vec3 origin=view_transform[3].xyz;
    vec3 dir= vec3(uv.x, uv.y, 1.);
    dir=normalize(dir);
    dir=mat3(view_transform)*dir;
    vec4 pixel_color = trace(origin,dir);
    imageStore(color_output, pixel_coords, pixel_color);
}
