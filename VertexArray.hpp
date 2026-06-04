#pragma once
#include <glad/glad.h>
#include <utility>
#include <span>
#include "VertexBuffer.hpp"

class VertexArray {
    private:
        GLuint id;

    public:
        VertexArray(){
            glGenVertexArrays(1, &id);
        }

        ~VertexArray(){
            if(id !=0){
                glDeleteVertexArrays(1, &id);
            }
        }

        void bind(){
            glBindVertexArray(id);
        }

        void unbind(){
            glBindVertexArray(0);
        }

        void addAtributte(VertexBuffer &vertexBuffer, int index, int size, GLenum type, GLboolean normalized, int divisor){
            bind();
            glEnableVertexAttribArray(index);
            vertexBuffer.bind();

            //for now 0 because we let the controller calculate the stride cause our data array are homogeneos
            //for now nullptr because we have separated buffers for each attribute
            glVertexAttribPointer(index, size, type, normalized, 0 ,nullptr);


            // Tells the gpu the size of the steps as it reads the vbos, 
            // we want to read an element at a time for the changing attibs and not move for the base geometry
            glVertexAttribDivisor(index, divisor); 

            

        }

};