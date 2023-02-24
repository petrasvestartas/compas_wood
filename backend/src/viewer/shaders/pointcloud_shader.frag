#version 330 core
out vec4 FragColor;

//in vec2 TexCoords;

//uniform sampler2D texture_diffuse1;
uniform float time;

in vec3 ourColor;


void main()
{
    //FragColor = texture(texture_diffuse1, TexCoords);
    FragColor = vec4(ourColor.x,ourColor.y,ourColor.z, 1.0f);
      //FragColor = vec4(ourColor.x+time,ourColor.y,ourColor.z+time, 1.0f);
    //FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
