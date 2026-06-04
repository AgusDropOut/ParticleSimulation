#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <vector>
#include <iterator>
#include "ShaderProgram.hpp"
#include "Window.hpp"
#include "VertexBuffer.hpp"
#include "VertexArray.hpp"
#include "GeometryGenerator.cpp"




#define MaxParticles 2





void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}




struct Particle
{
    float position[3];
    float velocity[3];
    unsigned char color[4];
    float size;
    float life;
};

std::vector<Particle> particles(MaxParticles);

void updatePositions(float (&positions)[4 * MaxParticles]){
    int j = 0;
    for(int i = 0 ; i < std::size(positions) ; i+= 4){
        positions[i] += particles[j].velocity[0];
        positions[i+1] += particles[j].velocity[1];
        positions[i+2] += particles[j].velocity[2];
        j++;
    }
}


int main() {

    int segments = 128;
    std::vector<GLfloat> vertices = GeometryGenerator::circleGeometry(segments,0.5f);
            




    GLfloat cpu_positions[4 * MaxParticles] = {
    -0.5f,  0.5f, 0.0f, 0.5f, 
     0.5f, -0.5f, 0.0f, 0.5f  
    };

    GLubyte colors[8] = {
    255, 0, 0, 255,   
    0, 255, 0, 255  
    };

    particles[0].position[0] = -0.5f;
    particles[0].position[1] = 0.5f;
    particles[0].position[2] = 0.0f;

    particles[0].velocity[0] = -0.0005f;
    particles[0].velocity[1] = 0.0f;
    particles[0].velocity[2] = 0.0f;

    particles[1].position[0] = 0.5f;
    particles[1].position[1] = -0.5f;
    particles[1].position[2] = 0.0f;

    particles[1].velocity[0] = -0.0005f;
    particles[1].velocity[1] = 0.0f;
    particles[1].velocity[2] = 0.0f;
    


    Window window(800, 600, "Particle Simulation");



    
    VertexArray vao{};

    //Dumbass c++ always wants to declare a function, I miss you Java :c
    VertexBuffer billboard_vbo{};
    VertexBuffer positions_vbo{};
    VertexBuffer color_vbo{};

    //STATIC DRAW because the vertices of the billboard do not change
    billboard_vbo.setData(vertices,GL_STATIC_DRAW);
    //STREAM DRAW because positions change really often
    positions_vbo.setData(MaxParticles * 4 * sizeof(GLfloat),GL_STREAM_DRAW);
    // STREAM DRAW , idk maybe we will change the color frquently
    color_vbo.setData(MaxParticles * 4 * sizeof(GLubyte),GL_STREAM_DRAW);


     // The last 0 on the call Tells the gpu the size of the steps as it reads the vbos, 
    // we want to read an element at a time for the changing attibs and not move for the base geometry

    //1st channel for streaming
    vao.addAtributte(billboard_vbo,0,3,GL_FLOAT,GL_FALSE,0);
    // 2nd channel for streaming , size 4 due to attrib x y z size
    vao.addAtributte(positions_vbo,1,4,GL_FLOAT,GL_FALSE,1);
     // 3rd channel for streaming, size 4 due to r g b a, GL_TRUE because chars gor form 0 to 255 and gpus expect a number between 0 and 1
    vao.addAtributte(color_vbo,2,4,GL_UNSIGNED_BYTE,GL_TRUE,1);
    
    
    

    //Shaders

    ShaderProgram shader("../particle.vsh", "../particle.fsh");



  
    while (!window.shouldClose()) {
        window.processInput();


        
    
        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();

        updatePositions(cpu_positions);

        

        
        positions_vbo.updateSubData( 0, cpu_positions);
        color_vbo.updateSubData(0,colors);
        glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, segments, MaxParticles);

        
        window.swapBuffers();
        window.pollEvents();
    }

   
    
    return 0;
}

