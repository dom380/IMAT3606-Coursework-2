#version 430

struct Material
{
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	float d;
	float Ns;
	bool used;
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

in Data
{
//FIXES INTEL/NVIDIA SHADER ERROR
	vec2 texCoord;
	vec3 Normal;
	vec3 fragmentPos;
	vec3 Ka;
	vec3 Kd;
	float Shininess;
} inData;

uniform vec3 viewPos;

out vec4 FragColour;

vec3 calcLight(in Light light, in vec3 Ka, in vec3 Kd, in vec3 Ks, in float Shininess, in vec3 norm, in vec3 fragmentPos, in vec3 viewDir);

void main() 
{
	//vec4 texel = texture(tex, inData.texCoord);
	//if(texel.a < 0.3) //Discard any low alpha value fragments as a quick fix for transparency with depth testing 
	//	discard;
	vec3 norm = normalize(inData.Normal);
	vec3 viewDir = normalize(viewPos - inData.fragmentPos);
	vec3 Ka; vec3 Kd; vec3 Ks; float Shininess;
	if(material.used)
	{
		Ka = material.Ka;
		Kd = material.Kd;
		Ks = material.Ks;
		Shininess = material.Ns;
	}
	else
	{ 
		Ka = inData.Ka;
		Kd = inData.Kd;
		Ks = inData.Kd;
		Shininess = inData.Shininess;
	}
	vec3 result = vec3(0,0,0);
	for(int i=0; i < NUM_LIGHTS; i++)
	{
		result += calcLight(lights[i], Ka, Kd, Ks, Shininess, norm, inData.fragmentPos, viewDir);
	}
	FragColour =  vec4(result, 1.0);
}

vec3 calcLight(in Light light, in vec3 Ka, in vec3 Kd, in vec3 Ks, in float Shininess, in vec3 norm, in vec3 fragmentPos, in vec3 viewDir)
{

	//ambient
	vec3 ambient = light.ambient *  Ka;

	//diffuse
    vec3 lightDir = normalize(light.pos - fragmentPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * Kd;
	diffuse = clamp(diffuse, 0.0, 1.0);

	//specular
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), Shininess);
    vec3 specular = light.specular * spec * Ks;
	specular = clamp(specular, 0.0, 1.0);

	return vec3(ambient + diffuse + specular);	
}