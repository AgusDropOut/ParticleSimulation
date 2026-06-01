#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

std::string vertexCode;
std::string fragmentCode;

std::ifstream vShaderFile;
std::ifstream fShaderFile;





#define MaxParticles 2


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}


void processInput(GLFWwindow* window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {

    //Quad geometry
    static const GLfloat vertices[] = {
       -0.5f,-0.5f, 0.0f,
        0.5f,-0.5f, 0.0f,
       -0.5f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
    };

    GLfloat cpu_positions[8] = {
    -0.5f,  0.5f, 0.0f, 0.5f, 
     0.5f, -0.5f, 0.0f, 0.5f  
    };

    GLubyte colors[8] = {
    255, 0, 0, 255,   
    0, 255, 0, 255  
    };

    



    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  
    GLFWwindow* window = glfwCreateWindow(800, 600, "Particle Simulation", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

   
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);


    //VBO for the billboard vertices
    GLuint billboard_vertex_buffer;
    glGenBuffers(1, &billboard_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //STATIC DRAW because the vertices of the billboard do not change

    //VBO for the positions
    GLuint particles_position_buffer;
    glGenBuffers(1, &particles_position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); //STREAM DRAW because positions change really often

    //VBO for the colors
    GLuint particles_color_buffer;
    glGenBuffers(1, &particles_color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // STREAM DRAW , idk maybe we will change the color frquently


    //1st channel for streaming
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glVertexAttribPointer(0, 3 , GL_FLOAT,GL_FALSE,0, nullptr);

    // 2nd channel for streaming , size 4 due to attrib x y z size
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr );

    // 3rd channel for streaming, size 4 due to r g b a, GL_TRUE because chars gor form 0 to 255 and gpus expect a number between 0 and 1
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
    glVertexAttribPointer(2, 4,GL_UNSIGNED_BYTE, GL_TRUE, 0, nullptr);


    // Tells the gpu the size of the steps as it reads the vbos, 
    // we want to read an element at a time for the changing attibs and not move for the base geometry
    glVertexAttribDivisor(0, 0); 
    glVertexAttribDivisor(1, 1); 
    glVertexAttribDivisor(2, 1);
    
    
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
       
        vShaderFile.open("../particle.vsh");
        fShaderFile.open("../particle.fsh");
        
        std::stringstream vShaderStream, fShaderStream;
        
       
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        
        
        vShaderFile.close();
        fShaderFile.close();
        
        
        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
    }


    //Shaders

    auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    const GLchar* vShaderCode = vertexCode.c_str();
    const GLchar* fShaderCode = fragmentCode.c_str();

    glShaderSource(vertex_shader, 1, &vShaderCode, nullptr);
    glShaderSource(fragment_shader, 1, &fShaderCode, nullptr);


    glCompileShader(vertex_shader);
    glCompileShader(fragment_shader);

    int program_creation_result = glCreateProgram();

    if(program_creation_result == 0){
        std::cout << "couldnt create the shader program" << std::endl;
    }

    glAttachShader(program_creation_result,vertex_shader);
    glAttachShader(program_creation_result,fragment_shader);

    glLinkProgram(program_creation_result);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);



  
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

    
        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program_creation_result);

        glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, 2 * 4 * sizeof(GLfloat), cpu_positions);
        glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, 2 * 4 * sizeof(GLubyte), colors);
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, MaxParticles);

        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

   
    glfwTerminate();
    return 0;
}