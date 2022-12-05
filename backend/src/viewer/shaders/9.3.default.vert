#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 projection_view_model;

void main()
{
    gl_Position = projection_view_model * vec4(aPos, 1.0f); 
}