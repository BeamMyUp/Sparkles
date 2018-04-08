#version 330 core

struct Material
{
	vec3 ambientCoefs;
	vec3 diffuseCoefs;
	vec3 specularCoefs;
	float shininess;  // acts as brightness for a light
};

uniform Material objectMaterial; 
uniform Material lightMaterial; 
  
in vec3 Normal; 
in vec3 FragPosition; 
in vec3 LightPosition; 

out vec4 color;

void main()
{
	// Compute Ambient lighting
	vec3 ambient = lightMaterial.ambientCoefs * objectMaterial.ambientCoefs;

	// Compute directional lighting information
	vec3 norm = normalize(Normal); 
	vec3 lightDir = normalize(LightPosition - FragPosition);
	
	// Compute Diffuse lighting
	float diffCoef = max(dot(norm, lightDir), 0.0); 
	vec3 diffuse = lightMaterial.diffuseCoefs * (diffCoef * objectMaterial.diffuseCoefs); 

	// Compute Specular lighting
	vec3 viewDir = normalize(-FragPosition); // Since viewPosition = (0,0,0)
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow( max( dot(viewDir, reflectDir), 0.0), objectMaterial.shininess); 
	vec3 specular = lightMaterial.specularCoefs * (spec * objectMaterial.specularCoefs);

	// Compute final color
	vec3 result = ambient + diffuse + specular; 
    color = vec4(result, 1.0f); 
}