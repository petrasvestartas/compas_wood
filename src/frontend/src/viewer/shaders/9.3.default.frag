#version 330 core
//resources:
//https://stackoverflow.com/questions/6017176/gllinestipple-deprecated-in-opengl-3-1
out vec4 FragColor;

//in vec2 TexCoords;

//uniform sampler2D texture_diffuse1;
uniform vec4 uniform_fragment_color;

void main()
{
    //FragColor = texture(texture_diffuse1, TexCoords);
       FragColor = uniform_fragment_color;
}
