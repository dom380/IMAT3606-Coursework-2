#version 430

struct Material
{
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	float d;
	float Ns;
};

struct Light
{
	vec3 pos;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform sampler2D tex;
layout (std140) uniform LightingBlock
{
	Light lights[10];
};

uniform Material material;
uniform int NUM_LIGHTS;

in vec2 texCoord;
in vec3 Normal;
in vec3 fragmentPos;
uniform vec3 viewPos;

out vec4 FragColour;

vec3 calcLight(in Light light, in Material material, in vec3 norm, in vec3 fragmentPos, in vec3 viewDir);

void main() 
{
	vec4 texel = texture(tex,texCoord);
	if(texel.a < 0.3) //Discard any low alpha value fragments as a quick fix for transparency with depth testing 
		discard;
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - fragmentPos);
	

	vec3 result;
	for(int i=0; i < NUM_LIGHTS; i++)
	{
		result += calcLight(lights[i], material, norm, fragmentPos, viewDir);
	}
	FragColour =  vec4(result, 1.0);
}

vec3 calcLight(in Light light, in Material material, in vec3 norm, in vec3 fragmentPos, in vec3 viewDir)
{

	//ambient
	vec3 ambient = light.ambient *  material.Ka;

	//diffuse
    vec3 lightDir = normalize(light.pos - fragmentPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * material.Kd;
	diffuse = clamp(diffuse, 0.0, 1.0);

	//specular
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0),material.Ns);
    vec3 specular = light.specular * spec * material.Ks;
	specular = clamp(specular, 0.0, 1.0);

	return vec3(ambient + diffuse + specular);	
}