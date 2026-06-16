#pragma once

#include <string>
#include <cmath>
#include <vector>
#include <array>
#include <iterator>
#include <glad/glad.h>
#include "MouseInteractionHandler.cpp"
#include "Vector3DMath.cpp"
#include "GridLayout.cpp"
#include <cstdint>
#include "Particle.hpp"






struct FastRNG {
    uint32_t state; 
    static constexpr float NormalizeMultiplier = 1.0f / static_cast<float>(std::numeric_limits<uint32_t>::max());

    uint32_t next() {
        uint32_t x = state;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        state = x;
        return x;
    }

    float nextFloat(){
        return next() * NormalizeMultiplier;
    }

    int nextColor(){
        return nextFloat() * 255;
    }
};

class ParticleSystem{

    public: 

        static constexpr const int maxParticles = 10000;
        static constexpr const float maxParticleSize = 0.01f;




        ParticleSystem( MouseInteractionHandler & interactionHandler, GridLayout & grid) : interactionHandler(interactionHandler), grid(grid){
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
            grid.mapParticlesToSectors(particles);
            for(int i = 0 ; i < particles.size() ; i++){
                particles[i].position[0] += particles[i].velocity[0] * (deltaTime);
                particles[i].position[1] += particles[i].velocity[1] * (deltaTime);
                particles[i].position[2] += particles[i].velocity[2] * (deltaTime);

            
                checkWallCollisions(particles[i]);
                checkMouseInteraction(particles[i]);
                applyFriction(particles[i]);
                

            }



            int sectorCount = grid.maxColumns * grid.maxRows;
            for(int sector = 0 ; sector < sectorCount ; sector++){
                
                int particleIndex = grid.getFirstParticleFromSector(sector);
                std::array<int, 4> neighbors =  grid.getNeighborSectors(sector);
                while(particleIndex != -1){

                    int otherParticleIndex = grid.getNextParticleIndex(particleIndex); 
                    while(otherParticleIndex != -1){
                        
                        checkCollision(particles[particleIndex],particles[otherParticleIndex]);
                        otherParticleIndex = grid.getNextParticleIndex(otherParticleIndex); 
                    }
                    


                    for(int neighbor : neighbors){
                        if(neighbor != -1){
                            int particleNearbyIndex = grid.getFirstParticleFromSector(neighbor);
                            while(particleNearbyIndex != -1){
                                checkCollision(particles[particleIndex], particles[particleNearbyIndex]);
                                particleNearbyIndex = grid.getNextParticleIndex(particleNearbyIndex);
                            }
                        }
                    }
                    particleIndex = grid.getNextParticleIndex(particleIndex);
                }
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
                    p.velocity[0] += forceDir[0] * 0.2;
                    p.velocity[1] += forceDir[1] * 0.2;
                    p.velocity[2] += forceDir[2] * 0.2;
                }

                if(isRightClicking){
                    
                    p.velocity[0] += forceDir[0] * -1 * 0.2;
                    p.velocity[1] += forceDir[1] * -1 * 0.2;
                    p.velocity[2] += forceDir[2] * -1 * 0.2;
                }



            }
        }

        

        void checkWallCollisions(Particle &p){
            if( p.position[0] + p.size > 1.0 || p.position[0] -  p.size < -1.0){
                p.velocity[0] *= -1;
                //p.position[0] += p.velocity[0];
            }

            if( p.position[1] > 0.9 + p.size || p.position[1] - p.size < -1.0){
                p.velocity[1] *= -1;
                //p.position[1] += p.velocity[1];
            }
        }

        std::array<Particle, maxParticles> getParticles() {

            return particles;
        }


    private: 
        std::array<Particle, maxParticles> particles;
        MouseInteractionHandler & interactionHandler;
        GridLayout & grid;

        float lastFrameTime;

        void initialize(){

            FastRNG random;
            random.state = 10;

            for(int i = 0 ; i < maxParticles ; i++){

                particles[i].size = random.nextFloat() *  (maxParticleSize - 0.01f) + 0.01f;

                particles[i].position[0] = random.nextFloat();
                particles[i].position[1] = random.nextFloat();
                particles[i].position[2] = 0;

                particles[i].color[0] = random.nextColor();
                particles[i].color[1] = random.nextColor();
                particles[i].color[2] = random.nextColor();
                particles[i].color[3] = 255;

                particles[i].velocity[0] = random.nextFloat() *  (1 + 1) - 1;
                particles[i].velocity[1] = random.nextFloat() *  (1 + 1) - 1;
                particles[i].velocity[2] = 0;
            }
    
        }


        void checkCollision(Particle & p1, Particle & p2){

            // Here we do a little optimization
            // pows and sqrts are really slow. If I can avoid using them, I will do
            
            float a = p1.position[0]-p2.position[0];
            float b = p1.position[1]-p2.position[1];
            // pow(x,2) is just mutiplying the number two times
            float c_squared = a * a + b * b;
            float overlap = p1.size + p2.size;
            // instead of doing an sqrt to restore the units, we "pow()" the righ side to fulfill the inequation
            // Here we are reasembling the original code c = sqrt(pow(a,2)+pow(b,2))
            // The original version was really slow, now I gained +40fps
            if( c_squared < overlap * overlap){
                float c = std::sqrt(c_squared);

                std::array<float, 3> colDir = Vector3DMath::normalize(Vector3DMath::substract(p1.position , p2.position));

                float normal = Vector3DMath::dot( Vector3DMath::substract(p1.velocity,p2.velocity), colDir);

                std::array<float, 3> finalForce = Vector3DMath::scalar(colDir,normal);

                

                

                if(normal > 0){
                    return;
                }

              


                float overlap   =   (p1.size + p2.size) - c;

                p1.position[0] += colDir[0] * (overlap * 0.5);
                p1.position[1] += colDir[1] * (overlap * 0.5);
                p2.position[0] -= colDir[0] * (overlap * 0.5);
                p2.position[1] -= colDir[1] * (overlap * 0.5);

            
                p1.velocity[0] -= finalForce[0];
                p1.velocity[1] -= finalForce[1];
                p1.velocity[2] -= finalForce[2];

                p2.velocity[0] -= finalForce[0] * -1;
                p2.velocity[1] -= finalForce[1] * -1;
                p2.velocity[2] -= finalForce[2] * -1;

                

                


            }

        }

        void applyFriction(Particle & p){
            p.velocity[0] *= 0.998;
            p.velocity[1] *= 0.998;
            p.velocity[2] *= 0.998;
        }

        void applyGravity(Particle & p){
            p.position[1] -= 0.001; 
        }


};