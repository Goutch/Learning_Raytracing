#pragma once

#include "ShaderProgram.h"

class ComputeShader: public ShaderProgram {
    int max_work_group_x;
    int max_work_group_y;
    int max_work_group_z;
    int max_work_group_size_x;
    int max_work_group_size_y;
    int max_work_group_size_z;
    int max_work_group_invocations;
    int work_group_x;
    int work_group_y;
    int work_group_z;
    int work_group_size_x;
    int work_group_size_y;
    int work_group_size_z;
public:
    ComputeShader(std::string path);
    void setGroups(int x,int y,int z,int size_x,int size_y,int size_z);
    void dispatch();
};

