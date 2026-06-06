#pragma once

#include <string>
#include <cmath>
#include <vector>
#include <array>
#include <iterator>
#include <glad/glad.h>
#include "MouseInteractionHandler.cpp"
#include "Vector3DMath.cpp"

struct Particle
{
    float position[3];
    float velocity[3];
    unsigned char color[4];
    float size;
    float life;
};

class ParticleSystem{

    public: 

        int maxParticles;

        


        ParticleSystem(int maxParticles, MouseInteractionHandler & interactionHandler) : interactionHandler(interactionHandler){
            this->maxParticles = maxParticles;
            particles.resize(maxParticles);
            initialize();
            this->lastFrameTime = 0.0f;
        }

        std::vector<GLfloat> positions(){
            std::vector<GLfloat> positions;

            
            

            for(Particle p : particles){
                positions.push_back(p.position[0]);
                positions.push_back(p.position[1]);
                positions.push_back(p.position[2]);
                positions.push_back(p.size);
            }

            return positions;
        }

        std::vector<GLchar> colors(){
            std::vector<GLchar> positions;

            for(Particle p : particles){
                positions.push_back(p.color[0]);
                positions.push_back(p.color[1]);
                positions.push_back(p.color[2]);
                positions.push_back(p.color[2]);
            }

            return positions;
        }

        void update(float currentFrameTime){

            
            float deltaTime = currentFrameTime - lastFrameTime;
            for(int i = 0 ; i < particles.size() ; i++){
                particles[i].position[0] += particles[i].velocity[0] * (deltaTime);
                particles[i].position[1] += particles[i].velocity[1] * (deltaTime);
                particles[i].position[2] += particles[i].velocity[2] * (deltaTime);

                checkWallCollisions(particles[i]);
                checkMouseInteraction(particles[i]);
                
            }

            lastFrameTime = currentFrameTime;
        }

        void checkMouseInteraction(Particle & p){
            bool isLeftClicking = interactionHandler.isLeftClicking();
            bool isRightClicking = interactionHandler.isRightClicking();

            if( isLeftClicking || isRightClicking ){
                double mouseX;
                double mouseY;
                interactionHandler.getMouseCoords(mouseX,mouseY);
                
                std::array<float, 3> forceDir = Vector3DMath::normalize(Vector3DMath::substract({mouseX,mouseY,0.0f}, p.position));

                if(isLeftClicking){
                    p.velocity[0] += forceDir[0];
                    p.velocity[1] += forceDir[1];
                    p.velocity[2] += forceDir[2];
                }

                if(isRightClicking){
                    
                    p.velocity[0] += forceDir[0] * -1;
                    p.velocity[1] += forceDir[1] * -1;
                    p.velocity[2] += forceDir[2] * -1;
                }



            }
        }

        

        void checkWallCollisions(Particle &p){
            if( p.position[0] + p.size > 1.0 || p.position[0] -  p.size < -1.0){
                p.velocity[0] *= -1;
            }

            if( p.position[1] > 1.0 + p.size || p.position[1] - p.size < -1.0){
                p.velocity[1] *= -1;
            }
        }

        std::vector<Particle> getParticles() {

            return particles;
        }


    private: 
        std::vector<Particle> particles;
        MouseInteractionHandler & interactionHandler;

        float lastFrameTime;

        void initialize(){

            particles[0].size = 0.15f;

            particles[1].size = 0.15f;

            particles[0].position[0] = -0.5f;
            particles[0].position[1] = 0.5f;
            particles[0].position[2] = 0.0f;

            particles[1].position[0] = 0.5f;
            particles[1].position[1] = -0.5f;
            particles[1].position[2] = 0.0;
            
            

            particles[0].color[0] = 255;
            particles[0].color[1] = 0;
            particles[0].color[2] = 0;
            particles[0].color[3] = 255;

            particles[1].color[0] = 0;
            particles[1].color[1] = 255;
            particles[1].color[2] = 0;
            particles[1].color[3] = 255;


            particles[0].position[0] = -0.5f;
            particles[0].position[1] = 0.5f;
            particles[0].position[2] = 0.0f;

            particles[0].velocity[0] = -0.5f;
            particles[0].velocity[1] = 0.0f;
            particles[0].velocity[2] = 0.0f;

            particles[1].position[0] = 0.9f;
            particles[1].position[1] = -0.9f;
            particles[1].position[2] = 0.0f;

            particles[1].velocity[0] = -0.5f;
            particles[1].velocity[1] = 0.0f;
            particles[1].velocity[2] = 0.0f;
    
        }


};