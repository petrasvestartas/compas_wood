#version 330 core
//resources:
//https://stackoverflow.com/questions/6017176/gllinestipple-deprecated-in-opengl-3-1
out vec4 FragColor;


uniform vec4 uniform_fragment_color;

void main()
{
     FragColor = uniform_fragment_color;
    
}
