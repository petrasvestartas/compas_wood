#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 LightPos;
in vec3 Color;

// extra in variable, since we need the light position in view space we calculate this in the vertex shader

uniform vec3 lightColor;
uniform vec3 objectColor;
uniform float f;
uniform float transparency;

void main()
{

    

   //flat shading
//   vec3 x_ = dFdx(FragPos);
//   vec3 y_= dFdy(FragPos);
//   vec3 normal_ = cross(x_, y_);
//   vec3 norm_ = normalize(normal_);


    // ambient
    float ambientStrength = 0.75;
    vec3 ambient = ambientStrength * lightColor;    
    
     // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);//change "norm_" to "norm" avoid the performance warning and have unwelded view
    vec3 diffuse = diff * lightColor;
    
    // specular
    float specularStrength = 0.01;
    vec3 viewDir = normalize(-FragPos); // the viewer is always at (0,0,0) in view-space, so viewDir is (0,0,0) - Position => -Position
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor; 
    
	// vec3 shading = (ambient + diffuse + specular)*objectColor;
	// //float f = 0.75;
	// float r_interpolated =  shading[0] + f * (objectColor[0] - shading[0]);
	// float g_interpolated =  shading[1] + f * (objectColor[1] - shading[1]);
	// float b_interpolated =  shading[2] + f * (objectColor[2] - shading[2]);

    vec3 shading = (ambient + diffuse + specular)*Color;
	float r_interpolated =  shading[0] + f * (Color[0] - shading[0]);
	float g_interpolated =  shading[1] + f * (Color[1] - shading[1]);
	float b_interpolated =  shading[2] + f * (Color[2] - shading[2]);

	 
    vec3 result = vec3(r_interpolated,g_interpolated,b_interpolated);

    if(objectColor[0] == 0 && objectColor[1] == 0 && objectColor[2] == 0  )
        FragColor = vec4(0,0,0,1);
    else
        FragColor = vec4(result, transparency);

  
    //FragColor = vec4(Color, transparency);
    
    
}