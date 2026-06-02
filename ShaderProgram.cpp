#include "ShaderProgram.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <glad/glad.h>


ShaderProgram::ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath){
    
    GLuint vertex_shader = this->compileShader(GL_VERTEX_SHADER, vertexPath);
    GLuint fragment_shader = this->compileShader(GL_FRAGMENT_SHADER, fragmentPath);
    
    int program = glCreateProgram();

    if(program == 0){
        std::cout << "couldnt create the shader program" << std::endl;
    }

    glAttachShader(program,vertex_shader);
    glAttachShader(program,fragment_shader);

    glLinkProgram(program);

    //Program link debug
    GLint linkSuccess;
    glGetProgramiv(program, GL_LINK_STATUS, &linkSuccess);
    if (!linkSuccess) {
        GLint logLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            std::vector<GLchar> infoLog(logLength);
            glGetProgramInfoLog(program, logLength, nullptr, infoLog.data());
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog.data() << std::endl;
        }
    }


    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    
    this->programID = program;

    
};

GLuint ShaderProgram::compileShader(GLenum type, const std::string& source){

    GLuint shader = glCreateShader(type);
    std::string shaderCodeString = this->readFile(source);
    const GLchar* shaderCode = shaderCodeString.c_str();
    glShaderSource(shader, 1, &shaderCode, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);


    if(success){
        return shader;
        // Print compilation errors
    } else {
        
        GLint logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        
        if (logLength > 0) {
            
            std::vector<GLchar> infoLog(logLength);
            
           
            glGetShaderInfoLog(shader, logLength, nullptr, infoLog.data());
            
          
            std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" 
                      << "Path: " << source << "\n"
                      << "Log: " << infoLog.data() << std::endl;
        } else {
            std::cout << "failed to compile shader from path: " << source << " (No log available)" << std::endl;
        }
    }

   return 0;


};

std::string ShaderProgram::readFile(const std::string& filePath){

    std::ifstream ShaderFile;

    ShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
       
        
        ShaderFile.open(filePath);
        
        std::stringstream ShaderStream;
        
       
        ShaderStream << ShaderFile.rdbuf();
        
        
        ShaderFile.close();
        
        
        return ShaderStream.str();
    }
    catch (std::ifstream::failure& e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
        return "";
    }




    

};

void ShaderProgram::use() const{
        glUseProgram(this -> programID);
};

ShaderProgram::~ShaderProgram() {
    if (programID != 0) {
        glDeleteProgram(programID);
    }
}


    