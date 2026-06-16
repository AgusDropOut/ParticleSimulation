#pragma once

#include <array>
#include <cmath>
#include <vector>
#include <algorithm>
#include "Window.hpp"
#include "Particle.hpp"



class GridLayout {

    private:
        std::vector<int> next;
        std::vector<int> head;
        
        int nextIndex;
        
    public:

        int maxRows;
        int maxColumns;

        GridLayout(int maxRows, int maxColumns, int maxParticles) 
        : next(maxParticles), 
          head(maxRows * maxColumns), 
          maxRows(maxRows),           
          maxColumns(maxColumns),    
          nextIndex(0) 
        {
        
        }

        int getSectorFromParticle(Particle p){

            int column = std::min(static_cast<int>(std::floor((std::max(p.position[0], -1.0f) + 1)/2 * maxColumns)), maxColumns-1);
            int row = std::min(static_cast<int>(std::floor((std::max(p.position[1], -1.0f) + 1)/2 * maxRows)), maxRows-1);
            

            return (row * maxColumns) + column;

        }

        int getNextParticleIndex(int currentParticleIndex) {
            if (currentParticleIndex == -1) return -1;
            return next[currentParticleIndex];
        }

        int getFirstParticleFromSector(int sector) {
            return head[sector];
        }
        template<size_t N>
        void mapParticlesToSectors(const std::array<Particle, N>& particles){
            std::fill(head.begin(), head.end(), -1);
            std::fill(next.begin(), next.end(), -1);
            for(int i = 0 ; i < particles.size() ; i++){
                int sector = getSectorFromParticle(particles.at(i));
                next[i] = head[sector];
                head[sector] = i;
            }
        }

        std::array<int, 4> getNeighborSectors(int sector){
            std::array<int, 4> neighbors = {-1,-1,-1,-1};
            int column = sector % maxColumns; 
            int row = sector / maxColumns;
            

            // (row we want * max columns) + column we want 
            // myyy brainnnnnn
            if(column + 1 < maxColumns){
                neighbors[0] = (row * maxColumns) + column + 1;
            } 
            if(column - 1 >= 0 && row + 1 < maxRows ){
                neighbors[1] = ((row + 1) * maxColumns) + column - 1;
            } 
            if( row + 1 < maxRows ){
                neighbors[2] = ((row + 1) * maxColumns) + column;
            }
            if(column + 1 < maxColumns && row + 1 < maxRows ){
                neighbors[3] = ((row + 1) * maxColumns) + column + 1;
            } 

            return neighbors;

        }









        


};