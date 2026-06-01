#version 330 core


layout (location = 0) in vec3 squareVertex;

layout (location = 1) in vec4 particlePosition;

out vec4 FragmentColor;

layout (location = 2) in vec4 particleColor;

void main()
{

    float particleSize = particlePosition.w;
    
    vec3 centerPosition = particlePosition.xyz;
    
    vec3 finalPosition = centerPosition + (squareVertex * particleSize);
    
    gl_Position = vec4(finalPosition, 1.0);
    

    FragmentColor = particleColor;
}