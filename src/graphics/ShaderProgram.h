#pragma once

#include <string>
#include "glm/glm.hpp"
#include <unordered_map>
#include <vector>


using namespace glm;

class ShaderProgram {
    mutable std::unordered_map<std::string, int> uniforms;
protected:
    std::string getSource(const std::string &path);

    unsigned int program_id;

    unsigned int compileShader(unsigned int type, const std::string &source);

private:
    void setShaders(std::string vertex_path, std::string fragment_path);

public:
    ShaderProgram();

    ShaderProgram(const std::string &vertexShader, const std::string &fragmentShader);


    ~ShaderProgram();

    void bind() const;

    void unbind() const;

    int uniformLocation(std::string) const;

    void setUniform(std::string name, int i) const;

    void setUniform(std::string name, float f) const;

    void setUniform(std::string name, const glm::vec2 &v) const;

    void setUniform(std::string name, const glm::vec3 &v) const;

    void setUniform(std::string name, const glm::vec4 &v) const;

    void setUniform(std::string name, const glm::mat3 &m) const;

    void setUniform(std::string name, const glm::mat4 &m) const;

    void setUniform(unsigned int location, int i) const;

    void setUniform(unsigned int location, float f) const;

    void setUniform(unsigned int location, const glm::vec2 &v) const;

    void setUniform(unsigned int location, const glm::vec3 &v) const;

    void setUniform(unsigned int location, const glm::vec4 &v) const;

    void setUniform(unsigned int location, const glm::mat3 &m) const;

    void setUniform(unsigned int location, const glm::mat4 &m) const;

    void setUniformIntArray(unsigned int location, int *i, unsigned int count) const;

    void setUniformFloatArray(unsigned int location, float *f, unsigned int count) const;

    void setUniformVec2Array(unsigned int location, const glm::vec2 *v, unsigned int count) const;

    void setUniformVec3Array(unsigned int location, const glm::vec3 *v, unsigned int count) const;

    void setUniformVec4Array(unsigned int location, const glm::vec4 *v, unsigned int count) const;

};

