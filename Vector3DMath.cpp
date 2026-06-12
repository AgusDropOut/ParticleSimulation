#pragma once

#include <vector>
#include <array>
#include <cmath>

namespace Vector3DMath
{
    std::array<float,3> substract(const std::array<float, 3> op1, const std::array<float, 3> op2){
        std::array<float,3> result;

        result[0] =  op1[0] - op2[0];
        result[1] =  op1[1] - op2[1];
        result[2] =  op1[2] - op2[2];

        return result;
    }

    std::array<float,3> scalar(const std::array<float, 3> op, const float scalar){
        std::array<float,3> result;

        result[0] =  op[0] * scalar;
        result[1] =  op[1] * scalar;
        result[2] =  op[2] * scalar;

        return result;
    }

   

    float getLength(const std::array<float, 3> op){
        return std::sqrt(std::pow(op[0],2.0f) + std::pow(op[1],2.0f) + std::pow(op[2],2.0f));
    }

    std::array<float,3> normalize(const std::array<float, 3> op){
        std::array<float,3> result;

        float lenght =  getLength(op);

        result[0] =  op[0] / lenght;
        result[1] =  op[1] / lenght;
        result[2] =  op[2] / lenght;

        return result;
    }

    

    float multiply(const std::array<float, 3> p1, const std::array<float, 3> p2){
        return p1[0] * p2[0] + p1[1] * p2[1] + p1[2] * p2[2];
    }

    float angleCosine(const std::array<float, 3> op1,const std::array<float, 3> op2){
        return multiply(op1,op2) / (getLength(op1) * getLength(op2));
    }

    
    float dot(const std::array<float, 3> p1, const std::array<float, 3> p2){
        return multiply(p1,p2);
    }

    
    
} 
