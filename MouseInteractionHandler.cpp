#pragma once

#include "Window.hpp"

class MouseInteractionHandler {

    public:
        MouseInteractionHandler(Window &window) : window(window){}

        void getMouseCoords(double mouseX, double mouseY){
            window.getMousePos(mouseX,mouseY);
        }

        bool isLeftClicking(){
            return window.isLeftClickPressed();
        }

        bool isRightClicking(){
            return window.isRightClickPressed();
        }



    private:
        Window& window;
};