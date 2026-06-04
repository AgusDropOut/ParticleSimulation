#include <glad/glad.h>
#include <vector>
#define PI 3.14159f
#include <cmath>

namespace GeometryGenerator{

    std::vector<GLfloat> circleGeometry(int segments,float radius){
        std::vector<GLfloat> vertices(segments * 3);


        float tetha = 0;
    
 
        int i = 3;
        int segment = 1;

        //GL_TRIANGLE_FAN requieres the initial vertex to be the center
        vertices[0] = 0;
        vertices[1] = 0;
        vertices[2] = 0;
        while(segment < segments - 1 ){

            tetha         =  segment * 2.0f * PI / segments;
            vertices[i]   =  std::sin(tetha) * radius;
            vertices[i+1] =  std::cos(tetha) * radius;
            vertices[i+2] =  0;

            i = i + 3;
            segment++;
        }

        // Also the last vertex needs to connect with the first one
        vertices[i]   = vertices[3]; 
        vertices[i+1] = vertices[4]; 
        vertices[i+2] = 0.0f;
        
        return vertices;

    }
};