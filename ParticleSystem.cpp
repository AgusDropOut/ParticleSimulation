#pragma once

#include <string>
#include <cmath>
#include <vector>
#include <array>
#include <iterator>
#include <future>
#include <glad/glad.h>
#include "MouseInteractionHandler.cpp"
#include "Vector3DMath.cpp"
#include "GridLayout.cpp"
#include <cstdint>
#include "Particle.hpp"
#include "ThreadPool.hpp"






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

        static constexpr const int maxParticles = 10;
        static constexpr const float maxParticleSize = 0.08f;
        static constexpr const float minParticleSize = 0.04f;
        static constexpr const int threadCount = 1;




        ParticleSystem( MouseInteractionHandler & interactionHandler, GridLayout & grid, ThreadPool & pool) : interactionHandler(interactionHandler), grid(grid), pool(pool){
            particles.resize(maxParticles);
            initialize();
            this->lastFrameTime = 0.0f;
        }

        std::vector<GLfloat> positions(){
            std::vector<GLfloat> positions;
            positions.reserve(maxParticles * 4);
            
            

            for(const auto& p : particles){
                positions.push_back(p.position[0]);
                positions.push_back(p.position[1]);
                positions.push_back(p.position[2]);
                positions.push_back(p.size);
            }

            return positions;
        }

        std::vector<GLchar> colors(){
            std::vector<GLchar> colors;
            colors.reserve(maxParticles * 4);
            for(const auto& p : particles){
                colors.push_back(p.color[0]);
                colors.push_back(p.color[1]);
                colors.push_back(p.color[2]);
                colors.push_back(p.color[3]);
            }

            return colors;
        }

        void update(float currentFrameTime){

            grid.mapParticlesToSectors(particles);
            float deltaTime = currentFrameTime - lastFrameTime;
            std::array<std::future<void>, threadCount> tasks;
            
            
            int particlesPerTask = std::ceil(maxParticles/threadCount);

            std::atomic<int> activeTasks = threadCount;
            std::mutex syncMutex;
            std::condition_variable syncCv;
            
            for(int taskIndex = 0 ; taskIndex < threadCount ; taskIndex++){
                int startParticleIndex = particlesPerTask*taskIndex;
                int endParticleIndex   = (taskIndex == threadCount - 1) ? maxParticles : (startParticleIndex + particlesPerTask);
                pool.queuTask([&,startParticleIndex, endParticleIndex, deltaTime](){
                    for(int i = startParticleIndex ; i < endParticleIndex ; i++){
                       
                        particles[i].position[0] += particles[i].velocity[0] * (deltaTime);
                        particles[i].position[1] += particles[i].velocity[1] * (deltaTime);
                        particles[i].position[2] += particles[i].velocity[2] * (deltaTime);

                    
                        checkWallCollisions(particles[i]);
                        checkMouseInteraction(particles[i]);
                        applyFriction(particles[i]);
                        

                    }

                    //wake up main thread if all workers finished
                    if(--activeTasks == 0){
                        std::lock_guard<std::mutex> lock(syncMutex);
                        syncCv.notify_one();
                    }


                });
            }



            //Main waits here till all active threas finishes
            // I did it this way cause in c++ 17 we dont have std::barrier :c
            std::unique_lock<std::mutex> lock(syncMutex);
            syncCv.wait(lock, [&] (){
                return activeTasks == 0;
            });

            activeTasks = threadCount;

            
            
            int columnsPerTask = grid.maxColumns / threadCount; 
            if (columnsPerTask < 1) columnsPerTask = 1;

            for(int taskIndex = 0 ; taskIndex < threadCount ; taskIndex++){
                int startCol = taskIndex * columnsPerTask + taskIndex;
                int endCol = startCol + columnsPerTask;
                pool.queuTask( [&,startCol, endCol](){

                
                    int sectorCount = grid.maxColumns * grid.maxRows;
                    for(int sector = 0 ; sector < sectorCount ; sector++){
                        int currentCol = sector % grid.maxColumns;
                        if(currentCol < startCol || currentCol > endCol) continue;
                        
                        int particleIndex = grid.getFirstParticleFromSector(sector);
                        std::array<int, 4> neighbors =  grid.getNeighborSectors(sector);
                        while(particleIndex != -1){
                            // beetween the same sector
                            int otherParticleIndex = grid.getNextParticleIndex(particleIndex); 
                            while(otherParticleIndex != -1){
                                
                                checkCollision(particles[particleIndex],particles[otherParticleIndex]);
                                otherParticleIndex = grid.getNextParticleIndex(otherParticleIndex); 
                            }
                            

                            // with neighbor sectors
                            for(int neighbor : neighbors){
                                // Also we need to check if its beetween rank
                                
                                if(neighbor != -1){
                                    int neighborCol = neighbor % grid.maxColumns;
                                    if(neighborCol < startCol || neighborCol > endCol) continue;

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

                    //wake up main thread if all workers finished
                    if(--activeTasks == 0){
                        std::lock_guard<std::mutex> lock(syncMutex);
                        syncCv.notify_one();
                    }
                
                });

                
            }   
            
            //Main waits here till all active threas finishes
            // I did it this way cause in c++ 17 we dont have std::barrier :c
            syncCv.wait(lock, [&] (){
                return activeTasks == 0;
            });

            // Buffer zones time
            int sectorCount = grid.maxColumns * grid.maxRows;
            for(int sector = 0 ; sector < sectorCount ; sector++){
                    int currentCol = sector % grid.maxColumns;
                    bool isWithinReach = ((currentCol + 1) % columnsPerTask == 0) && (currentCol < grid.maxColumns - 1);
                    if(!isWithinReach) continue;
                        
                    int particleIndex = grid.getFirstParticleFromSector(sector);
                    std::array<int, 4> neighbors =  grid.getNeighborSectors(sector);
                    while(particleIndex != -1){
                            // beetween the same sector
                        int otherParticleIndex = grid.getNextParticleIndex(particleIndex); 
                        while(otherParticleIndex != -1){
                                
                            checkCollision(particles[particleIndex],particles[otherParticleIndex]);
                            otherParticleIndex = grid.getNextParticleIndex(otherParticleIndex); 
                        }
                            

                        // with neighbor sectors
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
                
                std::array<float, 3> diff = Vector3DMath::substract({mouseX,mouseY,0.0f}, p.position);
                float distSq = diff[0]*diff[0] + diff[1]*diff[1];

                if (distSq > 0.000001f) {
                    std::array<float, 3> forceDir = Vector3DMath::normalize(diff);

                    if(isLeftClicking){
                        p.velocity[0] += forceDir[0] * 0.2f;
                        p.velocity[1] += forceDir[1] * 0.2f;
                        p.velocity[2] += forceDir[2] * 0.2f;
                    }

                    if(isRightClicking){
                        p.velocity[0] -= forceDir[0] * 0.2f;
                        p.velocity[1] -= forceDir[1] * 0.2f;
                        p.velocity[2] -= forceDir[2] * 0.2f;
                    }
                }



            }
        }

        

        void checkWallCollisions(Particle &p){
           
            if (p.position[0] + p.size > 1.0f) {
                p.position[0] = 1.0f - p.size; 
                p.velocity[0] *= -1.0f;        
            } 
            else if (p.position[0] - p.size < -1.0f) {
                p.position[0] = -1.0f + p.size; 
                p.velocity[0] *= -1.0f;
            }


            if (p.position[1] + p.size > 0.9f) {
                p.position[1] = 0.9f - p.size;
                p.velocity[1] *= -1.0f;
            } 
            else if (p.position[1] - p.size < -1.0f) {
                p.position[1] = -1.0f + p.size;
                p.velocity[1] *= -1.0f;
            }
        }

        std::vector<Particle>& getParticles() {

            return particles;
        }


    private: 
        std::vector<Particle> particles;
        MouseInteractionHandler & interactionHandler;
        GridLayout & grid;
        ThreadPool & pool;

        float lastFrameTime;

        void initialize(){

            FastRNG random;
            random.state = 10;

            for(int i = 0 ; i < maxParticles ; i++){

                particles[i].size = random.nextFloat() *  (maxParticleSize - minParticleSize) + minParticleSize;

                particles[i].position[0] = (random.nextFloat() * 2) - 1;
                particles[i].position[1] = (random.nextFloat() * 2) - 1;
                particles[i].position[2] = 0;

                

                float normX = (particles[i].position[0] + 1.0f) * 0.5f;
                float normY = (particles[i].position[1] + 1.0f) * 0.5f;

       
                particles[i].color[0] = static_cast<GLchar>(normX * 255);          
                particles[i].color[1] = static_cast<GLchar>(normY * 255);                 
                particles[i].color[2] = static_cast<GLchar>((1.0f - normX) * 255); 
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

                if (c_squared < 0.000001f) {
                   
                    p1.position[0] += 0.001f;
                    return; 
                }
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