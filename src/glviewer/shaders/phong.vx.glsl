#version 330 core
  
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix; 
uniform vec3 lightPosition; 

out vec3 Normal; 
out vec3 FragPosition;
out vec3 LightPosition; 

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
	Normal = normalMatrix * normal; 

	FragPosition = vec3(view * model * vec4(position, 1.0f)); 
	LightPosition = vec3(view * vec4(lightPosition, 1.0)); 	
}