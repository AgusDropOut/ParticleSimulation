#pragma once
#include <glad/glad.h>
#include <utility>
#include <span>
#include <vector>


class VertexBuffer {
    private:
        GLuint id;
    
    public:

    VertexBuffer(){
        glGenBuffers(1, &id);
    }

    ~VertexBuffer(){
        if(id!=0){
            glDeleteBuffers(1,&id);
        }
    }

    void bind(){
        glBindBuffer(GL_ARRAY_BUFFER, id);
    }

    void unbind(){
        glBindBuffer(GL_ARRAY_BUFFER,0);
    }

    //Case we reserve the space and also add the data
    template<typename T, size_t N>
    void setData(const T (&data)[N],GLenum usage){
        bind();
        glBufferData(GL_ARRAY_BUFFER, N * sizeof(T), data, usage);
    }

    //for vectors
    template<typename T>
    void setData(const std::vector<T> &data,GLenum usage){
        bind();
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), usage);
    }

    //Case we only reserve the space
    void setData(GLsizeiptr sizeInBytes, GLenum usage) {
        bind();
        glBufferData(GL_ARRAY_BUFFER, sizeInBytes, nullptr, usage);
    }


    //Maybe I need the offset for threading in the future
    template<typename T, size_t N>
    void updateSubData(GLintptr offset, const T (&data)[N]){
        bind();
        glBufferSubData(GL_ARRAY_BUFFER, offset, N * sizeof(T), data);
    }

    template<typename T>
    void updateSubData(GLintptr offset, const std::vector<T> &data){
        bind();
        glBufferSubData(GL_ARRAY_BUFFER, offset, data.size() * sizeof(T), data.data());
    }

};