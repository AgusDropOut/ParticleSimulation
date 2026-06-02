#pragma once

#include <string>
#include <glad/glad.h>

class ShaderProgram {
    private:
        GLuint programID{0};
        std::string readFile(const std::string& filePath );
        GLuint compileShader(GLenum type, const std::string& source);
    public:
    ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath);

    virtual ~ShaderProgram();

    void use() const;

    GLuint getID() const {return programID;};

};