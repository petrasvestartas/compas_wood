#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;


out vec3 FragPos;
out vec3 Normal;
out vec3 LightPos;
out vec3 Color;

uniform vec3 lightPos; 

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    Color = aColor;
    
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(view * model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(view * model))) * aNormal;
    LightPos = vec3(vec4(lightPos, 1.0)); // Transform world-space light position to view-space light position
   // FragPos = vec3(model * vec4(aPos, 1.0));
   //Normal = mat3(transpose(inverse(model))) * aNormal;  
   // gl_Position = projection * view * vec4(FragPos, 1.0);
}