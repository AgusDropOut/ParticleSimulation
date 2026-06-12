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
#include "ParticleSystem.cpp"
#include "MouseInteractionHandler.cpp"




#define MaxParticles 2





void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}



std::vector<Particle> particles(MaxParticles);




int main() {

    int segments = 128;
    std::vector<GLfloat> vertices = GeometryGenerator::circleGeometry(segments,0.15f);
    
    


    Window window(800, 600, "Particle Simulation");
    MouseInteractionHandler interactionHandler(window);



    
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


    ParticleSystem particleSystem(MaxParticles, interactionHandler);


  
    while (!window.shouldClose()) {
        window.processInput();


        
    
        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();

        particleSystem.update(glfwGetTime());

        particles = particleSystem.getParticles();

        

        
        positions_vbo.updateSubData( 0, particleSystem.positions());
        color_vbo.updateSubData(0, particleSystem.colors());
        glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, segments, MaxParticles);

        
        window.swapBuffers();
        window.pollEvents();
    }

   
    
    return 0;
}

