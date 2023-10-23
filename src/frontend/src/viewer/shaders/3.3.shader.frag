//https://github.com/JoeyDeVries/LearnOpenGL/blob/master/src/3.model_loading/1.model_loading/1.model_loading.fs
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{    
    FragColor = texture(texture_diffuse1, TexCoords);
}