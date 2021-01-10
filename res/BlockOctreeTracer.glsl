#version 430

#define BLOCK_SIZE 16
struct Block{
    int materials[BLOCK_SIZE][BLOCK_SIZE][BLOCK_SIZE];
};
struct Node{
    bool is_leaf;
    uint child_pointer;
};

layout(std430, binding = 0) buffer Nodes_input
{
    Node nodes[];
};
layout(std430, binding =1) buffer Blocks_input
{
    Block blocks[];
};

float trace(vec3 o,vec3 d)
{
    float t=0.;
    return t;
}
void main() {
    uvec3 id=gl_WorkGroupID * gl_WorkGroupSize + gl_LocalInvocationID;
    ivec2 pixel_coords = ivec2(id.xy);
    vec2 uv=id.xy/resolution.xy;
    uv-=0.5;
    uv*=1.3;
    float aspect_ratio=resolution.y/resolution.x;
    uv.y*=aspect_ratio;
}
