#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

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

            glfwDestroyWindow(window);
        }

        bool isLeftClickPressed(){
            return glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        }

        bool isRightClickPressed(){
            return glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
        }

        void getMousePos(double &normalizedMouseX, double &normalizedMouseY){
            double mouseX, mouseY = 0;

            glfwGetCursorPos(window, &mouseX, &mouseY);


            int viewPort[4];
            glGetIntegerv(GL_VIEWPORT, viewPort);
            double screenX = viewPort[2];
            double screenY = viewPort[3];

            normalizedMouseX = (mouseX / screenX) * 2 - 1;
            normalizedMouseY = (mouseY / screenY) * 2 - 1;

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

        void initializeUI() {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            
            ImGui_ImplGlfw_InitForOpenGL(window, true);
            ImGui_ImplOpenGL3_Init("#version 330 core");
        }
};