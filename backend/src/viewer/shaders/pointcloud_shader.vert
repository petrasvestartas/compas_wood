#version 330 core
layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
layout (location = 1) in vec3 aColor; // the color variable has attribute position 1


out vec3 ourColor;


uniform mat4 projection_view_model;

void main()
{
    //vs_out.texCoords = aTexCoords;
    gl_Position = projection_view_model * vec4(aPos, 1.0); 

     ourColor = aColor;
}