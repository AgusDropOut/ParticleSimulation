#pragma once

#include <string>
#include <cmath>
#include <vector>
#include <iterator>
#include <glad/glad.h>

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

        


        ParticleSystem(int maxParticles){
            this->maxParticles = maxParticles;
            particles.resize(maxParticles);
            initialize();
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

                
            }

            lastFrameTime = currentFrameTime;
        }

        std::vector<Particle> getParticles() {



            return particles;
        }


    private: 
        std::vector<Particle> particles;

        float lastFrameTime;

        void initialize(){

            particles[0].size = 0.5f;

            particles[1].size = 0.5f;

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