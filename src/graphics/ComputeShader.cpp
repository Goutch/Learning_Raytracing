//
// Created by User on 16-Jul.-2020.
//

#include "ComputeShader.h"
#include "glad/glad.h"
ComputeShader::ComputeShader(std::string path) : ShaderProgram() {
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &max_work_group_x);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &max_work_group_y);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &max_work_group_z);

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0,&max_work_group_size_x);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1,&max_work_group_size_y);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2,&max_work_group_size_z);

    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &max_work_group_invocations);

    std::string source_cs;
    source_cs = getSource(path);
    unsigned int cs = compileShader(GL_COMPUTE_SHADER, source_cs);

    glAttachShader(program_id, cs);
    glLinkProgram(program_id);
    glValidateProgram(program_id);
    glDeleteShader(cs);
}

void ComputeShader::setGroups(int x, int y, int z, int size_x, int size_y, int size_z) {
    work_group_x=x;
    work_group_y=y;
    work_group_z=z;
    work_group_size_x=size_x;
    work_group_size_y=size_y;
    work_group_size_z=size_z;
}



void ComputeShader::execute() {
    glDispatchCompute((GLuint)work_group_x, (GLuint)work_group_y,(GLuint)work_group_z);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}
