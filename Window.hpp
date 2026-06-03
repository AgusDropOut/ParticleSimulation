#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <iostream>

class Window {
    private:

    GLFWwindow* window = nullptr;


    public:

        Window(int width, int height, const char* title){
            glfwInit();
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        
            this -> window = glfwCreateWindow(width, height, title, NULL, NULL);
            if (window == NULL) {
                std::cout << "Failed to create GLFW window" << std::endl;
                glfwTerminate();
            }
            glfwMakeContextCurrent(window);

        
            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
                std::cout << "Failed to initialize GLAD" << std::endl;
            }
        }

        ~Window(){
            if(window){
                glfwDestroyWindow(window);
                glfwTerminate();
            }
        }

        bool shouldClose(){
            return glfwWindowShouldClose(window);
        }

        void processInput(){
            if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
                glfwSetWindowShouldClose(window, true);
            }
        }

        void swapBuffers(){
            glfwSwapBuffers(window);
        }

        void pollEvents(){
            glfwPollEvents();
        }
};