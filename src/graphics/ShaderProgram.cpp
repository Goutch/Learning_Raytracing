#include "glad/glad.h"
#include "ShaderProgram.h"
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

void ShaderProgram::setShaders(std::string vertex_path, std::string fragment_path) {
    std::cout<<vertex_path<<std::endl;
    std::string sourcevs;
    sourcevs = getSource(vertex_path);
    unsigned int vs = compileShader(GL_VERTEX_SHADER, sourcevs);

    std::cout<<vertex_path<<std::endl;
    std::string sourcefs;
    sourcefs = getSource(fragment_path);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, sourcefs);

    glAttachShader(program_id, vs);
    glAttachShader(program_id, fs);
    glLinkProgram(program_id);
    glValidateProgram(program_id);
    glDeleteShader(vs);
    glDeleteShader(fs);
}

ShaderProgram::ShaderProgram() {
    program_id = glCreateProgram();
}

ShaderProgram::ShaderProgram(const std::string &vertexShader, const std::string &fragmentShader) {
    program_id = glCreateProgram();
    setShaders(vertexShader, fragmentShader);
}


ShaderProgram::~ShaderProgram() {
    glDeleteProgram(program_id);
}

void ShaderProgram::bind() const {
    glUseProgram(program_id);
}

void ShaderProgram::unbind() const {
    glUseProgram(0);
}

int ShaderProgram::uniformLocation(std::string name) const {
    auto uniform_it = uniforms.find(name);
    if (uniform_it != uniforms.end())
        return uniform_it->second;
    int location = glGetUniformLocation(program_id, name.c_str());;
    uniforms[name] = location;
    return location;
}

void ShaderProgram::setUniform(std::string name, int i) const {
    glUniform1i(uniformLocation(name), i);
}

void ShaderProgram::setUniform(std::string name, float f) const {
    glUniform1f(uniformLocation(name), f);
}

void ShaderProgram::setUniform(std::string name, const glm::vec2 &v) const {
    glUniform2f(uniformLocation(name), v.x, v.y);
}

void ShaderProgram::setUniform(std::string name, const vec3 &v) const {
    glUniform3f(uniformLocation(name), v.x, v.y, v.z);
}

void ShaderProgram::setUniform(std::string name, const vec4 &v) const {
    glUniform4f(uniformLocation(name), v.x, v.y, v.z, v.w);
}

void ShaderProgram::setUniform(std::string name, const glm::mat3 &m) const {
    glUniformMatrix3fv(uniformLocation(name), 1, false, value_ptr(m));
}

void ShaderProgram::setUniform(std::string name, const mat4 &m) const {
    glUniformMatrix4fv(uniformLocation(name), 1, false, value_ptr(m));
}

void ShaderProgram::setUniform(unsigned int location, int i) const {
    glUniform1i(location, i);
}

void ShaderProgram::setUniform(unsigned int location, float f) const {
    glUniform1f(location, f);
}

void ShaderProgram::setUniform(unsigned int location, const glm::vec2 &v) const {
    glUniform2f(location, v.x, v.y);
}

void ShaderProgram::setUniform(unsigned int location, const vec3 &v) const {
    glUniform3f(location, v.x, v.y, v.z);
}

void ShaderProgram::setUniform(unsigned int location, const vec4 &v) const {
    glUniform4f(location, v.x, v.y, v.z, v.w);
}

void ShaderProgram::setUniform(unsigned int location, const glm::mat3 &m) const {
    glUniformMatrix3fv(location, 1, false, value_ptr(m));
}

void ShaderProgram::setUniform(unsigned int location, const mat4 &m) const {
    glUniformMatrix4fv(location, 1, false, value_ptr(m));
}

void ShaderProgram::setUniformIntArray(unsigned int location, int *i, unsigned int count) const {
    glUniform1iv(location, count, i);
}

void ShaderProgram::setUniformFloatArray(unsigned int location, float *f, unsigned int count) const {
    glUniform1fv(location, count, f);
}

void ShaderProgram::setUniformVec2Array(unsigned int location, const glm::vec2 *v, unsigned int count) const {
    const float *flat_array = &v[0].x;
    glUniform2fv(location, count, flat_array);
}

void ShaderProgram::setUniformVec3Array(unsigned int location, const glm::vec3 *v, unsigned int count) const {
    const float *flat_array = &v[0].x;
    glUniform3fv(location, count, flat_array);
}

void ShaderProgram::setUniformVec4Array(unsigned int location, const glm::vec4 *v, unsigned int count) const {
    const float *flat_array = &v[0].x;

    glUniform4fv(location, count, flat_array);
}


unsigned int ShaderProgram::compileShader(unsigned int type, const std::string &source) {
    unsigned int id = glCreateShader(type);
    const char *src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char *message = (char *) malloc(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
       std::cerr<<message<<std::endl;
        free(message);
    }
    return id;
}

std::string ShaderProgram::getSource(const std::string &path) {
    try {
        std::ifstream file;
        file.open(path);
        if (file.is_open()) {
            std::stringstream strStream;
            strStream << file.rdbuf();
            std::string str = strStream.str();
            file.close();
            return str;
        } else {
            std::cerr << "Unable to find file:" + path << std::endl;
        }

    }
    catch (std::exception &e) {
        std::cerr << "failed to read file " + path + "\n" + e.what() << std::endl;
    }
    return "";
}








